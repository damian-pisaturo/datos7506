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
		
		typedef struct _nodoListaCampos {
			std::string nombreCampo;
			std::string valorCampo;
			char operacion;
		} NodoListaCampos;
		
		typedef std::list<NodoListaCampos> ListaCampos;
		
		typedef struct _estructuraCampos {
			char operacion;
			ListaCampos* listaCampos;
		} EstructuraCampos;
		
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
		
		//Devuelve la lista de los tipos de cada atributo de la clase 'nombreTipo'.
		//NO hay que liberar la memoria de la lista.
		ListaTiposAtributos* getListaTiposAtributos(const string &nombreTipo);
		
		//Devuelve la lista de los tipos de cada atributo de la clase 'nombreTipo',
		//con los campos pk = "true" para aquellos cuyos nombres figuren en 'listaNombresClaves'.
		//HAY que liberar la memoria de la lista.
		ListaTiposAtributos* getListaTiposAtributos(const string &nombreTipo, const ListaNombresClaves &listaNombresClaves);
		
		//Devuelve la lista de los tipos de cada atributo de la clase 'nombreTipo'.
		//HAY que liberar la memoria de la lista.
		ListaTipos* getListaTipos(const string &nombreTipo);
		
		//Devuelve la lista de los tipos de cada valor de una clave.
		//NO hay que liberar la memoria de la lista.
		ListaTipos* getListaTiposClaves(const string &nombreTipo, const ListaNombresClaves &listaNombresClaves);
		
		//Devuelve una lista con los valores de los atributos ordenados segun el archivo de definiciones.
		//En caso que en el mapa no se encuentre algun atributo, el string correspondiente al mismo en la
		//lista quedará vacío.
		//HAY que liberar la memoria de la lista.
		ListaValoresAtributos* getListaValoresAtributos(const string &nombreTipo,
														const MapaValoresAtributos &mapaValoresAtributos);
		
		//Devuelve una lista con todos los datos correspondientes a cada índice de la clase 'nombreTipo'.
		//NO hay que liberar la memoria de la lista.
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
		
		//Devuelve una lista con los nombres de los campos de la clave primaria.
		//NO hay que liberar la memoria de la lista.
		ListaNombresClaves* getListaNombresClavesPrimarias(const string &nombreTipo) {
			return this->mapaNombresClavesPrimarias[nombreTipo];
		}
		
		//Devuelve una lista con los nombres de todos los atributos/campos de la clase 'nombreTipo'.
		//NO hay que liberar la memoria de la lista.
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
		
		bool buscarNombre(const string &nombre, const ListaNombresClaves &listaNombresClaves) const;
		
};



#endif /*DEFINITIONSMANAGER_H_*/
