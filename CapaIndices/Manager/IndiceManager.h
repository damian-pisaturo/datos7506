///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndice.h
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases ArchivoIndice correspodientes
//      a cada tipo de dato (booleano, char, short, entero, real, fecha, 
//		string) e indice (Griego y Romano).
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

#include <iostream>

#include "../Common/Nodo.h"
#include "../../Common/Tamanios.h"
#include "../Common/SetClaves.h"
#include "../../ComuDatos/ComuDatos.h"
#include "../../Common/NombresCapas.h"
#include "../Common/TipoDatos.h"
#include "../../Common/OperacionesCapas.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: TipoIndices
//			(Clase utilitaria con constantes que designan tipos
//			de indices utilizados).
///////////////////////////////////////////////////////////////////////////

class TipoIndices
{
	public:
		static const unsigned char ARBOL_BS = 0;
		static const unsigned char ARBOL_BP = 1;
		static const unsigned char HASH     = 2;
		static const unsigned char GRIEGO	= 3;
		static const unsigned char ROMANO	= 4;
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceManager
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class IndiceManager
{
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		string nombreArchivo;
		unsigned int tamanioBloque;
		unsigned int tipoIndice;
		
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		IndiceManager(unsigned int tamanioBloque, string nombreArchivo, unsigned char tipoIndice);		
		virtual ~IndiceManager();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Recibe un bloque creado y lo llena con la informacion leida
		 * dependiendo del tipo de bloque (nodo arbol B+, nodo B*, bucket de hash).
		 * Utiliza ComuDatos para comunicarse con la Capa Fisica.
		 */
		virtual int leerBloque(unsigned int numeroBloque, BloqueIndice* bloqueLeido) = 0;
		
		virtual int leerBloqueDoble(unsigned int numeroBloque, BloqueIndice* bloqueLeido) = 0;

		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y escribir
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual int escribirBloque(BloqueIndice* bloqueNuevo) = 0;
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque modificado en el disco
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual int escribirBloque(unsigned short numBloque, BloqueIndice* bloqueModif) = 0;
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * escribir dos bloques consecutivos en disco. Empleado para la escritura
		 * especial de una raiz de Arbol B*.
		 */
		virtual int escribirBloqueDoble(BloqueIndice* bloqueModif) = 0;
		virtual int escribirBloqueDoble(unsigned short numBloque, BloqueIndice* bloqueModif) = 0;
		
		/*Agrega una referencia en el archivo de bloques liberados al
		 * bloque que se quiere eliminar
		 */
		virtual int eliminarBloque(unsigned short posicion) = 0;
		virtual int eliminarBloqueDoble(unsigned short posicion) = 0;
		

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
		
		
	protected:
		
		/* Devuelve una instancia de un pipe de comunicacion entre
		 * la clase actual y el ejecutable cuyo nombre es pasado
		 * por parametro.
		 */
		ComuDatos* instanciarPipe(string nombreEjecutable);
		
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
			unsigned short espacioLibre;
			unsigned int refNodo;	
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
	
		/*Escribe una raiz vacia para cuando se crea el archivoIndice*/
		virtual void escribirRaizVacia();

	public:
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////	
		
		virtual int leerBloque(unsigned int numeroBloque, BloqueIndice* bloqueLeido);
		
		virtual int escribirBloque(BloqueIndice* bloqueNuevo);		
		/*Permite modificar la informacion de un nodo*/
		virtual int escribirBloque(unsigned short numBloque, BloqueIndice* bloqueModif);
		
		/*Agrega una referencia en el archivo de nodos liberados al
		 * nodo que se quiere eliminar
		 */
		virtual int eliminarBloque(unsigned short posicion);
		
		
		
		/*Exportar el archivo de Indice
		 */
		void exportar(ostream &archivoTexto,int posicion);
	
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
		void escribirTabla(unsigned int tamanio, unsigned int* buckets);
		
		/* Lee la tabla de dispersion de disco y coloca su tamaño
		 * y numero de buckets en los parametros pasados.
		 * 
		 * Se devuelve en 'buckets' un puntero a una cantidad de 
		 * numero de buckets igual a 'tamanio', reservando el 
		 * espacio necesario.
		 */
		void leerTabla(unsigned int* tamanio, unsigned int* buckets);

};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceSecundarioManager
//			(Abstracta. Clase que sirve de abstraccion de la capa 
//			fisica para los indices secundarios de la capa de indices).
///////////////////////////////////////////////////////////////////////////
class IndiceSecundarioManager: public IndiceArbolManager
{	
	private:		
		unsigned int tamanioArray;
		
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	//////////////////////////////////////////////////////////////////////
		IndiceSecundarioManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamanioBloqueLista, unsigned char tipoIndice);
		virtual ~IndiceSecundarioManager();

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
	
		/*Redefino el exportar para que tb imprima la lista de claves primarias*/
		void exportar(ostream &archivoTexto,int posicion);		
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroGriego 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
class IndiceEnteroGriegoManager: public IndiceArbolManager
{
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////		
		IndiceEnteroGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
		
		virtual ~IndiceEnteroGriegoManager() {}
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		
		/*Interpreta un buffer como una clave de tipo ClaveEntera.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);

	
}; //Fin clave ArchivoIndiceEnteroGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroRomano 
//		   (Implementa archivo de indices secundarios de clave entera).
///////////////////////////////////////////////////////////////////////////
class IndiceEnteroRomanoManager: public IndiceSecundarioManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceEnteroRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice);
		~IndiceEnteroRomanoManager();
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveEntera.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase ArchivoIndiceEnteroRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceBooleanGriego 
//		   (Implementa archivo de indices primarios de clave booleana).
///////////////////////////////////////////////////////////////////////////
//TODO Cambiar IndiceBooleanGriegoManager por IndiceBooleanRomanoManager
class IndiceBooleanGriegoManager: public IndiceArbolManager
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		/*Interpreta un buffer como una clave de tipo ClaveBoolean.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////		
		IndiceBooleanGriegoManager(unsigned int tamNodo,string nombreArchivo, unsigned char tipoIndice);
		
		~IndiceBooleanGriegoManager() {}
	
}; //Fin clase ArchivoIndiceBooleanGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharGriego
//		   (Implementa archivo de indices primarios de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class IndiceCharGriegoManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceCharGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
		
		~IndiceCharGriegoManager() {}
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveBoolean.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	

	
}; //Fin clase ArchivoIndiceCharGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharRomano
//		   (Implementa archivo de indices secundarios de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class IndiceCharRomanoManager: public IndiceSecundarioManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceCharRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice);

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Interpreta un buffer como una clave de tipo ClaveChar.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase ArchivoIndiceCharGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceShortGriego
//		   (Implementa archivo de indices primarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
class IndiceShortGriegoManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceShortGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveShort.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase ArchivoIndiceShortGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceShortRomano
//		   (Implementa archivo de indices primarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
class IndiceShortRomanoManager: public IndiceSecundarioManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceShortRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice);

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveShort.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
		
}; //Fin clase ArchivoIndiceShortRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceRealRomano
//		   (Implementa archivo de indices primarios de clave de tipo float).
///////////////////////////////////////////////////////////////////////////
class IndiceRealRomanoManager: public IndiceSecundarioManager
{	
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceRealRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice);

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveReal.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
}; //Fin clase ArchivoIndiceRealRomano


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceFechaGriegoManager
//		   (Implementa archivo de indices primarios de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////
class IndiceFechaGriegoManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceFechaGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveFecha.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);	
		
}; //Fin clase ArchivoIndiceFechaGriego

//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: IndiceFechaGriegoManager
//		   (Implementa archivo de indices secundarios de clave de tipo fecha).
//////////////////////////////////////////////////////////////////////////////
class IndiceFechaRomanoManager: public IndiceSecundarioManager
{	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceFechaRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		/*Interpreta un buffer como una clave de tipo ClaveFecha.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);

	
}; //Fin clase ArchivoIndiceFechaRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceVariableGriego 
//		   (Implementa archivo de indices primarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
class IndiceVariableGriegoManager: public IndiceArbolManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceVariableGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice);	

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
// Nombre: IndiceVariableRomanoManager 
//		   (Implementa archivo de indices secundarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
class IndiceVariableRomanoManager: public IndiceSecundarioManager
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceVariableRomanoManager(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, unsigned char tipoIndice);
	
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
// Nombre: IndiceCompuestoGriegoManager 
//		   (Implementa archivo de indices primarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////
typedef list<int> ListaTipos;

class IndiceCompuestoGriegoManager: public IndiceArbolManager
{
	private:
		ListaTipos* tipos;
		
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		IndiceCompuestoGriegoManager(unsigned int tamNodo, string nombreArchivo, unsigned char tipoIndice, ListaTipos* listaTipos);
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves compuestas
		 * variable y copiarla al buffer de escritura.
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer claves compuestas de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
};

//TODO Implementar IndiceCompuestoRomanoManager

#endif /*INDICEMANAGER_H_*/