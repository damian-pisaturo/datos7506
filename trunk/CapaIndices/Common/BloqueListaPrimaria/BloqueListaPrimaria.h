#ifndef BLOQUELISTAPRIMARIA_H_
#define BLOQUELISTAPRIMARIA_H_

#include "../../../Common/ClaveFactory/ClaveFactory.h"
#include "../../../Common/Clave/Clave.h"
#include "../../../Common/Bloque/Bloque.h"
#include "../../../Common/TipoDatos.h"
#include "../../Common/TipoIndices.h"
#include "../../../Common/CodigosPipe.h"
#include <list>
#include <sstream>

typedef list<std::string> ListaStrings;

class BloqueListaPrimaria : public Bloque {
		
	public:
		BloqueListaPrimaria(unsigned short tamLista) : Bloque(0, tamLista) {}
		
		virtual ~BloqueListaPrimaria() {}
/*	
		void crearLista(char* &lista) const;
		
		void crearLista(char* &lista, Clave* clave, const ListaTipos* listaTipos) const;
		
		bool insertarClave(char* lista, Clave* clave, const ListaTipos* listaTipos) const;
		
		bool eliminarClave(char* &lista, Clave* clave, const ListaTipos* listaTipos) const;
		
		ListaStrings* convertirAListaStrings(char* lista, const ListaTipos* listaTipos,
				const DefinitionsManager::ListaNombresClaves* listaNombresClaves) const;
		
		unsigned short getCantClaves(const char* lista) const;
*/		
		ListaClaves* getListaClaves(const char* lista, ListaTipos* listaTipos) const;

		
};

#endif /*BLOQUELISTAPRIMARIA_H_*/
