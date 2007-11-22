/////////////////////////////////////////////////////////////////////////
//	Archivo   : BloqueManager.h
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase abstracta BloqueManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#ifndef BLOQUEMANAGER_H_
#define BLOQUEMANAGER_H_

#include "../Manager.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BloqueManager
//			(Abstracta. Define el comportamiento de las clases que leen
//			bloques datos -indices primarios- o lista de claves 
//			-indices secundarios- de disco).
///////////////////////////////////////////////////////////////////////////

class BloqueManager : public Manager
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		BloqueManager(unsigned int tamanioBloque, string nombreArchivo):
			Manager(tamanioBloque, nombreArchivo)
		{ }
		
		virtual ~BloqueManager()
		{ }
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos (abstractos)
	///////////////////////////////////////////////////////////////////////
		/*Recibe un bloque creado/inicializado de tamaño 'tamanioBloque' y lo 
		 * llena con la informacion leida del archivo de datos en la posicion 
		 * 'numBloque'.
		 * Utiliza ComuDatos para comunicarse con la Capa Fisica.
		 */
		virtual int leerBloqueDatos(unsigned int numBloque, void* bloqueLeido) = 0;		

		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y escribir
		 * el bloqueNuevo en el archivo especificado por nombreArchivo. 
		 * Devuelve la posicion donde fue escrito finalmente.
		 */
		virtual int escribirBloqueDatos(const void* bloqueNuevo) = 0;
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque modificado en el disco
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		virtual int escribirBloqueDatos(unsigned short numBloque, const void* bloqueModif) = 0;
		
		/*Actualiza el archivo de espacio libre, indicando que el bloque cuya
		 * posicion en el archivo es 'numBloque' esta disponible para escritura.
		 */
		virtual int eliminarBloqueDatos(unsigned int numBloque) = 0;
		
		/*Emplea ComuDatos para acceder al archivo de control de espacio
		 * libre y buscar el primer bloque de datos que contenga, al menos,
		 * una cantidad de bytes libres igual a 'espLibre'. Si lo encuentra,
		 * lo devuelve en 'bloque'; en caso contrario 
		 * devuelve ResultadosFisica::BLOQUES_OCUPADOS o ResultadosFisica::ARCHIVO_VACIO.
		 */
		virtual int buscarEspacioLibre(void* bloque, unsigned int espLibre) = 0;

	
}; /*Fin clase BloqueManager*/


#endif /*BLOQUEMANAGER_H_*/
