#include "BloqueListaPrimaria.h"

void BloqueListaPrimaria::crearLista(char* &lista) const {
	
	lista = new char[this->tamLista];
	unsigned short cantClaves = 0, espLibre = this->tamLista;
	memset(lista, 0, this->tamLista);
	memcpy(lista, &cantClaves, Tamanios::TAMANIO_LONGITUD);
	memcpy(lista + Tamanios::TAMANIO_LONGITUD, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
	
}
	

void BloqueListaPrimaria::crearLista(char* &lista, Clave* clave, const ListaTipos* listaTipos) const {
	
	lista = new char[this->tamLista];
	unsigned short cantClaves = 1, espLibre = this->tamLista - clave->getTamanioValorConPrefijo();
	memset(lista, 0, this->tamLista);
	memcpy(lista, &cantClaves, Tamanios::TAMANIO_LONGITUD);
	memcpy(lista + Tamanios::TAMANIO_LONGITUD, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
	this->insertarClave(lista, clave, listaTipos);
	
}


bool BloqueListaPrimaria::insertarClave(char* lista, Clave* clave, const ListaTipos* listaTipos) const {
	
	unsigned short cantClaves = 0, espLibre = 0, tamClave = clave->getTamanioValorConPrefijo();
	
	memcpy(&cantClaves, lista, Tamanios::TAMANIO_LONGITUD);
	memcpy(&espLibre, lista + Tamanios::TAMANIO_LONGITUD, Tamanios::TAMANIO_ESPACIO_LIBRE);
	
	if (tamClave > espLibre) return false;
	
	char* punteroFinal = lista + this->tamLista - espLibre;
	
	++cantClaves;
	espLibre -= clave->getTamanioValorConPrefijo();
	
	memcpy(lista, &cantClaves, Tamanios::TAMANIO_LONGITUD);
	memcpy(lista + Tamanios::TAMANIO_LONGITUD, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);

	char* buffer = this->serializarClave(clave, listaTipos);
	
	memcpy(punteroFinal, buffer, tamClave);
	
	delete[] buffer;
	
	return true;
}


bool BloqueListaPrimaria::eliminarClave(char* &lista, Clave* clave, const ListaTipos* listaTipos) const {
	
	bool encontrada = false;
	Clave* claveAux = NULL;
	unsigned short offset = Tamanios::TAMANIO_LONGITUD + Tamanios::TAMANIO_ESPACIO_LIBRE;
	
	while (!encontrada) {
		claveAux = ClaveFactory::getInstance().getClave(lista + offset, *listaTipos);
		if (*claveAux == *clave)
			encontrada = true;
		else offset += claveAux->getTamanioValorConPrefijo();
		delete claveAux;
	}
	
	if (encontrada) {
		unsigned short cantClaves = 0, espLibre = 0, espLibreViejo = 0;
		unsigned short tamClave = clave->getTamanioValorConPrefijo();
		
		memcpy(&cantClaves, lista, Tamanios::TAMANIO_LONGITUD);
		memcpy(&espLibre, lista + Tamanios::TAMANIO_LONGITUD, Tamanios::TAMANIO_ESPACIO_LIBRE);
		
		--cantClaves;
		espLibreViejo = espLibre;
		espLibre += tamClave;
		
		char* listaAux = new char[this->tamLista];
		memset(listaAux, 0, this->tamLista);
		
		memcpy(listaAux, &cantClaves, Tamanios::TAMANIO_LONGITUD);
		memcpy(listaAux + Tamanios::TAMANIO_LONGITUD, &espLibre, Tamanios::TAMANIO_ESPACIO_LIBRE);
		memcpy(listaAux + Tamanios::TAMANIO_LONGITUD + Tamanios::TAMANIO_ESPACIO_LIBRE, lista, offset);
		memcpy(listaAux + Tamanios::TAMANIO_LONGITUD + Tamanios::TAMANIO_ESPACIO_LIBRE + offset,
			   lista + offset + tamClave, this->tamLista - offset - tamClave - espLibreViejo);
		
		delete[] lista;
		lista = listaAux;
	}
	
	return encontrada;
}


ListaStrings* BloqueListaPrimaria::convertirAListaStrings(char* lista, const ListaTipos* listaTipos,
							const DefinitionsManager::ListaNombresClaves* listaNombresClaves) const {
	
	unsigned short offset = Tamanios::TAMANIO_LONGITUD + Tamanios::TAMANIO_ESPACIO_LIBRE;
	ListaStrings* listaStrings = new ListaStrings();
	stringstream conversor;
	DefinitionsManager::ListaNombresClaves::const_iterator iterNombres = listaNombresClaves->begin();
	
	for (ListaTipos::const_iterator iterTipos = listaTipos->begin(); iterTipos != listaTipos->end(); ++iterTipos, ++iterNombres){
		
		conversor << *iterNombres << "=";
		
		switch(*iterTipos){
			case TipoDatos::TIPO_BOOL:
			{	
				conversor << *(bool*)(lista + offset);				
				offset += sizeof(bool);
		
			}break;
			
			case TipoDatos::TIPO_CHAR:
			{	
				conversor << *(char*)(lista + offset);				
				offset += sizeof(char);
	
			}break;
			
			case TipoDatos::TIPO_SHORT:
			{
				conversor << *(short*)(lista + offset);				
				offset += sizeof(short);
				
			}break;
			
			case TipoDatos::TIPO_ENTERO:
			{
				conversor << *(int*)(lista + offset);				
				offset += sizeof(int);
				
			}break;
			
			case TipoDatos::TIPO_FLOAT:
			{
				conversor << *(char*)(lista + offset);				
				offset += sizeof(char);
				
			}break;
			
			case TipoDatos::TIPO_FECHA:
			{
				conversor << *(unsigned short*)(lista + offset);
				offset += sizeof(unsigned short);

				conversor << *(unsigned char*)(lista + offset);
				offset += sizeof(unsigned char);
				
				conversor << *(unsigned char*)(lista + offset);
				offset += sizeof(unsigned char);
		
				break;
			}
			
			case TipoDatos::TIPO_STRING:
			{
				unsigned char sizeCadena = *(unsigned char*)(lista + offset);
				offset += sizeof(unsigned char);
				
				string cadena(lista + offset, sizeCadena);		
				conversor << cadena;
				
				offset += sizeCadena;
				
			}break;
			
			conversor << CodigosPipe::COD_FIN_CLAVE;
			listaStrings->push_back(conversor.str());
		}
		conversor.clear();
	}
	
	return listaStrings;
	
}


char* BloqueListaPrimaria::serializarClave(Clave* clave, const ListaTipos* listaTipos) const {
	
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


unsigned short BloqueListaPrimaria::getCantClaves(const char* lista) const {
	
	return *((unsigned short*)lista);
	
}


ListaClaves* BloqueListaPrimaria::getListaClaves(const char* lista, ListaTipos* listaTipos) const {
	
	ListaClaves* listaClaves = new ListaClaves();
	unsigned short offset = Tamanios::TAMANIO_LONGITUD + Tamanios::TAMANIO_ESPACIO_LIBRE;
	unsigned short espLibre = *((unsigned short*)(lista + Tamanios::TAMANIO_LONGITUD));
	const char* punteroFinal = lista + this->tamLista - espLibre;
	Clave* clave = NULL;
	
	while ((lista + offset) < punteroFinal) {
		
		clave = ClaveFactory::getInstance().getClave(lista + offset, *listaTipos);
		
		offset += clave->getTamanioValorConPrefijo();
		
		listaClaves->push_back(clave);
		
	}
	
	return listaClaves;
}

