#ifndef CLAVEFACTORY_H_
#define CLAVEFACTORY_H_

#include "../../Common/DefinitionsManager.h"
#include "Clave.h"
#include <sstream>

class ClaveFactory {
	
	private:
		ClaveFactory() {}
	
		static ClaveFactory instance;
	
	public:
		virtual ~ClaveFactory() {}
	
		static const ClaveFactory& getInstance() { return instance; }
		
		Clave* getClave(const DefinitionsManager::ListaValoresClaves& listaValoresClaves, const ListaTipos& listaTipos) const;

		Clave* getClave(const DefinitionsManager::ListaClaves& listaValoresClaves, const ListaNodos& listaNodos) const;
		
};

#endif /*CLAVEFACTORY_H_*/
