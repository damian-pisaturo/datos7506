///////////////////////////////////////////////////////////////////////////
//	Archivo   : BloqueListaManager.h
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase BloqueListaManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef BLOQUELISTAMANAGER_H_
#define BLOQUELISTAMANAGER_H_

#include "../BloqueManager.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BloqueListaManager
//			(Permite la escritura, lectura, modificacion y eliminacion de
//			bloques de listas de claves primarias indexados en indices 
//			secundarios, accediendo remotamente a la Capa Fisica).
///////////////////////////////////////////////////////////////////////////

class BloqueListaManager : public BloqueManager
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		BloqueListaManager(unsigned int tamanioBloqueLista, string nombreArchivo);		
		virtual ~BloqueListaManager();
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		/*Recibe un buffer creado/inicializado de tamaño 'tamanioBloqueLista' 
		 * y lo llena con la informacion leida del archivo de listas de
		 * claves primarias en la posicion 'numBloque'.
		 * Utiliza ComuDatos para comunicarse con la Capa Fisica.
		 */
		int leerBloqueDatos(unsigned int numLista, void* listaLeida);		

		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y escribir
		 * la 'listaNueva' conteniendo una cantidad de claves igual a 'cantClaves'
		 *  en el archivo de listas de claves primarias especificado
		 *  por 'nombreArchivo'.
		 * Devuelve la posicion donde fue escrita finalmente.
		 */
		int escribirBloqueDatos(const void* listaNueva);
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque de listas de claves primarias en la posicion
		 * 'numLista' del archivo especificado por nombreArchivo con la lista
		 * modificada 'listaModif', contedora de una cantidad de claves igual
		 * a 'cantClaves'.
		 */
		int escribirBloqueDatos(unsigned short numLista, const void* listaModif);
		
		/* Este metodo abstracto heredado no recibe implementacion en esta clase.
		 */
		virtual int eliminarBloqueDatos(unsigned int numLista);
		
		/* Este metodo abstracto heredado no recibe implementacion en esta clase.
		*/
		virtual int buscarEspacioLibre(void* bloque, unsigned int espLibre);

	
		
}; /*Fin clase BloqueListaManager*/

#endif /*BLOQUELISTAMANAGER_H_*/
