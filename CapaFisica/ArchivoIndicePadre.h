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
		ComuDatos pipe;
		string nombreArchivo;
		unsigned int tamanioBloque;
		
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoIndice(string nombreArchivo, unsigned int tamanioBloque);
		virtual ~ArchivoIndice() { };
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Recibe un bloque creado y lo llena con la informacion leida
		 * dependiendo del tipo de bloque (nodo arbol B+, nodo B*, bucket de hash).
		 * Utiliza ComuDatos para comunicarse con la Capa Fisica.
		 */
		virtual void leerBloque(int numeroBloque, Bloque* bloqueLeido) = 0;

		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y escribir
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual void escribirBloque(Bloque* bloqueNuevo) = 0;

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
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		/*Si el archivo esta vacio, crea una raiz vacia*/
		ArchivoIndiceArbol(unsigned int tamNodo, string nombreArchivo);
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
		
		virtual void leerBloque(unsigned int numeroBloque, Bloque* bloqueLeido);
		virtual void escribirBloque(Bloque* bloqueNuevo);		
	
		/*Permite modificar la informacion de un nodo -> inclusive la raiz si posicion = 0*/
		virtual void sobreescribirNodo(Nodo* nodoModif);
		
		/*Agrega una referencia en el archivo de nodos liberados al
		 * nodo que se quiere eliminar*/
		virtual void eliminarNodo(int posicion);
		
		/*Exportar el archivo de Indice*/
		void exportar(ostream &archivoTexto,int posicion);
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
		static int getTamanioHeader();	
};

/*******************************************************/
/* Clase Archivo de Indice Secundario                  */
/*-----------------------------------------------------*/

class ArchivoIndiceSecundario: public ArchivoIndiceArbol{
	
private:		
	ArchivoRegistros* archivoLista; //TODO Esto ya no es necesario. Debiera llamarse al ComuDatos.
	int tamanioArray;
	
public:
	 
	/*Constructor*/
	ArchivoIndiceSecundario(int tamNodo,string nombreArchivo,int tamanioBloqueLista);
  	
	/*POST: Devuelve una lista de claves primaras de la posicion dada.
	 * RECORDAR: Borrar la lista una vez utilizada*/
	Lista* obtenerLstClavesP(int posicion);
	
	/*POST: SobreEscribe con listaClaves la lista que se encuentra en la posicion dada
	 * para actualizarla una vez insertada una nueva referencia en la lista*/
	void sobreEscribirListaClavesP(int posicion, Lista* listaClaves);
	
	/*POST: Graba una nueva lista de claves primarias en el archivo, devolviendo la posicion
	 * donde se grabo, 
	 * $$$: Recordar que el valor que sale de aca se tiene que grabar
	 * en el NODO, estoy hay que hacerlo antes de insertar en el arbol*/ 
	int grabarNuevaLstClavesP(Lista* listaClaves);
	
	virtual ~ArchivoIndiceSecundario();
	
	int getCantEscriturasLista();
	int getCantLecturasLista();
	Clave* leerClaveNoHoja(char* &buffer){return NULL;};
	Clave* leerClaveHoja(char* &buffer){return NULL;};
	void copiarClave(Clave* clave,char* &puntero){};
	void copiarClaveHoja(Clave* clave,char* &puntero){};
	/*Redefino el exportar para que tb imprima la lista de claves primarias*/
	void exportar(ostream &archivoTexto,int posicion);
};

#endif /*ARCHIVOINDICEPADRE_H_*/
