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
			virtual unsigned short insertarBloque(void* bloque) = 0;
			
			/*Modifica el nodo cuyo numero en el archivo es numNodo con el 
			 * contenido de 'nodo'.
			 */
			virtual char modificarBloque(void* bloque, unsigned short numBloque) = 0;	
			
			/*Actualiza el archivo de control de espacio libre, modificando
			 * la entrada booleana correspondiente a numNodo.
			 */
			virtual char eliminarBloque(unsigned short numBloque) = 0;

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
 *           Hernan D. LetCorchett. vs. Nestor "CharGriego" Romano
 * 
 * Musicalizador: DJ Hash
 **************************************************************************/ 


class ArchivoIndiceHash : public ArchivoIndice
{
	private:
		fstream archivoTabla;
		
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

