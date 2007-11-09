#ifndef DEFINITIONSMANAGER_H_
#define DEFINITIONSMANAGER_H_

#include "../CapaIndices/RegisterInfo/RegisterInfo.h"
#include "../CapaIndices/Common/TipoIndices.h"
#include "TipoDatos.h"
#include <list>
#include <map>

class DefinitionsManager {
	
	public:
		//Definiciones
		typedef ListaNodos ListaTiposAtributos;
		
		typedef std::list<std::string> ListaNombresAtributos;
		
		typedef std::list<std::string> ListaValoresAtributos;
		
		typedef struct _nodoListaClaves {
			std::string nombreClave;
			std::string valorClave;
		} NodoListaClaves;
		
		typedef std::list<NodoListaClaves> ListaClaves;
		
		typedef std::list<std::string> ListaNombresClaves;
		
		typedef std::list<std::string> ListaValoresClaves;
		
		typedef struct _estructTipoIndice {
			unsigned char tipoIndice;
			unsigned char tipoEstructura;
			unsigned short tamanioBloque;
			int tipoClave;
			string nombreArchivo;
		} EstructTipoIndice;
		
		typedef struct _nodoListaIndices {
			EstructTipoIndice estructTipoIndice;
			ListaNombresClaves* listaNombresClaves;
			ListaTipos* listaTipos;
		} NodoListaIndices;
		
		typedef std::list<NodoListaIndices> ListaTiposIndices;
		
		typedef std::map<std::string, ListaTiposAtributos*> MapaTiposAtributos;
		
		typedef std::map<std::string, std::string> MapaValoresAtributos;
		
		typedef std::map<std::string, ListaNombresAtributos*> MapaNombresAtributos;
		
		typedef std::map<std::string, ListaTiposIndices*> MapaTiposIndices;
		
		
		//Metodos
		DefinitionsManager();
		
		virtual ~DefinitionsManager();
		
		ListaTiposAtributos* getListaTiposAtributos(const string &nombreTipo);
		
		ListaTipos* getListaTipos(const string &nombreTipo);
		
		ListaTipos* getListaTiposClaves(const string &nombreTipo, const ListaNombresClaves &listaNombresClaves);
		
		//Devuelve una lista con los valores de los atributos ordenados segun el archivo de definiciones.
		//Si 'insercion == true' significa que la lista de valores de atributos se utilizara en una alta,
		//por lo cual el campo 'debeModificar' de cada nodo de la lista tendra el valor false.
		//Si 'insercion == false' significa que la lista de valores de atributos se utilizara en una modificacion,
		//por lo cual el campo 'debeModificar' de cada nodo de la lista que se haya encontrado en el mapa de valores
		//tendra el valor true, y los nodos que no se encontraron en el mapa tendran el valor false.
		ListaValoresAtributos* getListaValoresAtributos(const string &nombreTipo,
															  const MapaValoresAtributos &mapaValoresAtributos);
		
		ListaTiposIndices* getListaTiposIndices(const string &nombreTipo);
		
		const MapaTiposAtributos& getMapaTiposAtributos() const {
			return this->mapaTiposAtributos;
		}
		
		const MapaTiposIndices& getMapaTiposIndices() const {
			return this->mapaTiposIndices;
		}
		
		const MapaNombresAtributos& getMapaNombresAtributos() const {
			return this->mapaNombresAtributos;
		}
		
	private:
		//Atributos
		MapaTiposAtributos mapaTiposAtributos;
		
		MapaTiposIndices mapaTiposIndices;
		
		MapaNombresAtributos mapaNombresAtributos;

		//Metodos
		void cargarDefiniciones();
		
		void cargarNombresAtributos();
		
		void cargarTiposAtributos();
		
		void cargarTiposIndices();
		
};



#endif /*DEFINITIONSMANAGER_H_*/
