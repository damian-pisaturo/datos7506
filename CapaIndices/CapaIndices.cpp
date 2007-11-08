#include "CapaIndices.h"

void crearIndice(const string &nombreTipo, MapaIndices &mapaIndices,
				const DefinitionsManager::NodoListaIndices &nodo,
				DefinitionsManager &defManager) {
	
	Indice* indice;
	DefinitionsManager::ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	DefinitionsManager::EstructTipoIndice estructura = nodo.estructTipoIndice;
	
	
	switch (estructura.tipoEstructura) {
	
		case TipoIndices::ARBOL_BP:
		case TipoIndices::ARBOL_BS:	
			indice = new IndiceArbol(estructura.tipoIndice, estructura.tamanioBloque, estructura.tipoClave,
									defManager.getListaTipos(nombreTipo), estructura.tipoEstructura, estructura.tamanioBloque,
									estructura.nombreArchivo);
			break;
			
		case TipoIndices::HASH:
			indice = new IndiceHash(listaTiposAtributos, estructura.tamanioBloque, estructura.nombreArchivo);
	}	
	mapaIndices[*(nodo.listaNombresClaves)] = indice;
}


void procesarOperacion(unsigned char codOp, const string &nombreTipo,
					   const DefinitionsManager::ListaValoresClaves &listaValoresClaves, char *bloqueDatos) {
	
	
	
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
	
	procesarOperacion(codOp, nombreTipo, listaValoresClaves, bufferPipe + posAnterior);
	
	
	
	
	
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
 
