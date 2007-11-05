#ifndef CLAVEFACTORY_H_
#define CLAVEFACTORY_H_

#include "DefinitionsManager.h"
#include "Clave.h"

class ClaveFactory {
	
	private:
		ClaveFactory() {}
	
		static ClaveFactory instance;
	
	public:
		virtual ~ClaveFactory() {}
	
		static ClaveFactory& getInstance() { return instance; }
		
		Clave* getClave(const ListaClaves &listaClaves) const;

		
};

#endif /*CLAVEFACTORY_H_*/
