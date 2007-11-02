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

#include "ArchivoBase.h"
#include "ArchivoEL.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndice
//			(Abstracta. Define el comportamiento de las clases de manejo
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

/*********************** NOCHE EN EL FIJO **********************************
 *       
 *           Hernan D. LetCorchett vs. Nestor "CharGriego" Romano
 * 
 * Musicalizador: DJ Hash
 **************************************************************************/ 

///////////////////////////////////////////////////////////////////////////
// Clase
//-----------------------------------------------------------------------
// Nombre: ArchivoTablaHash
//			(Permite el manejo de archivos contenedores de los datos
//			de la tabla de dispersion utilizada en los indices de Hash).
///////////////////////////////////////////////////////////////////////////
class ArchivoTablaHash : ArchivoBase
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
		unsigned int* obtenerTabla();
		
		/*Actualiza la tabla de dispersion con los datos pasados por 
		 * parametro (cantidad de elementos y numero de buckets).
		 * escribirTabla() asume que una llamada previa a escribirCantElementos()
		 * escribio en el archivo la cantidad de elementos correcta que debe insertar.
		 */
		char escribirCantElementos(unsigned short cantElem);
		char escribirTabla(unsigned int* tabla);	
		
}; /*Fin clase ArchivoTablaHash*/

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
			void leerTabla(unsigned short* cantElem, unsigned int* elementos);
			
			/* Actualiza el contenido del archivo de tabla de dispersion
			 * con los elementos pasados por parametros.
			 */
			void escribirTabla(unsigned short cantElem, unsigned int* elementos);			
			
};

#endif /*ARCHIVOINDICEARBOL_H_*/

