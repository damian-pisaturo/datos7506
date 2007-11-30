///////////////////////////////////////////////////////////////////////////
//	Archivo   : IndiceManager.h
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases IndiceManager correspodientes
//      a cada tipo de dato (booleano, char, short, entero, real, fecha, 
//		string).
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#ifndef INDICEMANAGER_H_
#define INDICEMANAGER_H_

#include <string>

#include "../../Common/Nodo.h"
#include "../../../Common/Tamanios.h"
#include "../../Common/SetClaves.h"
#include "../../../Common/TipoDatos.h"
#include "../../Common/TipoIndices.h"
#include "../Manager.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceManager
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class IndiceManager : public Manager
{
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributo
	///////////////////////////////////////////////////////////////////////
		unsigned char tipoIndice;
		
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		IndiceManager(unsigned int tamanioBloque, string nombreArchivo, unsigned char tipoIndice);		
		virtual ~IndiceManager() {}
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Todos los metodos de lectura/escritura a disco devuelven
		 * ResultadosFisica::ARCHIVO_INVALIDO si el archivo siendo utilizado
		 * no corresponde al archivo propio del proyecto esperado.
		 */
		
		/*Recibe un bloque creado y lo llena con la informacion leida
		 * dependiendo del tipo de bloque (nodo arbol B+, nodo B*, bucket de hash).
		 * Utiliza ComuDatos para comunicarse con la Capa Fisica.
		 */
		virtual int leerBloque(unsigned int numeroBloque, BloqueIndice* bloqueLeido) = 0;		

		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y escribir
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual int escribirBloque(BloqueIndice* bloqueNuevo) = 0;
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque modificado en el disco
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual int escribirBloque(unsigned short numBloque, BloqueIndice* bloqueModif) = 0;
		
		/*Agrega una referencia en el archivo de bloques liberados al
		 * bloque que se quiere eliminar
		 */
		virtual int eliminarBloque(unsigned short posicion) = 0;


	///////////////////////////////////////////////////////////////////////
	// Getters
	///////////////////////////////////////////////////////////////////////
		
		const unsigned int getTipoIndice()
		{
			return this->tipoIndice;
		}	
		
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceArbolManager
//			( Clase que sirve de abstraccion de la capa 
//			fisica para los indices en arboles de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class IndiceArbolManager : public IndiceManager
{	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		/*Si el archivo esta vacio, crea una raiz vacia*/
		IndiceArbolManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
		virtual ~IndiceArbolManager() {}
	
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////

		/*Header del Nodo*/
		struct HeaderNodo{
			unsigned char nivel;
			unsigned int refNodo;	
			unsigned short espacioLibre;			
		}; 

	///////////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////////	
		/*Se encarga de interpretar cada tipo de clave y copiarla al buffer
		 * de escritura dependiendo si es un nodo hoja o un nodo no hoja*/
		virtual void copiarClaveHoja(Clave* clave,char* &puntero);
	
		virtual void copiarClaveNoHoja(Clave* clave,char* &puntero);
	
		/*Se encarga de leer cada tipo de Clave de un buffer
		 * dependiendo si es un nodo hoja o un nodo no hoja*/
		virtual Clave* leerClaveHoja(char* &buffer) = 0;
		
		virtual Clave* leerClaveNoHoja(char* &buffer) = 0;
	

	public:
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
		/*Todos los metodos de lectura/escritura a disco devuelven
		 * ResultadosFisica::ARCHIVO_INVALIDO si el archivo siendo utilizado
		 * no corresponde al archivo propio del proyecto esperado.
		 */
		
		virtual int leerBloque(unsigned int numeroBloque, BloqueIndice* bloqueLeido);
		
		virtual int escribirBloque(BloqueIndice* bloqueNuevo);		
		/*Permite modificar la informacion de un nodo*/
		virtual int escribirBloque(unsigned short numBloque, BloqueIndice* bloqueModif);
		
		/*Agrega una referencia en el archivo de nodos liberados al
		 * nodo que se quiere eliminar
		 */
		virtual int eliminarBloque(unsigned short posicion);
		
		virtual int eliminarBloqueDoble(unsigned short posicion);
		virtual int escribirBloqueDoble(BloqueIndice* bloqueNuevo);
		virtual int escribirBloqueDoble(unsigned short numBloque, BloqueIndice* bloqueModif);
		virtual int leerBloqueDoble(unsigned short numBloque, BloqueIndice* bloqueLeido);
	
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
		static int getTamanioHeader()
		{
			return sizeof(HeaderNodo);
		}
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceHashManager
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de dispersion de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class IndiceHashManager : public IndiceManager
{	
	private:
		unsigned int bloqueSiguiente;
	
	public:
		
		typedef struct HeaderBucket{
			unsigned short espLibre;
			unsigned short cantRegs;
			unsigned short tamDispersion;
		};
		
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		IndiceHashManager(unsigned int tamBucket, string nombreArchivo);		
		virtual ~IndiceHashManager();
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Todos los metodos de lectura/escritura a disco devuelven
		 * ResultadosFisica::ARCHIVO_INVALIDO si el archivo siendo utilizado
		 * no corresponde al archivo propio del proyecto esperado.
		 */
		
		/*Lee el bucket cuya referencia en el archivo es numeroBloque y llena
		 * una estructura Bucket con su informacion para ser consultada.
		 */
		virtual int leerBloque(unsigned int numeroBucket, BloqueIndice* bloqueLeido);
		
		/* Busca el primer bucket libre en el archivo y escribe el nuevo bucket
		 * en el. Si no encuentra ninguno, appendea al final del archivo.
		 */
		virtual int escribirBloque(BloqueIndice* bloqueNuevo);
		
		/* Sobreescribe el bloque actual con el bloqueNuevo.
		 */
		virtual int escribirBloque(unsigned short numBucket, BloqueIndice* bloqueNuevo);
		
		virtual int eliminarBloque(unsigned short posicion);
		
		/* Sobre-escribe la tabla de dispersion con el tamaño y los
		 * numeros de buckets pasados por parametro.
		 */
		char escribirTabla(unsigned int tamanio, unsigned int* buckets);
		
		/* Lee la tabla de dispersion de disco y coloca su tamaño
		 * y numero de buckets en los parametros pasados.
		 * 
		 * Se devuelve en 'buckets' un puntero a una cantidad de 
		 * numero de buckets igual a 'tamanio', reservando el 
		 * espacio necesario.
		 */
		char leerTabla(unsigned int* tamanio, unsigned int* &buckets);
		
		/* Devuelve en 'bloque' el siguiente bloque de datos presente en el archivo. 
		 * Si pudo obtener un bloque retorna ResultadosFisica::OK, de lo contrario 
		 * ResultadosFisica::FIN_BLOQUES.
		 */
		int siguienteBloque(void* bloque);

};


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceEnteroManager 
//		   (Implementa archivo de indices de clave entera).
///////////////////////////////////////////////////////////////////////////
class IndiceEnteroManager: public IndiceArbolManager
{
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////		
		IndiceEnteroManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
		
		virtual ~IndiceEnteroManager() {}
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		
		/*Interpreta un buffer como una clave de tipo ClaveEntera.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);

	
}; //Fin clave IndiceEnteroManager


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceBooleanManager 
//		   (Implementa archivo de indices de clave booleana).
///////////////////////////////////////////////////////////////////////////

class IndiceBooleanManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	//////////////////////////////////////////////////////////////////////		
		IndiceBooleanManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
		virtual ~IndiceBooleanManager() {}
			
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		/*Interpreta un buffer como una clave de tipo ClaveBoolean.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);	
		
}; //Fin clase IndiceBooleanManager


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceCharManager
//		   (Implementa archivo de indices de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class IndiceCharManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	//////////////////////////////////////////////////////////////////////
		IndiceCharManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);		
		virtual ~IndiceCharManager() {}
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveChar.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase IndiceCharGriegoManager

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceShortManager
//		   (Implementa archivo de indices de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
class IndiceShortManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceShortManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveShort.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase IndiceShortManager

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceRealManager
//		   (Implementa archivo de indices de clave de tipo float).
///////////////////////////////////////////////////////////////////////////
class IndiceRealManager: public IndiceArbolManager
{	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceRealManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveReal.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
}; //Fin clase IndiceRealManager


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceFechaManager
//		   (Implementa archivo de indices de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////
class IndiceFechaManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceFechaManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
		virtual ~IndiceFechaManager() {}
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveFecha.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);	
		
}; //Fin clase IndiceFechaManager

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceVariableManager
//		   (Implementa archivo de indices de clave de longitud variable).
///////////////////////////////////////////////////////////////////////////
class IndiceVariableManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceVariableManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);	

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		void copiarClaveNoHoja(Clave* clave,char* &buffer);
		void copiarClaveHoja(Clave* clave,char* &buffer);
		
		/*Interpreta un buffer como una clave de tipo ClaveVariable.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
};
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceCompuestoManager 
//		   (Implementa archivo de indices primarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////

class IndiceCompuestoManager: public IndiceArbolManager
{
	private:
		ListaTipos* tipos;
		
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceCompuestoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice, ListaTipos* listaTipos);
		
		virtual ~IndiceCompuestoManager() { delete tipos; }
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Interpreta claves compuestas para copiarlas a
		 * un buffer de escritura.
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Lee claves compuestas de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; // Fin case IndiceCompuestoManager

#endif /*INDICEMANAGER_H_*/
