#include "CapaIndices.h"

void crearIndices(const string &nombreTipo, MapaIndices &mapaIndices,
				  DefinitionsManager &defManager) {
	
	Indice* indice;
	unsigned char tipoIndice;
	DefinitionsManager::EstructTipoIndice estructura;
	DefinitionsManager::NodoListaIndices nodoListaIndices;
	DefinitionsManager::ListaTiposIndices *listaTiposIndices = defManager.getListaTiposIndices(nombreTipo);
	DefinitionsManager::ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	
	for (DefinitionsManager::ListaTiposIndices::const_iterator iter = listaTiposIndices->begin();
		iter != listaTiposIndices->end(); ++iter) {
		
		nodoListaIndices = *iter;
		estructura = nodoListaIndices.estructTipoIndice;
		tipoIndice = estructura.tipoEstructura;
			
		switch (tipoIndice) {
			
			case TipoIndices::ARBOL_BP:
			case TipoIndices::ARBOL_BS:
				indice = new IndiceArbol(estructura.tipoIndice, estructura.tamanioBloque, estructura.tipoClave,
										defManager.getListaTipos(nombreTipo), estructura.tipoEstructura, estructura.tamanioBloque,
										estructura.nombreArchivo);
				break;
				
			case TipoIndices::HASH:
				indice = new IndiceHash(listaTiposAtributos, estructura.tamanioBloque, estructura.nombreArchivo);
				break;
		}
		
		mapaIndices[*(nodoListaIndices.listaNombresClaves)] = indice;
		
	}
	
}


void destruirIndices(MapaIndices &mapaIndices) {

	for (MapaIndices::iterator iter = mapaIndices.begin();
		iter != mapaIndices.end(); ++iter)
		delete iter->second;
	
}


char procesarOperacion(unsigned char codOp, const string &nombreTipo,
					   const DefinitionsManager::ListaNombresClaves &listaNombresClaves,
					   const DefinitionsManager::ListaValoresClaves &listaValoresClaves,
					   char *bloqueDatos, ComuDatos &pipe, DefinitionsManager &defManager) {
	
	MapaIndices mapaIndices;
	unsigned char tipoIndice;
	Clave *clave, *claveResultante = NULL;
	Indice* indice;
	//Posibles valores de retorno de las funciones de los índices
	//TODO Ver si hay que reservar memoria para el bloque de datos
	char* bloqueDatosAEnviar;
	SetClaves* setClavesPrimarias; //Conjunto de claves primarias (indice secundario)
	unsigned int nroBloque = 0; //Variable utilizada para almacenar el número de bloque
								//en el cual se debe insertar un resgistro
	
	//Se crean los indices correspondientes al tipo 'nombreTipo'
	crearIndices(nombreTipo, mapaIndices, defManager);
	indice = mapaIndices[listaNombresClaves];
	tipoIndice = indice->getTipo();
	clave = ClaveFactory::getInstance().getClave(listaValoresClaves, *(defManager.getListaTiposClaves(nombreTipo, listaNombresClaves)));
	
	switch(codOp) {
		case OperacionesCapas::INDICES_CONSULTAR:
			if (tipoIndice == TipoIndices::GRIEGO) //Indice Primario
				claveResultante = indice->buscar(clave, bloqueDatosAEnviar);
			else //Indice Secundario
				claveResultante = indice->buscar(clave, setClavesPrimarias);
			//TODO Enviar a la capa de metadata el bloque de datos o la lista de claves primarias
			break;
		case OperacionesCapas::INDICES_CONSULTAR_EXISTENCIA:
			claveResultante = indice->buscar(clave);
			if (claveResultante) {
				//TODO Avisar a la capa de metadata que se encontró la clave
			} else {
				//TODO Avisar a la capa de metadata que no se encontró la clave
			}
			break;
		case OperacionesCapas::INDICES_INSERTAR:
			unsigned short tamRegistro;
			//TODO Leer del pipe el tamaño del registro
			//Se busca un bloque que contenga espacio suficiente para insertar el nuevo registro
			indice->buscarBloqueDestino(tamRegistro, bloqueDatosAEnviar, nroBloque);
			pipe.escribir(nroBloque);
			//TODO Ver donde metemos el tamaño del bloque
			pipe.escribir(bloqueDatosAEnviar, 4096);
			//Recibo el bloque con el registro insertado
			pipe.leer(4096, bloqueDatosAEnviar);
			clave->setReferencia(nroBloque);
			indice->insertar(clave, bloqueDatosAEnviar);
			//TODO Actualizar los indices secundarios :S
			break;
		case OperacionesCapas::INDICES_ELIMINAR:
			indice->eliminar(clave);
			//TODO Actualizar los indices secundarios (no es obligatorio)
			break;
		case OperacionesCapas::INDICES_MODIFICAR:
			//TODO Ver si se trata de una clave primaria o secundaria
			//TODO Leer del pipe el bloque con los datos modificados
			//TODO Comprobar si se modificaron atributos que componen alguna clave
			Clave* claveNueva;
			indice->modificar(clave, claveNueva, bloqueDatosAEnviar);
			break;
	}
	
	destruirIndices(mapaIndices);
	
	return 0;
	
}


int main(int argc, char* argv[]) {
	
	ComuDatos pipe(argv);
	
	unsigned char codOp;
	string nombreTipo, buffer;
	unsigned short tamanioBuffer, bytesLeidos = 0;
	
	pipe.parametro(0, codOp);
	pipe.parametro(1, nombreTipo);
	
	//Leo el tamanio del buffer a recibir
	pipe.leer(&tamanioBuffer);
	
	char *bufferPipe = new char[tamanioBuffer+1];
	
	//Leo los datos provenientes de la capa de metadata
	if (tamanioBuffer <= CodigosPipe::BUFFER_SIZE)
		pipe.leer(tamanioBuffer, bufferPipe);
	else {
		while (bytesLeidos < tamanioBuffer) {
			pipe.leer(CodigosPipe::BUFFER_SIZE, bufferPipe + bytesLeidos);
			bytesLeidos += CodigosPipe::BUFFER_SIZE;
		}
	}
	
	bufferPipe[tamanioBuffer] = 0;
	
	//HARDCODEO A MANO PARA PROBAR
	strcpy(bufferPipe,"clave1");
	bufferPipe[6] = CodigosPipe::COD_FIN_CLAVE;
	strcpy(bufferPipe+7,"clave2");
	bufferPipe[13] = CodigosPipe::COD_FIN_CLAVE;
	strcpy(bufferPipe+14,"bloque");
	//FIN HARDCODEO
	
	buffer = bufferPipe;
	string::size_type posAnterior = 0, posActual = 0;
	DefinitionsManager::ListaValoresClaves listaValoresClaves;
	
	while (posActual != string::npos) {
		posActual = buffer.find(CodigosPipe::COD_FIN_CLAVE, posAnterior);
		if (posActual != string::npos) {
			listaValoresClaves.push_back(buffer.substr(posAnterior, posActual - posAnterior));
			posAnterior = posActual + 1;
		}
	}
	
	//posAnterior queda cargado con la posición donde comienza el bloque de datos (si es que hay)
	
	DefinitionsManager defManager;
	//Ver si la lista de nombres claves se recibe por el pipe o si se hardcodea en el DefManager
	DefinitionsManager::ListaNombresClaves listaNombresClaves;
	char resultado = procesarOperacion(codOp, nombreTipo, listaNombresClaves, listaValoresClaves,
					  				   bufferPipe + posAnterior, pipe, defManager);
	
	
	//TODO Enviar resultado a la capa de metadata!
	
	
	//PRUEBA DEL MINI PARSER
	for (DefinitionsManager::ListaValoresClaves::iterator iter = listaValoresClaves.begin();
	iter != listaValoresClaves.end(); ++iter)
		cout << *iter << endl;
	
	if (posAnterior != string::npos)
		cout << bufferPipe + posAnterior << endl;
	else cout << "no se recibio bloque" << endl;
	//FIN PRUEBA MINI PARSER
	
	delete[] bufferPipe;
	
}
 
