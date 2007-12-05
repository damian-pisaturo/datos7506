#ifndef COMMONDEFINITIONS_H_
#define COMMONDEFINITIONS_H_

#include "Tamanios.h"
#include <vector>
#include <string>
#include <list>
#include <map>
#include <iostream>

using namespace std;

/////////////////////////////////////////////////////////
// Definiciones de tipos de datos comunes
/////////////////////////////////////////////////////////	
	typedef list<string> ListaNombresAtributos;
		
	typedef list<string> ListaValoresAtributos;
	
	typedef list<string> ListaNombresClaves;
			
	typedef list<string> ListaValoresClaves;
	
	typedef list<string> ListaStrings;
	
	typedef list<unsigned char> ListaTipos;
	
	typedef vector<unsigned char> VectorTiposAtributos;
	
	typedef vector<unsigned char> VectorTiposIndices;
	
	typedef vector<unsigned short> VectorTamaniosBloques;
	
	typedef vector<ListaNombresClaves*> VectorNombresClaves;


/////////////////////////////////////////////////////////
// Estructuras utilizadas en las consultas
/////////////////////////////////////////////////////////	

	typedef struct _nodoListaCampos {
		string nombreCampo;
		string valorCampo;
		char operacion;
	} NodoListaCampos;
	
	typedef list<NodoListaCampos> ListaCampos;
	
	typedef struct _estructuraCampos {
		char operacion;
		ListaCampos listaCampos;
	} EstructuraCampos;
	
	typedef struct _estructuraNombres {
		string nombreTipo;
		string nombreCampo;
	} EstructuraNombres;
	
	typedef struct _nodoListaOperaciones {
		EstructuraNombres estructuraNombresIzq;
		EstructuraNombres estructuraNombresDer;
		char operacion;
	} NodoListaOperaciones;
	
	typedef list<NodoListaOperaciones> ListaOperaciones;
	
	typedef struct _estructuraOperaciones {
		char operacion;
		ListaOperaciones listaOperaciones;
	} EstructuraOperaciones;	
	
	typedef list<EstructuraNombres> ListaEstructuraNombres;
	
	typedef struct _estructuraConsulta {
		ListaStrings listaNombresTipos;
		ListaEstructuraNombres listaCamposSeleccionados;
		EstructuraOperaciones estructuraJoins;
		EstructuraOperaciones estructuraWhere;
		ListaEstructuraNombres listaOrderBy;
	} EstructuraConsulta;
	
///////////////////////////////////////////////////////////////////////////
// Clases
//------------------------------------------------------------------------
// Nombres: NodoInfoRegistro 
//	(Definen nodos de la lista contenedora de la información específica
//	de un registro (tipo de datos, cantidad de claves primarias, etc.)
///////////////////////////////////////////////////////////////////////////
	class NodoInfoRegistro //nodo de la antigua ListaLoca
	{	
		public:
			// Si se trata del primero nodo de la lista de 
			// información de registros, tipoDato valdrá
			// TIPO_VARIABLE o TIPO_FIJO. En cualquier otro
			// caso, contendrá el tipo de dato correspondiente
			// al atributo actual (TIPO_ENTERO, TIPO_CHAR, etc).
			unsigned char tipoDato;
			
			// Si tipoDatos especifica que el registro es de 
			// TIPO_FIJO, tamRegistro contendrá el size del
			// registro. De lo contrario, valdrá 0.
			unsigned short tamRegistro; 
			
			// Cantidad de claves primarias dentro del registro.
			unsigned char cantClaves;
			
			// Indice si el atributo actual cuyo tipo
			// es indicado por tipoDato es clave primaria
			// o no.
			bool esPk;

		////////////////////////////////////////
		// Constructor/Destructor
		////////////////////////////////////////
			NodoInfoRegistro(): 
				tipoDato(0),
				tamRegistro(0),
				cantClaves(0),
				esPk(false)
			{ }
			
			NodoInfoRegistro(const NodoInfoRegistro& infoReg)
			{
				this->tipoDato    = infoReg.tipoDato;
				this->tamRegistro = infoReg.tamRegistro;
				this->cantClaves  = infoReg.cantClaves;
				this->esPk        = infoReg.esPk;
			}
			
			virtual ~NodoInfoRegistro() 
			{ }
		
		////////////////////////////////////////
		// Metodo publico
		////////////////////////////////////////
			NodoInfoRegistro& operator = (const NodoInfoRegistro& infoReg)
			{
				this->tipoDato    = infoReg.tipoDato;
				this->tamRegistro = infoReg.tamRegistro;
				this->cantClaves  = infoReg.cantClaves;
				this->esPk        = infoReg.esPk;
				
				return *this;
			}
		}; /* Fin clase NodoInfoRegistro */

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombres: NodoTipoIndice 
//	(Define el nodo de la estructura que almacena los diversos tipos de
//	 indices definidos en el archivo de definiciones).
///////////////////////////////////////////////////////////////////////////
	class InfoIndice //antiguo EstructTipoIndice
	{
		public:
		////////////////////////////////////////
		// Atributos públicos
		////////////////////////////////////////
			unsigned char tipoIndice;
			unsigned char tipoEstructura;
			unsigned short tamanioBloque;
			unsigned char tipoClave;
			string nombreArchivo;
			
		////////////////////////////////////////
		// Constructor/Destructor
		////////////////////////////////////////
			InfoIndice():
				tipoIndice(0), 
				tipoEstructura(0),
				tamanioBloque(0),
				tipoClave(0)
			{ }
			
			virtual ~InfoIndice()
			{ }
		
		////////////////////////////////////////
		// Metodo publico
		////////////////////////////////////////
			InfoIndice& operator = (const InfoIndice& nodoIndice)
			{
				this->tipoIndice     = nodoIndice.tipoIndice;
				this->tipoEstructura = nodoIndice.tipoEstructura;
				this->tamanioBloque  = nodoIndice.tamanioBloque;
				this->tipoClave      = nodoIndice.tipoClave;
				this->nombreArchivo  = nodoIndice.nombreArchivo;
				
				return *this;
			}
			
	}; /* Fin clase InfoIndice */
	
	class NodoListaIndices
	{
		public:
		////////////////////////////////////////
		// Atributos publicos
		////////////////////////////////////////
			InfoIndice infoTipoIndice;
			ListaNombresClaves* listaNombresClaves;
			ListaTipos* listaTipos;
		
		////////////////////////////////////////
		// Constructor/Destructor
		////////////////////////////////////////
			NodoListaIndices()
			{ 
				this->listaNombresClaves = new ListaNombresClaves();
				this->listaTipos         = new ListaTipos();
			}
			
			NodoListaIndices(const NodoListaIndices& infoIndice)
			{
				this->infoTipoIndice     = infoIndice.infoTipoIndice;
				this->listaNombresClaves = new ListaNombresClaves(*infoIndice.listaNombresClaves);
				this->listaTipos         = new ListaTipos(*infoIndice.listaTipos);
			}
			
			virtual ~NodoListaIndices()
			{
				delete this->listaNombresClaves;
				delete this->listaTipos;
			}
			
		////////////////////////////////////////
		// Metodo publico
		////////////////////////////////////////
			NodoListaIndices& operator = (const NodoListaIndices& infoIndice)
			{
				this->infoTipoIndice = infoIndice.infoTipoIndice;
				
				if (this->listaNombresClaves)
					delete this->listaNombresClaves;
				
				this->listaNombresClaves = new ListaNombresClaves(*infoIndice.listaNombresClaves);
				
				if (this->listaTipos)
					delete this->listaTipos;
				
				this->listaTipos = new ListaTipos(*infoIndice.listaTipos);
				
				return *this;
			}
			
	}; /* Fin clase NodoListaIndices */
	
	class NodoListaClaves
	{
		public:
		////////////////////////////////////////
		// Atributos publicos
		////////////////////////////////////////
			string nombreClave;
			string valorClave;
		
		////////////////////////////////////////
		// Constructor/Destructor
		////////////////////////////////////////
			NodoListaClaves()
			{ }
			
			virtual ~NodoListaClaves()
			{ }
			
		////////////////////////////////////////
		// Metodo publico
		////////////////////////////////////////
			NodoListaClaves& operator = (NodoListaClaves& nodoListaClaves)
			{
				this->nombreClave = nodoListaClaves.nombreClave;
				this->valorClave = nodoListaClaves.valorClave;
				
				return *this;
			}
	
	}; /* Fin clase NodoListaClaves*/
	
	class DatosIndices
	{
		public:
		////////////////////////////////////////
		// Atributos publicos
		////////////////////////////////////////
			VectorTiposIndices* vTiposIndices;
			VectorTamaniosBloques* vTamBloques;
			VectorNombresClaves* vNomClaves;
		
		////////////////////////////////////////
		// Constructor/Destructor
		////////////////////////////////////////
			DatosIndices()
			{
				this->vTiposIndices = new VectorTiposIndices();
				this->vTamBloques   = new VectorTamaniosBloques();
				this->vNomClaves    = new VectorNombresClaves();
			}
			
			DatosIndices(DatosIndices& datosIndices)
			{
				this->vTiposIndices = new VectorTiposIndices(*datosIndices.vTiposIndices);
				this->vTamBloques   = new VectorTamaniosBloques(*datosIndices.vTamBloques);
				
				this->vNomClaves    = new VectorNombresClaves();
				
				VectorNombresClaves::const_iterator iter;
				for (iter = datosIndices.vNomClaves->begin(); iter != datosIndices.vNomClaves->end(); ++iter)
					this->vNomClaves->push_back(new ListaNombresClaves(*(*iter)));
			}
			
			virtual ~DatosIndices()
			{
				delete this->vTiposIndices;
				this->vTiposIndices = NULL;
				
				delete this->vTamBloques;
				this->vTamBloques = NULL;
				
				VectorNombresClaves::const_iterator iter;
				for (iter = this->vNomClaves->begin(); iter != this->vNomClaves->end(); ++iter)
					delete (*iter);			
				
				delete this->vNomClaves;
				this->vNomClaves = NULL;
			}		
			
			DatosIndices& operator = (DatosIndices& datosIndices)
			{
				if (this->vTiposIndices) delete this->vTiposIndices;
				this->vTiposIndices = new VectorTiposIndices(*datosIndices.vTiposIndices);
				
				if (this->vTamBloques) delete this->vTamBloques;
				this->vTamBloques = new VectorTamaniosBloques(*datosIndices.vTamBloques);
				
				VectorNombresClaves::const_iterator iter;
				if (this->vNomClaves){
					for (iter = this->vNomClaves->begin(); iter != this->vNomClaves->end(); ++iter)
						delete *iter;
					
					delete this->vNomClaves;
				}
				
				this->vNomClaves = new VectorNombresClaves();
				for (iter = datosIndices.vNomClaves->begin(); iter != datosIndices.vNomClaves->end(); ++iter)
					this->vNomClaves->push_back(new ListaNombresClaves(*(*iter)));
					
				return *this;
			}
	}; /*Fin clase DatosIndices*/
	
	class DefinicionTipo
	{
		public:
		////////////////////////////////////////
		// Atributos publicos
		////////////////////////////////////////
			string nombreTipo;
			unsigned char tipoOrganizacion;
			ListaNombresAtributos* nombresAtributos;
			VectorTiposAtributos* tiposAtributos;
			DatosIndices* datosIndicesGriegos;
			DatosIndices* datosIndicesRomanos;
		
		////////////////////////////////////////
		// Constructores/Destructor
		////////////////////////////////////////
			DefinicionTipo()
			{
				this->nombresAtributos = new ListaNombresAtributos();
				this->tiposAtributos = new VectorTiposAtributos();
				this->datosIndicesGriegos = new DatosIndices();
				this->datosIndicesRomanos = new DatosIndices(); 
			}
			
			DefinicionTipo(const DefinicionTipo& defTipo)
			{
				this->nombreTipo = defTipo.nombreTipo;
				this->tipoOrganizacion = defTipo.tipoOrganizacion;
				this->nombresAtributos = new ListaNombresAtributos(*defTipo.nombresAtributos);
				this->tiposAtributos   = new VectorTiposAtributos(*defTipo.tiposAtributos);
				this->datosIndicesGriegos = new DatosIndices(*defTipo.datosIndicesGriegos);
				this->datosIndicesRomanos = new DatosIndices(*defTipo.datosIndicesRomanos);
			}
			
			virtual ~DefinicionTipo()
			{
				delete this->nombresAtributos;
				this->nombresAtributos = NULL;
				
				delete this->tiposAtributos;
				this->tiposAtributos = NULL;
				
				delete this->datosIndicesGriegos;
				this->datosIndicesGriegos = NULL;
				
				delete this->datosIndicesRomanos;
				this->datosIndicesRomanos = NULL;
			}
			
			////////////////////////////////////////
			// Metodo publico
			////////////////////////////////////////
			DefinicionTipo& operator = (DefinicionTipo& defTipo)
			{
				this->nombreTipo       = defTipo.nombreTipo;
				this->tipoOrganizacion = defTipo.tipoOrganizacion;
				
				if (this->nombresAtributos) delete this->nombresAtributos;
				this->nombresAtributos = new ListaNombresAtributos(*defTipo.nombresAtributos);
				
				if (this->tiposAtributos) delete this->tiposAtributos;
				this->tiposAtributos = new VectorTiposAtributos(*defTipo.tiposAtributos);
				
				if (this->datosIndicesGriegos) delete this->datosIndicesGriegos;
				this->datosIndicesGriegos = new DatosIndices(*defTipo.datosIndicesGriegos);
				
				if (this->datosIndicesRomanos) delete this->datosIndicesRomanos;
				this->datosIndicesRomanos = new DatosIndices(*defTipo.datosIndicesRomanos);
				
				return *this;
			}	
	}; /* Fin clase DefinicionTipo*/


	/////////////////////////////////////////////////////////
	// Definiciones de tipos comunes que emplean las 
	// estructuras declaradas anteriormente.
	/////////////////////////////////////////////////////////	
		typedef list<DefinicionTipo> ListaDefinicionesTipos;
		
		typedef list<NodoInfoRegistro> ListaInfoRegistro;
		
		typedef ListaInfoRegistro ListaTiposAtributos;	
		
		typedef list<NodoListaClaves> ListaInfoClave;		
		
		typedef list<NodoListaIndices> ListaTiposIndices;		
		
		typedef map<string, ListaTiposAtributos*> MapaTiposAtributos;
		
		typedef map<string, string> MapaValoresAtributos;
		
		typedef map<string, ListaNombresAtributos*> MapaNombresAtributos;
		
		typedef map<string, ListaNombresClaves*> MapaNombresClavesPrimarias;
		
		typedef map<string, ListaTiposIndices*> MapaTiposIndices;
		
#endif /*COMMONDEFINITIONS_H_*/
