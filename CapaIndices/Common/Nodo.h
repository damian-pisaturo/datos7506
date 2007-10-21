///////////////////////////////////////////////////////////////////////////
//	Archivo   : Nodo.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase abstracta Nodo.
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

#include "SetClaves.h"
#include "Codigo.h"

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
		
		/*unsigned int refNodo
		 * 
		 * Arbol B+
		 *		En nodos hoja, refNodo indica offset al hermano
		 * 		derecho; en nodos internos, indica offset al hijo 
		 * 		izquierdo.
		 *
		 * Arbol B*
		 * 		En nodos hoja, refNodo es 0; en nodos internos
		 * 		indica offset a hijo izquierdo.
		 */	
		unsigned int refNodo;
		unsigned int nivel;
		unsigned int espacioLibre;
		unsigned int posicionEnArchivo;
		SetClaves* claves;
	
	public:
	///////////////////////////////////////////////////////////////////////
    // Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Crea el nodo sin setear ningun atributo*/
		Nodo(unsigned int refNodo, int nivel);
		
		/*Crea un nuevo nodo para insertarle una Clave*/
		Nodo(unsigned int refNodo,int nivel,Clave* clave);
		
		/*Lee el archivo y crea ese nodo*/
		Nodo(unsigned int referencia);
		
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
		 *	Devuelve un puntero al siguiente Nodo en la busqueda
		 *  recursiva (hijo derecho o izquierdo) de la clave pasada 
		 *  por parametro.
		 */
		Nodo* siguiente(Clave* clave); 
		
		/*eliminarClave()
		 * Elimina la clave pasada en el nodo corriente.
		 * Devuelve un Codigo dependiendo del resultado:
		 * 	Codigo::OK - Eliminacion llevada a cabo correctamente.
		 *  Codigo::UNDERFLOW - Subflujo de claves.
		 */		
		virtual char eliminarClave(Clave* clave);
		
		/*actualizarEspacioLibre()
	     * Modifica el espacioLibre en el nodo sumando o restando
	     * el tamanio de la/s clave/s a insertar, segun sea una
		 * insercion o una eliminacion.
	     */
		void actualizarEspacioLibre(SetClaves* claves, bool insercion); 
		void actualizarEspacioLibre(Clave* clave, bool insercion);
		//void actualizarEspacioLibre(ArchivoIndice* archivo); TODO Esta comentado porque recibe un Archivo (comunicacion con la capa fisica a traves del pipe)
				
		//TODO condicionMinima() calcula el tamanio minimo que puede estar
		//ocupado en un nodo, pero usa el Archivo. Reveer eso.
		//int condicionMinima(ArchivoIndice* archivo);
			
		/*reemplazarClave()
		 * Reemplaza la claveVieja del Nodo actual con una copia de 
		 * la clave nueva. Devuelve Codigo::MODIFICADO en codigo.
		 */
		Clave* reemplazarClave(Clave* claveVieja,Clave* claveNueva, char* codigo);
	
		/*buscar()
		 * Devuelve la clave mas cercana a la claveBuscada dentro de la lista
		 * de claves contenidas en el nodo (o la misma si se encuentra presente).
		 */ 
		Clave* buscar(Clave* claveBuscada);
			
		//bool vacio(ArchivoIndice* archivo);
		
	///////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////////
		/*Setters*/
		void setRefNodo(unsigned int refNodo)
		{
			this->refNodo = refNodo;
		}
		
		void setNivel(unsigned int nivel)
		{
			this->nivel = nivel;
		}
		
		void setClaves(ListaClaves* lista)
		{		
			this->claves = lista;
		}
		
		void setEspacioLibre(unsigned int cant)
		{
			this->espacioLibre = cant;
		}
		void setPosicionEnArchivo(unsigned int posicion)
		{
			this->posicionEnArchivo = posicion;	
		}		
		
		/*Getters*/
		unsigned int getRefNodo()
		{
			return this->refNodo;
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
		
		SetClaves* getClaves()
		{
			return this->claves;
		}
}; //Fin clase Nodo.

#endif /*NODO_H_*/
