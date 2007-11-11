////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases ArchivoIndice y ArchivoIndiceHash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef ARCHIVOINDICEARBOL_H_
#define ARCHIVOINDICEARBOL_H_

#include "../ArchivoBase/ArchivoBase.h"
#include "../ArchivoEL/ArchivoEL.h"
#include "../../Common/ResultadosFisica.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndice
//			(Define el comportamiento de las clases de manejo
//			 de archivos de indices en disco).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndice : public ArchivoBase
{
	private:
		ArchivoEL* archivoEL;
	
	public:
		///////////////////////////////////////////////////////////////////////
		// Constructor/Destructor
		///////////////////////////////////////////////////////////////////////
			ArchivoIndice(string nombreArchivo, unsigned short tamBloque);
			virtual ~ArchivoIndice();
		
		///////////////////////////////////////////////////////////////////////
		//	Metodos publicos
		///////////////////////////////////////////////////////////////////////
		    
			/*Añade un nuevo nodo en el primer bloque libre indicado
			 * por el archivo de control de espacio libre. Devuelve
			 * el numero de bloque en el que fue insertado finalmente.
			 */
			virtual short escribirBloque(const void* bloque);
			
			/*Modifica el nodo cuyo numero en el archivo es numNodo con el 
			 * contenido de 'nodo'.
			 */
			virtual char escribirBloque(const void* bloque, unsigned short numBloque);	
			
			/*Actualiza el archivo de control de espacio libre, modificando
			 * la entrada booleana correspondiente a numNodo.
			 */
			virtual char eliminarBloque(unsigned short numBloque);
			
			/*Devuelve en 'bloque' el bloque del archivo de indice cuya 
			 * posicion es numBloque.
			 */
			virtual char leerBloque(void* bloque, unsigned short numBloque);

		///////////////////////////////////////////////////////////////////////
		//	Getter
		///////////////////////////////////////////////////////////////////////	
			ArchivoEL* getArchivoEL()
			{
				return archivoEL;
			}
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoLista
//			(Permite el manejo de archivos contenedores de listas de 
//			claves primarias utilizadas por los indices secundarios).
///////////////////////////////////////////////////////////////////////////

class ArchivoLista : public ArchivoIndice
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoLista(string nombreArchivo, unsigned short tamBloqueLista);
		virtual ~ArchivoLista();
		
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Añade una nueva lista en el primer bloque libre indicado
		 * por el archivo de control de espacio libre. Devuelve
		 * el numero de bloque en el que fue insertada finalmente.
		 */
		short escribirLista(const unsigned int cantClaves, const void* lista);
		
		/*Modifica la lista cuyo numero en el archivo es numBloque con el 
		 * contenido de 'lista' conteniendo una cantidad cantClaves 
		 * de claves primarias.
		 */
		char escribirLista(const unsigned int cantClaves, const void* lista, unsigned short numBloque);	
		
		/*Actualiza el archivo de control de espacio libre, modificando
		 * la entrada booleana correspondiente a numBloque.
		 */
		char eliminarLista(unsigned short numBloque);
		
		/*Devuelve en 'lista' el bloque del archivo de indice cuya 
		 * posicion es numBloque y en cantClaves la cantidad de claves
		 * contenida en la lista.
		 */
		virtual char leerLista(unsigned int* cantClaves, void* lista, unsigned short numBloque);

}; /*Fin clase ArchivoLista*/

///////////////////////////////////////////////////////////////////////////
// Clase
//-----------------------------------------------------------------------
// Nombre: ArchivoTablaHash
//			(Permite el manejo de archivos contenedores de los datos
//			de la tabla de dispersion utilizada en los indices de Hash).
///////////////////////////////////////////////////////////////////////////
class ArchivoTablaHash :public ArchivoBase
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoTablaHash(string nombreArchivo);
		virtual ~ArchivoTablaHash();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		/*Obtiene los datos de la tabla de dispersion (cantidad de elementos
		 * y numeros de buckets en cada elemento).
		 */
		unsigned int obtenerCantElementos();
		unsigned int* obtenerTabla(unsigned int cantidadElementos);
		
		/*Actualiza la tabla de dispersion con los datos pasados por 
		 * parametro (cantidad de elementos y numero de buckets).
		 * escribirTabla() asume que una llamada previa a escribirCantElementos()
		 * escribio en el archivo la cantidad de elementos correcta que debe insertar.
		 */
		char escribirCantElementos(unsigned int cantElem);
		char escribirTabla(unsigned int* tabla);	
		
}; /*Fin clase ArchivoTablaHash*/

///////////////////////////////////////////////////////////////////////////
// Clase
//-----------------------------------------------------------------------
// Nombre: ArchivoIndiceHash
//			(Permite el manejo de archivos empleados en la persistencia de
//			datos a traves de un indice de dispersion extensible).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceHash : public ArchivoIndice
{
	private:
		ArchivoTablaHash* archivoTabla;
		
	public:
		///////////////////////////////////////////////////////////////////////
		// Constructor/Destructor
		///////////////////////////////////////////////////////////////////////
			ArchivoIndiceHash(string nombreArchivo, unsigned short tamNodo);
			virtual ~ArchivoIndiceHash();
		
		///////////////////////////////////////////////////////////////////////
		//	Metodos publicos
		///////////////////////////////////////////////////////////////////////
			
			/* Devuelve en cantElem la cantidad de elementos de la tabla
			 * de dispersion y en elementos sus valores.
			 */
			void leerTabla(unsigned int* cantElem, unsigned int* &elementos);
			
			/* Actualiza el contenido del archivo de tabla de dispersion
			 * con los elementos pasados por parametros.
			 */
			void escribirTabla(unsigned int cantElem, unsigned int* elementos);			
			
};

///////////////////////////////////////////////////////////////////////////
// Clase
//-----------------------------------------------------------------------
// Nombre: ArchivoIndiceArbol
//			(Permite el manejo de archivos empleados en la persistencia de
//			datos a traves de indices con estructura de arbol B+ o B*).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceArbol : public ArchivoIndice
{		
	public:
		///////////////////////////////////////////////////////////////////////
		// Constructor/Destructor
		///////////////////////////////////////////////////////////////////////
			ArchivoIndiceArbol(string nombreArchivo, unsigned short tamNodo);
			virtual ~ArchivoIndiceArbol();
		
		///////////////////////////////////////////////////////////////////////
		//	Metodos publicos
		///////////////////////////////////////////////////////////////////////
			
			/*Añade dos nuevos nodos en los dos primeros bloques libres indicados
			 * por el archivo de control de espacio libre. Devuelve
			 * el numero de bloque en el que fue insertado el primero de ellos.
			 */
			short escribirBloqueDoble(const void* nodo);
			
			/*Modifica los nodos cuyos numeros en el archivo son numNodo y numNodo + 1 
			 * con el contenido de 'nodo'.
			 */
			char escribirBloqueDoble(const void* nodo, unsigned short numBloque);	
			
			/*Actualiza el archivo de control de espacio libre, modificando
			 * las entradas booleana correspondiente a numNodo y numNodo + 1 .
			 */
			char eliminarBloqueDoble(unsigned short numNodo);
			
			/*Devuelve en 'nodo' los bloques del archivo de indice cuyas 
			 * posiciones son numNodo y numNodo + 1.
			 */
			char leerBloqueDoble(void* nodo, unsigned short numNodo);
			
}; /*Fin clase ArchivoIndiceArbol*/

#endif /*ARCHIVOINDICEARBOL_H_*/

