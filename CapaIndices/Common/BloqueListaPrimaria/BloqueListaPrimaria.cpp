#include "BloqueListaPrimaria.h"



SetClaves* BloqueListaPrimaria::getSetClavesPrimarias(const char* lista, ListaTipos* listaTipos) const {
	
	SetClaves* setClaves = new SetClaves();
	unsigned short offset = this->getOffsetADatos() + Tamanios::TAMANIO_LONGITUD;
	unsigned short cantClaves = *((unsigned short*)(lista + Tamanios::TAMANIO_ESPACIO_LIBRE));
	Clave* clave = NULL;
	
	for (unsigned short i = 0; i < cantClaves; ++i) {
		
		clave = ClaveFactory::getInstance().getClave(lista + offset, *listaTipos);
		
		setClaves->insert(clave);
		
		offset += clave->getTamanioValorConPrefijo() + Tamanios::TAMANIO_LONGITUD;
		
	}
	
	return setClaves;
}

