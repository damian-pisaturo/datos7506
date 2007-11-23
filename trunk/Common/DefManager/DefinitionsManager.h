#ifndef DEFINITIONSMANAGER_H_
#define DEFINITIONSMANAGER_H_

#include "../../CapaIndices/Common/TipoIndices.h"
#include "../RegisterInfo/RegisterInfo.h"
#include "../Clave/Clave.h"
#include "../TipoDatos.h"
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
		
		typedef std::map<std::string, ListaNombresClaves*> MapaNombresClavesPrimarias;
		
		typedef std::map<std::string, ListaTiposIndices*> MapaTiposIndices;
		
		//Destructor
		virtual ~DefinitionsManager();
		
		//Método para obtener la instancia de este objeto
		static DefinitionsManager& getInstance() { return instance; }
		
		//Devuelve la lista de los tipos de cada atributo de la clase 'nombreTipo'
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
		
		unsigned char getTipoOrgRegistro(const string &nombreTipo) {
			return this->mapaTiposAtributos[nombreTipo]->begin()->tipo;
		}
		
		ListaNombresClaves* getListaNombresClavesPrimarias(const string &nombreTipo) {
			return this->mapaNombresClavesPrimarias[nombreTipo];
		}
		
		ListaNombresAtributos* getListaNombresAtributos(const string &nombreTipo) {
			return this->mapaNombresAtributos[nombreTipo];
		}
		
	private:
		DefinitionsManager();

		static DefinitionsManager instance;
		
		//Atributos
		MapaTiposAtributos mapaTiposAtributos;
		
		MapaTiposIndices mapaTiposIndices;
		
		MapaNombresAtributos mapaNombresAtributos;
		
		MapaNombresClavesPrimarias mapaNombresClavesPrimarias;
				
		//Metodos
		void cargarDefiniciones();
		
		void cargarNombresAtributos();
		
		void cargarTiposAtributos();
		
		void cargarTiposIndices();
		
};



#endif /*DEFINITIONSMANAGER_H_*/
