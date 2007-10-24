///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndicePadre.h
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases ArchivoIndice, 
//		ArchivoIndiceArbol, ArchivoIndiceArbolSecundario,
//		ArchivoIndiceHash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef ARCHIVOINDICEPADRE_H_
#define ARCHIVOINDICEPADRE_H_

#include <iostream>
using namespace std;

#include "../ComuDatos/ComuDatos.h"
#include "BloqueIndice.h"
#include "../NombreCapas.h"

typedef enum{ARBOL_BP, ARBOL_BS, HASH} t_indice;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndice
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndice
{
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		string nombreArchivo;
		unsigned int tamanioBloque;
		const unsigned int tipoIndice;
		
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoIndice(string nombreArchivo, unsigned int tamanioBloque, t_indice tipoIndice);
		virtual ~ArchivoIndice() { };
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Recibe un bloque creado y lo llena con la informacion leida
		 * dependiendo del tipo de bloque (nodo arbol B+, nodo B*, bucket de hash).
		 * Utiliza ComuDatos para comunicarse con la Capa Fisica.
		 */
		virtual void leerBloque(int numeroBloque, BloqueIndice* bloqueLeido) = 0;

		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y escribir
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual void escribirBloque(BloqueIndice* bloqueNuevo) = 0;
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque modificado en el disco
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual void sobreEscribirBloque(BloqueIndice* bloqueModif) = 0;
		
		/* Devuelve una instancia de un pipe de comunicacion entre
		 * la clase actual y el ejecutable cuyo nombre es pasado
		 * por parametro.
		 */
		ComuDatos* instanciarPipe(string nombreEjecutable);
		

	///////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////
		unsigned int getTamanioBloque()
		{
			return this->tamanioBloque;
		}
		
		void setTamanioBloque(unsigned int tamanio)
		{
			this->tamanioBloque = tamanio;
		}
		
		string getNombreArchivo()
		{
			return this->nombreArchivo;
		}
		
		void setNombreArchivo(string nombre)
		{
			this->nombreArchivo = nombre;
		}
		
		const unsigned int getTipoIndice()
		{
			return this->tipoIndice;
		}
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceArbol
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices en arboles de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceArbol : public ArchivoIndice
{
	private:
		string nombreArchivoEL;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		/*Si el archivo esta vacio, crea una raiz vacia*/
		ArchivoIndiceArbol(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice);
		virtual ~ArchivoIndice() { };
	
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////

	/*Header del Nodo*/
	struct Header{
		unsigned int nivel;
		unsigned int espacioLibre;
		unsigned int hijoIzq;	
	}; 

	///////////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////////	
		/*Esta funcion se encarga de Interpretar cada tipo de clave y copiarla al buffer
		 * de escritura dependiendo si es un nodo hoja o un nodo no hoja*/
		virtual void copiarClaveHoja(Clave* clave,char* &puntero) = 0;
	
		virtual void copiarClaveNoHoja(Clave* clave,char* &puntero) = 0;
	
		/*Esta funcion se encarga de leer cada tipo de Clave de un buffer
		 * dependiendo si es un nodo hoja o un nodo no hoja*/
		virtual Clave* leerClaveHoja(char* &buffer) = 0;
		
		virtual Clave* leerClaveNoHoja(char* &buffer) = 0;
	
		/*Escribe una raiz vacia para cuando se crea el archivoIndice*/
		virtual void escribirRaizVacia();

	public:
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
		
		virtual void leerBloque(unsigned int numeroBloque, BloqueIndice* bloqueLeido);
		virtual void escribirBloque(BloqueIndice* bloqueNuevo);		
		/*Permite modificar la informacion de un nodo -> inclusive la raiz si posicion = 0*/
		virtual void sobreEscribirBloque(BloqueIndice* bloqueModif);
		
		/*Agrega una referencia en el archivo de nodos liberados al
		 * nodo que se quiere eliminar*/
		virtual void eliminarNodo(int posicion);
		
		/*Exportar el archivo de Indice*/
		void exportar(ostream &archivoTexto,int posicion);
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
		static int getTamanioHeader()
		{
			return sizeof(Header);
		}
		
		string setNombreArchivoEL()
		{
			return nombreArchivoEL;
		}
		
		void setNombreArchivoEL(string nombre)
		{	
			this->nombreArchivoEL = nombre;
		}
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceHash
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de dispersion de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceHash : public ArchivoIndice
{
	private:
		string nombreArchivoTabla;
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoIndiceHash(unsigned int tamBucket, string nombreArchivo, t_indice tipoIndice);		
		virtual ~ArchivoIndiceHash() { };
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Lee el bucket cuya referencia en el archivo es numeroBloque y llena
		 * una estructura Bucket con su informacion para ser consultada.
		 */
		virtual void leerBloque(unsigned int numeroBloque, BloqueIndice* bloqueLeido);
		
		/* Busca el primer bucket libre en el archivo y escribe el nuevo bucket
		 * en el. Si no encuentra ninguno, appendea al final del archivo.
		 */
		virtual void escribirBloque(BloqueIndice* bloqueNuevo);
		
		/* Busca el primer bucket libre en el archivo y escribe el nuevo bucket
		 * en el. Si no encuentra ninguno, appendea al final del archivo.
		 */
		virtual void sobreEscribirBloque(BloqueIndice* bloqueNuevo);
	
	///////////////////////////////////////////////////////////////////////
	// Getter/Setter
	///////////////////////////////////////////////////////////////////////
		string getNombreArchivoTabla()
		{
			return this->nombreArchivoTabla;
		}
		
		void setNombreArchivoTable(string nombre)
		{
			return this->nombreArchivoTabla;
		}		
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceSecundario
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices secundarios de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceSecundario: public ArchivoIndiceArbol
{	
	private:		
		unsigned int tamanioArray;
		string nombreArchivoLista;
		
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceSecundario(int tamNodo, string nombreArchivo, unsigned int tamanioBloqueLista, t_indice tipoIndice);
		virtual ~ArchivoIndiceSecundario();

	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////

		/*POST: Devuelve una lista de claves primaras de la posicion dada.
		 * RECORDAR: Borrar la lista una vez utilizada*/
		SetClaves* obtenerLstClavesP(unsigned int posicion);
		
		/*POST: SobreEscribe con listaClaves la lista que se encuentra en la posicion dada
		 * para actualizarla una vez insertada una nueva referencia en la lista*/
		void sobreEscribirListaClavesP(unsigned int posicion, SetClaves* setClaves);
		
		/*POST: Graba una nueva lista de claves primarias en el archivo, devolviendo la posicion
		 * donde se grabo, 
		 * $$$: Recordar que el valor que sale de aca se tiene que grabar
		 * en el NODO, estoy hay que hacerlo antes de insertar en el arbol*/ 
		unsigned int grabarNuevaLstClavesP(SetClaves* setClaves);		
	
		Clave* leerClaveNoHoja(char* &buffer, t_indice tipoIndice){return NULL;};
		Clave* leerClaveHoja(char* &buffer, t_indice tipoIndice){return NULL;};
		void copiarClave(Clave* clave, char* &puntero){};
		void copiarClaveHoja(Clave* clave, char* &puntero){};
		
		/*Redefino el exportar para que tb imprima la lista de claves primarias*/
		void exportar(ostream &archivoTexto,int posicion);		
};

#endif /*ARCHIVOINDICEPADRE_H_*/