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
		unsigned short espacioLibre;
		unsigned int refNodo;
		unsigned char nivel;
		unsigned int posicionEnArchivo;
		unsigned short tamanio;
		SetClaves* claves;		
	
	public:
	///////////////////////////////////////////////////////////////////////
    // Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Crea un nuevo nodo para insertarle una Clave*/
		Nodo(unsigned int refNodo, unsigned char nivel, Clave* clave, unsigned short tamanio);
		
		/*Crea el nodo sin setear ninguna clave*/
		Nodo(unsigned int refNodo, unsigned char nivel, unsigned short tamanio);
		
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
		virtual void insertarClave(Clave* &clave, char* codigo);
		
		/*eliminarClave()
		 * Elimina la clave pasada en el nodo corriente.
		 * Devuelve en codigo, dependiendo del resultado:
		 * 	Codigo::NO_MODIFICADO - Eliminacion llevada a cabo correctamente.
		 *  Codigo::UNDERFLOW - Subflujo de claves.
		 */
		virtual void eliminarClave(Clave* clave, char* codigo);
		
		/*actualizarEspacioLibre()
	     * Modifica el espacioLibre en el nodo sumando o restando
	     * el tamanio de la/s clave/s a insertar, segun sea una
		 * insercion o una eliminacion.
	     */
		virtual void actualizarEspacioLibre(SetClaves* claves, bool insercion) = 0; 
		virtual void actualizarEspacioLibre(Clave* clave, bool insercion) = 0;
		void actualizarEspacioLibre();
			
		/*reemplazarClave()
		 * Reemplaza la claveVieja del Nodo actual con una copia de 
		 * la clave nueva. Devuelve Codigo::MODIFICADO en codigo.
		 */
		bool reemplazarClave(Clave* claveVieja, Clave* claveNueva, char* codigo);
	
		/*buscar()
		 * Devuelve la clave mas cercana a la claveBuscada dentro de la lista
		 * de claves contenidas en el nodo (o la misma si se encuentra presente).
		 */ 
		Clave* buscar(Clave* claveBuscada) const;
			
		bool vacio() const {
			return this->getClaves()->empty();
		}
		
		bool operator == (const Nodo &nodo) const {
			return (this->getPosicionEnArchivo() == nodo.getPosicionEnArchivo());
		}
		
		unsigned short getCantidadClaves() const {
			return this->getClaves()->size();
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
		unsigned short obtenerBytesSobrantes(unsigned short &cantClaves, bool izquierda = true) const;
				
		SetClaves* cederBytes(unsigned short bytesRequeridos, bool izquierda = true);
		
		SetClaves* cederClaves(unsigned short cantClaves, bool izquierda = true);
		
		void recibir(SetClaves* set);
		
		bool esPadre(const Nodo* hijo, Clave* &clave) const;
		
		//Retorna un puntero la primera clave o NULL si el nodo no tiene claves.
		Clave* obtenerPrimeraClave() const;
		
		//Retorna un puntero la última clave o NULL si el nodo no tiene claves.
		Clave* obtenerUltimaClave() const;
		
		virtual bool puedePasarClaveHaciaIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre, Clave* clavePadre) const;
		
		virtual bool puedePasarClaveHaciaDer(Nodo* nodoHnoDer, Nodo* nodoPadre, Clave* clavePadre) const;
		
		bool puedeRecibirClaveDesdeIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre, Clave* clavePadre) const;
		
		bool puedeRecibirClaveDesdeDer(Nodo* nodoHnoDer, Nodo* nodoPadre, Clave* clavePadre) const;
		
		//Este metodo fusiona el conjunto de claves de este nodo con el conjunto
		//de un nodo hermano y con una clave padre.
		void merge(Nodo* nodoHno, Clave* clavePadre);
		
		//Este metodo fusiona el conjunto de claves de este nodo con los conjuntos
		//de dos nodos hermanos y con dos claves padre.
		void merge(Nodo* nodoHno1, Nodo* nodoHno2, Clave* clavePadre1, Clave* clavePadre2);
		
		//Saca del setClaves a la ultima clave, sin liberar la memoria.
		Clave* extraerUltimaClave();
		
		Clave* extraerPrimeraClave();
		
		//Saca del setClaves la clave recibida por parametro, sin liberar la memoria.
		void extraerClave(Clave* clave);
		
		//Revisa si el nodo tiene overflow o no.
		bool tieneOverflow() const;
		
		bool tieneUnderflow() const;
		
		//Split que se comporta como el de un arbol B (parte en dos el nodo), donde minClaves es
		//la cantidad (en bytes) que debe tener cada parte resultante.
		SetClaves* splitB(unsigned short minClaves);
		
		//Método abstracto que devuelve una copia de este nodo
		virtual Nodo* copiar() const = 0;

		
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
		
		void setClaves(SetClaves* set);
		
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
		
		virtual unsigned short getTamanioMinimo() const = 0;
		
		//Devuelve el espacio (en bytes) destinado para almacenar claves (elementos)
		virtual unsigned short getTamanioEspacioClaves() const = 0;
		
}; //Fin clase Nodo.

#endif /*NODO_H_*/
