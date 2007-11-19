#include "CapaIndices.h"

/*
 * Crea todos los indices correspondientes a un tipo, y carga su mapa de indices.
 **/
void crearIndices(const string &nombreTipo, MapaIndices &mapaIndices,
				  DefinitionsManager &defManager) {
	
	Indice* indice;
	unsigned char tipoIndice;
	DefinitionsManager::EstructTipoIndice estructura;
	DefinitionsManager::NodoListaIndices nodoListaIndices;
	DefinitionsManager::ListaTiposIndices *listaTiposIndices = defManager.getListaTiposIndices(nombreTipo);
	DefinitionsManager::ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	char tipoOrg = (*(listaTiposAtributos->begin())).tipo;
	
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
										Tamanios::TAMANIO_BLOQUE_DATO, estructura.nombreArchivo, tipoOrg);
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


int procesarOperacion(unsigned char codOp, const string &nombreTipo, ComuDatos &pipe) {
	
	int resultado = ResultadosIndices::OK;
	string buffer(""), auxStr("");
	unsigned short tamanioBuffer = 0, bytesLeidos = 0;
	DefinitionsManager& defManager = DefinitionsManager::getInstance();
	DefinitionsManager::ListaNombresClaves listaNombresClaves;
	DefinitionsManager::ListaValoresClaves listaValoresClaves;
	string::size_type posAnterior = 0, posActual = 0, posSeparador = 0;
	
	MapaIndices mapaIndices;
	Clave *clave = NULL;
	Indice* indice = NULL;
	
	char* bloqueDatosAEnviar = NULL;
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
	buffer = bufferPipe; //Copio el buffer en un string para parsearlo con más facilidad
	
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
	clave = ClaveFactory::getInstance().getClave(listaValoresClaves, *(defManager.getListaTiposClaves(nombreTipo, listaNombresClaves)));
	
	switch(codOp) {
		case OperacionesCapas::INDICES_CONSULTAR:
			resultado = indice->buscar(clave, bloqueDatosAEnviar);
			pipe.escribir(resultado);
			if (resultado == ResultadosIndices::OK)
				pipe.escribir(bloqueDatosAEnviar, Tamanios::TAMANIO_BLOQUE_DATO);
			break;
		case OperacionesCapas::INDICES_INSERTAR:
			unsigned short tamRegistro;
			pipe.leer(&tamRegistro);
			//Se busca un bloque que contenga espacio suficiente para insertar el nuevo registro
			resultado = indice->buscarBloqueDestino(tamRegistro, bloqueDatosAEnviar, nroBloque);
			pipe.escribir(resultado);
			if (resultado == ResultadosIndices::REQUIERE_REGISTRO) {
				//El índice es un Hash
				pipe.leer(tamRegistro, bloqueDatosAEnviar);
			} else {
				pipe.escribir(bloqueDatosAEnviar, Tamanios::TAMANIO_BLOQUE_DATO);
				//Recibo el bloque con el registro insertado
				pipe.leer(Tamanios::TAMANIO_BLOQUE_DATO, bloqueDatosAEnviar);				
				clave->setReferencia(nroBloque);
			}
			resultado = indice->insertar(clave, bloqueDatosAEnviar);
			pipe.escribir(resultado);
			break;
		case OperacionesCapas::INDICES_ELIMINAR:
			indice->eliminar(clave);
			break;
		case OperacionesCapas::INDICES_MODIFICAR:
			if (indice->getTipo() == TipoIndices::GRIEGO) {
				indice->buscar(clave, bloqueDatosAEnviar);
				
				
			} else {
				
				
				
			}
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
	
	procesarOperacion(codOp, nombreTipo, pipe);
	
	
	// MÉTODOS DE PRUEBA PARA UN ÁRBOL B+
//	IndiceArbol indice(TipoIndices::GRIEGO, 48, TipoDatos::TIPO_ENTERO, NULL, TipoIndices::ARBOL_BS, 48, "locura");
	
//	char* null = NULL;
/*	
	indice.insertar(new ClaveVariable("hola"), null);
	indice.insertar(new ClaveVariable("chau"), null);
	indice.insertar(new ClaveVariable("mundo"), null);
	indice.insertar(new ClaveVariable("aleee"), null);
	indice.insertar(new ClaveVariable("nico"), null);
	indice.insertar(new ClaveVariable("manu"), null);
	indice.insertar(new ClaveVariable("moe"), null);
	indice.insertar(new ClaveVariable("lau"), null);
	indice.insertar(new ClaveVariable("tifi"), null);
	indice.insertar(new ClaveVariable("yo"), null);
	indice.insertar(new ClaveVariable("hola mundo"), null);
*/
/*	
	stringstream texto;	
	for (unsigned i = 0; i < 300; ++i) {
		texto << "hola" << i;
		cout << "inserto: " << texto.str() << endl;
		indice.insertar(new ClaveVariable(texto.str()), null);
		texto.str("");
	}
*/
/*	
	ClaveVariable clave("lau");
	indice.eliminar(&clave);
	
	indice.insertar(new ClaveVariable("gollum"), null);
	
	ClaveVariable clave2("chau");
	indice.eliminar(&clave2);
*/
/*	
	indice.insertar(new ClaveEntera(20), null);
	indice.insertar(new ClaveEntera(90), null);
	indice.insertar(new ClaveEntera(49), null);
	indice.insertar(new ClaveEntera(33), null);
	indice.insertar(new ClaveEntera(57), null);
	indice.insertar(new ClaveEntera(30), null);
	indice.insertar(new ClaveEntera(17), null);
	
	indice.insertar(new ClaveEntera(8), null);
	indice.insertar(new ClaveEntera(10), null);
	indice.insertar(new ClaveEntera(1), null);
	indice.insertar(new ClaveEntera(7), null);
	indice.insertar(new ClaveEntera(86), null);
*/
/*	
	ClaveEntera clave(456);
	indice.eliminar(&clave);
*/
	
	cout << "Fin Main" << endl;
	
}

