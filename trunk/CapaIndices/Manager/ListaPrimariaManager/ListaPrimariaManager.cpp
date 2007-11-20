#include "ListaPrimariaManager.h"

void ListaPrimariaManager::crearLista(char* &lista) const {
	
	lista = new char[this->tamLista];
	unsigned short cantClaves = 0, espLibre = this->tamLista;
	memset(lista, 0, this->tamLista);
	memcpy(lista, &cantClaves, Tamanios::TAMANIO_LONGITUD);
	memcpy(lista + Tamanios::TAMANIO_LONGITUD, &espLibre, Tamanios::TAMANIO_LONGITUD);
	
}
	

void ListaPrimariaManager::crearLista(char* &lista, Clave* clave, const ListaTipos* listaTipos) const {
	
	lista = new char[this->tamLista];
	unsigned short cantClaves = 1, espLibre = this->tamLista - clave->getTamanioValorConPrefijo();
	memset(lista, 0, this->tamLista);
	memcpy(lista, &cantClaves, Tamanios::TAMANIO_LONGITUD);
	memcpy(lista + Tamanios::TAMANIO_LONGITUD, &espLibre, Tamanios::TAMANIO_LONGITUD);
	this->insertarClave(lista, clave, listaTipos);
	
}


bool ListaPrimariaManager::insertarClave(char* lista, Clave* clave, const ListaTipos* listaTipos) const {
	
	unsigned short cantClaves = 0, espLibre = 0, tamClave = clave->getTamanioValorConPrefijo();
	
	memcpy(&cantClaves, lista, Tamanios::TAMANIO_LONGITUD);
	memcpy(&espLibre, lista + Tamanios::TAMANIO_LONGITUD, Tamanios::TAMANIO_LONGITUD);
	
	if (tamClave > espLibre) return false;
	
	char* punteroFinal = lista + this->tamLista - espLibre;
	
	++cantClaves;
	espLibre -= clave->getTamanioValorConPrefijo();
	
	memcpy(lista, &cantClaves, Tamanios::TAMANIO_LONGITUD);
	memcpy(lista + Tamanios::TAMANIO_LONGITUD, &espLibre, Tamanios::TAMANIO_LONGITUD);

	char* buffer = this->serializarClave(clave, listaTipos);
	
	memcpy(punteroFinal, buffer, tamClave);
	
	delete[] buffer;
	
	return true;
}


bool ListaPrimariaManager::eliminarClave(char* &lista, Clave* clave, const ListaTipos* listaTipos) const {
	
	char* buffer = this->serializarClave(clave, listaTipos);
	
	char* claveBuscada = strstr(lista, buffer);
	
	//TODO Ver como localizar la clave dentro de la lista
	
	delete[] buffer;
	
	if (!claveBuscada) return false;
	
	return true;
}


ListaStrings* ListaPrimariaManager::convertirAListaStrings(char* lista, const ListaTipos* listaTipos) const {
	
	return NULL;
	
}


char* ListaPrimariaManager::serializarClave(Clave* clave, const ListaTipos* listaTipos) const {
	
	char* buffer = new char[clave->getTamanioValorConPrefijo()];
	
	ListaClaves listaClaves;
	if (listaTipos->size() > 1) //'clave' es una clave compuesta
		listaClaves = *(((ClaveCompuesta*)clave)->getListaClaves());
	else listaClaves.push_back(clave);

	ListaClaves::const_iterator iterClaves;
	ListaTipos::const_iterator iterTipos;
	Clave* claveAux = NULL;
	char* cadena = NULL;
	unsigned int sizeCadena = 0;
	
	for (iterClaves = listaClaves.begin(), iterTipos = listaTipos->begin();
		(iterClaves != listaClaves.end()) && (iterTipos != listaTipos->end());
		++iterClaves, ++iterTipos) {	
		
		claveAux = *iterClaves;
		
		if (*iterTipos == TipoDatos::TIPO_STRING) {
			
			cadena = (char*)claveAux->getValor();
			sizeCadena = claveAux->getTamanioValor();
		
			memcpy(buffer, &sizeCadena, Tamanios::TAMANIO_LONGITUD_CADENA);
			memcpy(buffer + Tamanios::TAMANIO_LONGITUD_CADENA, cadena, sizeCadena);
			
		} else memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
		
		buffer += claveAux->getTamanioValorConPrefijo();
	}
	
	return buffer;
	
}

