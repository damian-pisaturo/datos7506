#ifndef INDICEMANAGERFACTORY_H_
#define INDICEMANAGERFACTORY_H_

#include "../Manager/IndiceManager/IndiceManager.h"
#include "../../Common/TipoDatos.h"

class IndiceManagerFactory {
	private:
		IndiceManagerFactory() {}
	
		static IndiceManagerFactory instance;
	
	public:
		virtual ~IndiceManagerFactory() {}
	
		static IndiceManagerFactory& getInstance() { return instance; }
		
		IndiceManager* getIndiceManager(unsigned char tipoIndice,
										int tipoDato, ListaTipos* listaTipos,
										unsigned char tipoEstructura, unsigned short tamNodo,
										unsigned int tamBucket, const string& nombreArchivo) const;
};

#endif /*INDICEMANAGERFACTORY_H_*/
