///////////////////////////////////////////////////////////////////////////
//	Archivo   : Nodo.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases Nodo, NodoBp y NodoBstar.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel.
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef NODO_H_
#define NODO_H_

#include "Lista.h"
#include "Codigo.h"

class Clave;
class ListaClaves;
class Codigo;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Nodo (Abstracta)
//////////////////////////////////////////////////////////////////////////
class Nodo
{
	private:
    //////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////
		
		/*Header del Nodo
		 * Datos que no son claves 
		 * pero que se almacena en el archivo
		 */
		int hijoIzq;
		unsigned int nivel;
		unsigned int espacioLibre;
		unsigned int posicionEnArchivo;
		ListaClaves* claves; //TODO Modificar por el set de Dami.
	
	public:
	///////////////////////////////////////////////////////////////////////
    // Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Crea el nodo sin setear ningun atributo*/
		Nodo(int hijoIzq, int nivel);
		
		/*Crea un nuevo nodo para insertarle una Clave*/
		Nodo(int hijoIzq,int nivel,Clave* clave);
		
		/*Lee el archivo y crea ese nodo*/
		Nodo(int referencia);
		
		/*Destructor*/
		virtual ~Nodo();
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		/*insertarClave()
		 * Inserta la clave pasada en el nodo corriente.
		 * Devuelve un Codigo dependiendo del resultado:
		 * 	Codigo::OK - Insercion llevada a cabo correctamente.
		 *  Codigo::OVERFLOW - Sobreflujo de claves.
		 * */
		virtual char insertarClave(Clave* &clave) = 0;
		//TODO insertarEnHoja e insertarEnNodo debieran ser casos particulares del
		//arbol B+. Dejo comentadas sus cabeceras para futura referencia.
		
		//void insertarEnHoja(ArchivoIndice* archivoIndice,Clave* &clave,Codigo* codigo);
		//void insertarEnNodo(ArchivoIndice* archivoIndice,Clave* &clave,Codigo* codigo);
		
		
		/*siguiente()
		 * */
		virtual Nodo* siguiente(Clave* clave); //TODO NADIE entiende este metodo. Creo que se usa en la gesta de embriones macho de Vicuña de la Pampa Humeda.
		
		/*eliminarClave()
			 * Elimina la clave pasada en el nodo corriente.
			 * Devuelve un Codigo dependiendo del resultado:
			 * 	Codigo::OK - Insercion llevada a cabo correctamente.
			 *  Codigo::UNDERFLOW - Subflujo de claves.
			 * */		
		virtual char eliminarClave(Clave* clave);
		
		//TODO Aparentemente este metodo puede fletarse.
		//bool soportaSplit(Nodo* nodoConSubflow);
		
		/*actualizarEspacioLibre()
				 * Modifica el espacioLibre en el nodo sumando o restando
				 * el tamanio de la/s clave/s a insertar, segun sea una
				 * insercion o una eliminacion.
				 */
		void actualizarEspacioLibre(ListaClaves* claves, bool insercion); //TODO cambiar a set STL.
		void actualizarEspacioLibre(Clave* clave, bool insercion);
		//void actualizarEspacioLibre(ArchivoIndice* archivo); TODO Esta comentado porque recibe un Archivo (comunicacion con la capa fisica a traves del pipe)
				
		//TODO condicionMinima() calcula el tamanio minimo que puede estar
		//ocupado en un nodo, pero usa el Archivo. Reveer eso.
		//int condicionMinima(ArchivoIndice* archivo);
		
		//TODO claveSiguiente() devuelve la clave siguiente a 
		//la pasada por parametro la lista de claves del nodo.
		//Cambiar a set STL.
		//Clave* claveSiguiente(Clave* clave);
		
		/*reemplazarClave()
		 * Reemplaza la claveVieja del Nodo actual con una copia de 
		 * la clave nueva. Devuelve Codigo::MODIFICADO en codigo.
		 */
		Clave* reemplazarClave(Clave* claveVieja,Clave* claveNueva,char* codigo);
		
		//TODO Cambiar a set STL.
		/*inserta la lista al final de  la existente en el nodo, graba el nodo en el archivo , actualiza el espcio libre*/
		//void insertarFinal(ListaClaves* lista,ArchivoIndice* archivoIndice);
		
		/*Devuelven referencias a archivos de las claves anterior y posterior a la clave dada
		 * devuelven -1 si la clave no tiene referencia posterior o anterior*/
		char refAnterior(Clave* clave);
		char refPosterior(Clave* clave);
		
		/*buscar()
		 * Devuelve la clave mas cercana a la claveBuscada dentro de la lista
		 * de claves contenidas en el nodo (o la misma si se encuentra presente).
		 */ 
		Clave* buscar(Clave* claveBuscada);
		
	///////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////////
		/*Setters*/
		void setHijoIzq(int hijoIzq)
		{
			this->hijoIzq = hijo;
		}
		
		void setNivel(unsigned int nivel)
		{
			this->nivel = nivel;
		}
		
		//TODO Cambiar al set STL.
		/*
		void setClaves(ListaClaves* lista)
		{		
			this->claves = lista;
		}
		*/
		
		void setEspacioLibre(unsigned int cant)
		{
			this->espacioLibre = cant;
		}
		void setPosicionEnArchivo(unsigned int posicion)
		{
			this->posicionEnArchivo = posicion;	
		}		
		
		/*Getters*/
		unsigned int getHijoIzq()
		{
			return this->hijoIzq;
		}
		
		unsigned int getNivel()
		{
			return this->nivel;
		}
		
		unsigned int getPosicionEnArchivo()
		{
			return this->posicionEnArchivo;
		}
		
		unsigned int getEspacioLibre()
		{
			return this->espacioLibre;
		}
		
		//TODO Los siguientes metodos usan ListaClaves.
		//Cambiar al set STL (si es que son necesarios).
		/*
		ListaClaves* obtenerClaves(); 
		
		bool vacio(ArchivoIndice* archivo);
		void primero();
		Clave* obtenerClaveActual();
		bool siguiente();
		*/                     
}; //Fin clase Nodo.


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: NodoBp (Implementa nodos de Arbol B+)
//////////////////////////////////////////////////////////////////////////
class NodoBp: public Nodo
{
	private:
		/*Referencia en nodos hoja al hermano derecho del nodo actual.
		 * Su valor en nodos del index-set es -1.
		 */
	//////////////////////////////////////////////////////////////////////
	// Atributos privados
	//////////////////////////////////////////////////////////////////////
		int hermanoSiguiente;
	
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		void insertarEnHoja(Clave* &clave, char* codigo);
		void insertarEnNodo(Clave* &clave, char* codigo);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructores
	//////////////////////////////////////////////////////////////////////
		NodoBp(int hijoIzq, unsigned int nivel, int hermanoSiguiente = -1): 
			Nodo(hijoIzq, nivel)
		{
			this->hermanoSiguiente = hermanoSiguiente;
		}
		
		NodoBp(int hijoIzq, unsigned int nivel, Clave* clave, int hermanoSiguiente = -1) : 
					Nodo(hijoIzq, nivel, clave)
		{
			this->hermanoSiguiente = hermanoSiguiente;
		}
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		virtual char insertarClave(Clave* &clave);	
		virtual char eliminarClave(Clave* clave);
	
	//////////////////////////////////////////////////////////////////////
	// Getters/Setters
	//////////////////////////////////////////////////////////////////////		
		int getHermanoSiguiente()
		{
			return this->hermanoSiguiente;
		}
		
		void setHermanoSiguiente(int referencia)
		{
			this->hermanoSiguiente = referencia;			
		}
}; //Fin clase NodoBp

#endif /*NODO_H_*/
