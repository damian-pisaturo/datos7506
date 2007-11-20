#ifndef CLAVEFACTORY_H_
#define CLAVEFACTORY_H_

#include "../DefManager/DefinitionsManager.h"
#include "../Clave/Clave.h"
#include <sstream>

class ClaveFactory {
	
	private:
		ClaveFactory() {}
	
		static ClaveFactory instance;
	
	public:
		virtual ~ClaveFactory() {}
	
		static const ClaveFactory& getInstance() { return instance; }
		
		Clave* getClave(const DefinitionsManager::ListaValoresClaves& listaValoresClaves, const ListaTipos &listaTipos) const;

		Clave* getClave(const DefinitionsManager::ListaClaves& listaValoresClaves, const ListaNodos& listaNodos) const;
		
		Clave* getClave(const char* lista, const ListaTipos& listaTipos) const;
		
};

#endif /*CLAVEFACTORY_H_*/
