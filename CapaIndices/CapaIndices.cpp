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


char procesarOperacion(unsigned char codOp, const string &nombreTipo, ComuDatos &pipe) {
	
	string buffer(""), auxStr("");
	unsigned short tamanioBuffer = 0, bytesLeidos = 0;
	DefinitionsManager defManager;
	DefinitionsManager::ListaNombresClaves listaNombresClaves;
	DefinitionsManager::ListaValoresClaves listaValoresClaves;
	string::size_type posAnterior = 0, posActual = 0, posSeparador = 0;
	
	MapaIndices mapaIndices;
	unsigned char tipoIndice;
	Clave *clave, *claveResultante = NULL;
	Indice* indice;
	//Posibles valores de retorno de las funciones de los índices
	//TODO Ver si hay que reservar memoria para el bloque de datos
	char* bloqueDatosAEnviar;
	SetClaves* setClavesPrimarias; //Conjunto de claves primarias (indice secundario)
	unsigned int nroBloque = 0; //Variable utilizada para almacenar el número de bloque
								//en el cual se debe insertar un registro
	
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
	buffer = bufferPipe;
	
	//Se parsea el buffer obteniendo los nombres de la claves y sus valores correspondientes (NOMBRE_CLAVE=VALOR_CLAVE)
	while (posActual != string::npos) {
		posActual = buffer.find(CodigosPipe::COD_FIN_CLAVE, posAnterior);
		if (posActual != string::npos) {
			auxStr = buffer.substr(posAnterior, posActual - posAnterior); //En auxStr tengo NOMBRE_CLAVE=VALOR_CLAVE
			posSeparador = auxStr.find(SEPARADOR);
			listaNombresClaves.push_back(auxStr.substr(0, posSeparador));
			listaValoresClaves.push_back(auxStr.substr(posSeparador));
			posAnterior = posActual + 1;
		}
	}
	
	
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
	
	delete[] bufferPipe;
	
	return 0;
	
}


int main(int argc, char* argv[]) {
	
	ComuDatos pipe(argv);
	unsigned char codOp;
	string nombreTipo;
	
	pipe.parametro(0, codOp);
	pipe.parametro(1, nombreTipo);
	
	char resultado = procesarOperacion(codOp, nombreTipo, pipe);
	
	//TODO Enviar resultado a la capa de metadata!
	pipe.escribir(resultado);
	
	/* MÉTODOS DE PRUEBA PARA UN ÁRBOL B+
	IndiceArbol indice(TipoIndices::GRIEGO, 512, TipoDatos::TIPO_ENTERO, NULL, TipoIndices::ARBOL_BP, 512, "locura");
	
	indice.insertar(new ClaveEntera(346), NULL);
	indice.insertar(new ClaveEntera(9870), NULL);
	indice.insertar(new ClaveEntera(2345), NULL);
	indice.insertar(new ClaveEntera(123), NULL);
	indice.insertar(new ClaveEntera(456), NULL);
	indice.insertar(new ClaveEntera(789), NULL);

	cout << "Fin Main" << endl;
	*/
}

