///////////////////////////////////////////////////////////////////////////
//	Archivo   : DefinitionsManager.h
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase DefinitionsManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef DEFINITIONSMANAGER_H_
#define DEFINITIONSMANAGER_H_

#include "../Clave/Clave.h"
#include "../ArchivoMaestro/ArchivoMaestro.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: DefinitionsManager 
//	(Contiene las definiciones de los tipos de datos declarados en el 
// 	archivo de definiciones, almacenadas en estructuras (mapas, listas)
// 	que pueden ser consultadas)
///////////////////////////////////////////////////////////////////////////
class DefinitionsManager 
{
	// Instancia única del DefinitionsManager
		static DefinitionsManager instance;

	///////////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////////
		ArchivoMaestro archivoMaestro; // Archivo Maestro conocedor del Universo circundante.
		
		// Mapas contenedores de las listas con las definiciones
		// parseadas del archivo maestro.
		MapaTiposAtributos mapaTiposAtributos;		
		MapaTiposIndices mapaTiposIndices;		
		MapaNombresAtributos mapaNombresAtributos;		
		MapaNombresClavesPrimarias mapaNombresClavesPrimarias;
		
	///////////////////////////////////////////////////////////////////////////
	// Constructor (privado)
	///////////////////////////////////////////////////////////////////////////
		DefinitionsManager();
	
	public:
	///////////////////////////////////////////////////////////////////////////
	// Destructor 
	///////////////////////////////////////////////////////////////////////////
		virtual ~DefinitionsManager();
	
	///////////////////////////////////////////////////////////////////////////
	// Métodos publicos
	///////////////////////////////////////////////////////////////////////////
		
		//Método para obtener la instancia de este objeto
		static DefinitionsManager& getInstance() { return instance; }		

		
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
		
		char cargarArchivoMaestro(const string& nomArchivoDefiniciones)
		{
			return this->archivoMaestro.agregarTipo(nomArchivoDefiniciones);
		}
		
		const MapaTiposAtributos& getMapaTiposAtributos() const 
		{
			return this->mapaTiposAtributos;
		}
		
		const MapaTiposIndices& getMapaTiposIndices() const 
		{
			return this->mapaTiposIndices;
		}
		
		const MapaNombresAtributos& getMapaNombresAtributos() const 
		{
			return this->mapaNombresAtributos;
		}
		
		const MapaNombresClavesPrimarias& getMapaNombresClavesPrimarias() const
		{
			return this->mapaNombresClavesPrimarias;
		}
		
		//Devuelve la lista de los tipos de cada atributo de la clase 'nombreTipo',
		//con los campos pk = "true" para aquellos cuyos nombres figuren en 'listaNombresClaves'.
		//HAY que liberar la memoria de la lista.
		ListaTiposAtributos* getListaTiposAtributos(const string &nombreTipo, const ListaNombresClaves &listaNombresClaves);

		//Devuelve la lista de los tipos de cada atributo de la clase 'nombreTipo'.
		//NO hay que liberar la memoria de la lista.
		ListaTiposAtributos* getListaTiposAtributos(const string &nombreTipo);
		
		unsigned char getTipoOrgRegistro(const string &nombreTipo);
		
		unsigned short getTamBloqueDatos(const string& nombreTipo);
		
		unsigned short getTamBloqueLista(const string& nombreTipo);
		
		//Devuelve una lista con los nombres de los campos de la clave primaria.
		//NO hay que liberar la memoria de la lista.
		ListaNombresClaves* getListaNombresClavesPrimarias(const string &nombreTipo);
		
		//Devuelve una lista con los nombres de todos los atributos/campos de la clase 'nombreTipo'.
		//NO hay que liberar la memoria de la lista.
		ListaNombresAtributos* getListaNombresAtributos(const string &nombreTipo);
		
		//Devuelve una lista con todos los datos correspondientes a cada índice de la clase 'nombreTipo'.
		//NO hay que liberar la memoria de la lista.
		ListaTiposIndices* getListaTiposIndices(const string &nombreTipo);
		
		
	private:
	///////////////////////////////////////////////////////////////////////////
	// Método privado
	///////////////////////////////////////////////////////////////////////////
		bool buscarNombre(const string &nombre, const ListaNombresClaves &listaNombresClaves) const;
		char recuperarTipo(const string& nombreTipo);
};

#endif /*DEFINITIONSMANAGER_H_*/
