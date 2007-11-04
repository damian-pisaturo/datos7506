#ifndef DEFINITIONSMANAGER_H_
#define DEFINITIONSMANAGER_H_

#include "../RegisterInfo/RegisterInfo.h"
#include <list>
#include <map>

class DefinitionsManager {
	
	public:
		//Definiciones
		typedef ListaNodos ListaTiposAtributos;
		
		typedef std::list<std::string> ListaValoresAtributos;
		
		typedef struct _nodoListaClaves {
			string nombreClave;
			string valorClave;
		} NodoListaClaves;
		
		typedef std::list<NodoListaClaves> ListaClaves;
		
		typedef std::list<string> ListaNombresClaves;
		
		typedef struct _estructTipoIndice {
			unsigned char tipoIndice;
			unsigned char tipoEstructura;
			unsigned short tamanioBloque;
		} EstructTipoIndice;
		
		typedef struct _nodoListaIndices {
			EstructTipoIndice estructTipoIndice;
			ListaNombresClaves* listaNombresClaves;
		} NodoListaIndices;
		
		typedef std::list<NodoListaIndices> ListaTiposIndices;
		
		typedef std::map<std::string, ListaTiposAtributos*> MapaTiposAtributos;
		
		typedef std::map<std::string, std::string> MapaValoresAtributos;
		
		typedef std::map<std::string, ListaTiposIndices*> MapaTiposIndices;
		
		//Metodos
		DefinitionsManager();
		
		virtual ~DefinitionsManager();
		
	private:
		//Atributos
		MapaTiposAtributos mapaTiposAtributos;
		
		MapaTiposIndices mapaTiposIndices;

		//Metodos
		void cargarDefiniciones();
		
};



#endif /*DEFINITIONSMANAGER_H_*/
