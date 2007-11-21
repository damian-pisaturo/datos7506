#ifndef LISTAPRIMARIAMANAGER_H_
#define LISTAPRIMARIAMANAGER_H_

#include "../../../Common/ClaveFactory/ClaveFactory.h"
#include "../../../Common/Clave/Clave.h"
#include "../../../Common/TipoDatos.h"
#include "../../Common/TipoIndices.h"
#include "../../../Common/CodigosPipe.h"
#include <list>
#include <sstream>

typedef list<std::string> ListaStrings;

class ListaPrimariaManager {
	
	private:
		unsigned short tamLista;
		
	public:
		ListaPrimariaManager(unsigned short tamLista) : tamLista(tamLista) {}
		
		virtual ~ListaPrimariaManager() {}
		
		void crearLista(char* &lista) const;
		
		void crearLista(char* &lista, Clave* clave, const ListaTipos* listaTipos) const;
		
		bool insertarClave(char* lista, Clave* clave, const ListaTipos* listaTipos) const;
		
		bool eliminarClave(char* &lista, Clave* clave, const ListaTipos* listaTipos) const;
		
		ListaStrings* convertirAListaStrings(char* lista, const ListaTipos* listaTipos,
				const DefinitionsManager::ListaNombresClaves* listaNombresClaves) const;
		
		unsigned short getCantClaves(const char* lista) const;
		
		unsigned short getTamanioLista() const { return this->tamLista; }
		
		ListaClaves* getListaClaves(const char* lista, ListaTipos* listaTipos) const;
		
		void setTamanioLista(unsigned short tamLista) { this->tamLista = tamLista; }
		
	private:
		char* serializarClave(Clave* clave, const ListaTipos* listaTipos) const;
		
};

#endif /*LISTAPRIMARIAMANAGER_H_*/
