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
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef NODO_H_
#define NODO_H_

#include "BloqueIndice.h"
#include "SetClaves.h"
#include "Codigo.h"
#include "Clave.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Nodo (Abstracta)
//////////////////////////////////////////////////////////////////////////
class Nodo : public BloqueIndice
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
		unsigned char nivel;
		unsigned int posicionEnArchivo;
		unsigned short tamanio;
		unsigned short tamanioMinimo;
		SetClaves* claves;		
	
	public:
	///////////////////////////////////////////////////////////////////////
    // Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Crea un nuevo nodo para insertarle una Clave*/
		Nodo(unsigned int refNodo, unsigned char nivel, Clave* clave, unsigned short tamanio);
		
		/*Crea el nodo sin setear ninguna clave*/
		Nodo(unsigned int refNodo, unsigned char nivel, unsigned short tamanio);
		
		/*Lee el archivo y crea ese nodo*/
		//Nodo(ArchivoIndice* archivo, unsigned int referencia);
		
		/*Destructor*/
		virtual ~Nodo();
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		/*insertarClave()
		 * Inserta la clave pasada en el nodo corriente.
		 * Devuelve en codigo, dependiendo del resultado:
		 * 	Codigo::NO_MODIFICADO - Insercion llevada a cabo correctamente.
		 *  Codigo::OVERFLOW - Sobreflujo de claves.
		 * */
		virtual void insertarClave(Clave* &clave, char* codigo) = 0;
				
		/*siguiente()
		 *	Devuelve un puntero al siguiente Nodo en la busqueda
		 *  recursiva (hijo derecho o izquierdo) de la clave pasada 
		 *  por parametro.
		 */
		virtual Nodo* siguiente(Clave* clave) = 0; 
		
		/*eliminarClave()
		 * Elimina la clave pasada en el nodo corriente.
		 * Devuelve en codigo, dependiendo del resultado:
		 * 	Codigo::NO_MODIFICADO - Eliminacion llevada a cabo correctamente.
		 *  Codigo::UNDERFLOW - Subflujo de claves.
		 */		
		virtual void eliminarClave(Clave* clave, char* codigo) = 0;
		
		/*actualizarEspacioLibre()
	     * Modifica el espacioLibre en el nodo sumando o restando
	     * el tamanio de la/s clave/s a insertar, segun sea una
		 * insercion o una eliminacion.
	     */
		void actualizarEspacioLibre(SetClaves* claves, bool insercion); 
		void actualizarEspacioLibre(Clave* clave, bool insercion);
		//void actualizarEspacioLibre(ArchivoIndice* archivo); TODO Esta comentado porque recibe un Archivo (comunicacion con la capa fisica a traves del pipe)
			
		/*reemplazarClave()
		 * Reemplaza la claveVieja del Nodo actual con una copia de 
		 * la clave nueva. Devuelve Codigo::MODIFICADO en codigo.
		 */
		//Clave* reemplazarClave(Clave* claveVieja, Clave* claveNueva, char* codigo);
	
		/*buscar()
		 * Devuelve la clave mas cercana a la claveBuscada dentro de la lista
		 * de claves contenidas en el nodo (o la misma si se encuentra presente).
		 */ 
		Clave* buscar(Clave* claveBuscada) const;
			
		//bool vacio(ArchivoIndice* archivo);
		
		bool operator == (const Nodo &nodo) const {
			return (this->getPosicionEnArchivo() == nodo.getPosicionEnArchivo());
		}
		
		//Calcula el tamanio que ocupa el conjunto de claves dentro del nodo
		unsigned short getTamanioEnDiscoSetClaves() const;
		
		//Método que se encarga de ver cuantos bytes deberia ceder realmente el nodo para
		//poder cumplir con los 'bytesRequeridos'.
		//Si 'izquierda' es true, indica que el nodo debe ceder al hermano izquierdo,
		//en caso contrario, debe ceder al hermano derecho.
		//Si se le pide ceder mas de los bytes que tiene, devuleve cero. Sino, devuelve los
		//bytes a ceder.
		unsigned short bytesACeder(unsigned short bytesRequeridos, unsigned char &clavesPropuestas,
								   bool izquierda = true) const;
		
		//Como el anterior pero se fija si puede ceder cierta cantidad de claves en vez de bytes.
		unsigned short bytesACeder(unsigned char clavesPropuestas, bool izquierda = true) const;
		
		bool puedeRecibir(unsigned short bytesEntrantes, unsigned short bytesSalientes) const;
		
		unsigned short obtenerBytesRequeridos() const;
		
		//Devuelve la cantidad de bytes que sobran en un nodo luego de una inserción con overflow.
		//En 'cantClaves' devuelve la cantidad de claves que representan los bytes sobrantes.
		unsigned short obtenerBytesSobrantes(unsigned short &cantClaves) const;
				
		SetClaves* ceder(unsigned short bytesRequeridos, bool izquierda = true);
		
		void recibir(SetClaves* set);
		
		bool esPadre(const Nodo* hijo, Clave* &clave) const;
		
		//Retorna un puntero la primera clave o NULL si el nodo no tiene claves.
		Clave* obtenerPrimeraClave() const;
		
		//Retorna un puntero la última clave o NULL si el nodo no tiene claves.
		Clave* obtenerUltimaClave() const;
		
		bool puedePasarClaveHaciaIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre) const;
		
		bool puedePasarClaveHaciaDer(Nodo* nodoHnoDer, Nodo* nodoPadre) const;
		
	///////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////////
		/*Setters*/
		void setRefNodo(unsigned int refNodo)
		{
			this->refNodo = refNodo;
		}
		
		void setHijoIzq(unsigned hijoIzq)
		{ 
			this->refNodo = hijoIzq;
		}
		
		void setHnoDer(unsigned hnoDer)
		{ 
			this->refNodo = hnoDer;
		}
		
		void setNivel(unsigned char nivel)
		{
			this->nivel = nivel;
		}
		
		void setClaves(SetClaves* set)
		{	
			if (this->claves) delete this->claves;
			this->claves = set;
		}		
		
		void setPosicionEnArchivo(unsigned int posicion)
		{
			this->posicionEnArchivo = posicion;	
		}
		
		void setTamanio(unsigned short tamanio)
		{
			this->tamanio = tamanio;	
		}
		
		/*Getters*/
		unsigned int getRefNodo() const
		{
			return this->refNodo;
		}
		
		unsigned getHijoIzq() const
		{ 
			return this->refNodo;
		}
		
		unsigned getHnoDer() const
		{ 
			return this->refNodo;
		}
		
		unsigned char getNivel() const
		{
			return this->nivel;
		}
		
		unsigned int getPosicionEnArchivo() const
		{
			return this->posicionEnArchivo;
		}
		
		SetClaves* getClaves() const
		{
			return this->claves;
		}
		
		static unsigned char getTamanioHeader()
		{
			return (Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_NIVEL + Tamanios::TAMANIO_REFERENCIA);
		}
		
		unsigned short getTamanio() const
		{
			return this->tamanio;
		}
		
		virtual unsigned short getTamanioMinimo() const 
		{
			return this->tamanioMinimo;
		}
		
		//Devuelve el espacio (en bytes) destinado para almacenar claves (elementos)
		virtual unsigned short getTamanioEspacioClaves() const
		{
			return (this->getTamanio() - this->getTamanioHeader());
		}
		
	protected:
		void setTamanioMinimo(unsigned short tamMin)
		{
			this->tamanioMinimo = tamMin;			
		}
		
}; //Fin clase Nodo.

#endif /*NODO_H_*/
