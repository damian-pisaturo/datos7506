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
		}
		
		mapaIndices[*(nodoListaIndices.listaNombresClaves)] = indice;
		
	}
	
}


char procesarOperacion(unsigned char codOp, const string &nombreTipo,
					   const DefinitionsManager::ListaNombresClaves &listaNombresClaves,
					   const DefinitionsManager::ListaValoresClaves &listaValoresClaves,
					   char *bloqueDatos, char* &bloqueRetorno,
					   DefinitionsManager &defManager) {
	
	MapaIndices mapaIndices;
	
	switch(codOp) {
		case OperacionesCapas::INDICES_CONSULTAR:
			crearIndices(nombreTipo, mapaIndices, defManager);
			break;
		case OperacionesCapas::INDICES_CONSULTAR_EXISTENCIA:
			break;
		case OperacionesCapas::INDICES_BUSCAR_ESPACIO_LIBRE:
			break;
		case OperacionesCapas::INDICES_INSERTAR:
			//Llamada al indice correspondiente
			//dataManager.insertar(nombreTipo, defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos), defManager.getListaTiposAtributos(nombreTipo) );
			break;
		case OperacionesCapas::INDICES_ELIMINAR:
			//llamada al indice correspondiente
			//dataManager.eliminar(nombreTipo, &listaClaves);
			break;
		case OperacionesCapas::INDICES_MODIFICAR:
			//Llamada al indice correspondiente
			//dataManager.modificar(nombreTipo, defManager.getListaValoresAtributos(nombreTipo, mapValoresAtributos), defManager.getListaTiposAtributos(nombreTipo), &listaClaves);			
			break;
	}
	
	return 0;
	
}


int main(int argc, char* argv[]) {
	
	ComuDatos pipe(argv);
	
	unsigned char codOp;
	string nombreTipo, buffer;
	unsigned int tamanioBuffer;
	
	pipe.parametro(0, codOp);
	pipe.parametro(1, nombreTipo);
	
	//Leo el tamanio del buffer a recibir
	pipe.leer(&tamanioBuffer);
	
//	tamanioBuffer = 20;
	char *bufferPipe = new char[tamanioBuffer+1];
	
	//Leo los datos provenientes de la capa de metadata
	pipe.leer(tamanioBuffer, bufferPipe);
	bufferPipe[tamanioBuffer] = 0;
	
	//HARDCODEO A MANO PARA PROBAR
	strcpy(bufferPipe,"clave1");
	bufferPipe[6] = COD_FIN_CLAVE;
	strcpy(bufferPipe+7,"clave2");
	bufferPipe[13] = COD_FIN_CLAVE;
	strcpy(bufferPipe+14,"bloque");
	//FIN HARDCODEO
	
	buffer = bufferPipe;
	string::size_type posAnterior = 0, posActual = 0;
	DefinitionsManager::ListaValoresClaves listaValoresClaves;
	
	while (posActual != string::npos) {
		posActual = buffer.find(COD_FIN_CLAVE, posAnterior);
		if (posActual != string::npos) {
			listaValoresClaves.push_back(buffer.substr(posAnterior, posActual - posAnterior));
			posAnterior = posActual + 1;
		}
	}
	
	//posAnterior queda cargado con la posición donde comienza el bloque de datos (si es que hay)
	
	DefinitionsManager defManager;
	char* bloqueRetorno = NULL;
	char resultado;
	
//	procesarOperacion(codOp, nombreTipo, listaNombresClaves, listaValoresClaves,
//					  bufferPipe + posAnterior, bloqueRetorno, defManager);
	
	
	
	
	
	//PRUEBA DEL MINI PARSER
	for (DefinitionsManager::ListaValoresClaves::iterator iter = listaValoresClaves.begin();
	iter != listaValoresClaves.end(); ++iter)
		cout << *iter << endl;
	
	if (posAnterior != string::npos)
		cout << bufferPipe + posAnterior << endl;
	else cout << "no se recibio bloque" << endl;
	//FIN PRUEBA MINI PARSER
	
	delete[] bufferPipe;
	
	pipe.liberarRecursos();
}
 
