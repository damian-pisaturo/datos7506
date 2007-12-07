////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase ArchivoBase. 
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef ARCHIVOBASE_H_
#define ARCHIVOBASE_H_

#include "../../Common/Archivo/Archivo.h"
#include "../../Common/Tamanios.h"
#include "../../Common/ResultadosFisica.h"
#include <sstream>

using namespace std;


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoBase
//			(Abstracta. Implementa primitivas para el manejo de archivos
//           organizados en bloques fijos en disco).
///////////////////////////////////////////////////////////////////////////
class ArchivoBase
{	
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		Archivo archivo;  //Referencia al archivo
	    unsigned short tamBloque;  //Tamaño en bytes del bloque

	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Abre el archivo. Si no existe lo crea. Los primeros
		 * Tamanios::TAMANIO_LONGITUD bytes contienen el tamaÃ±o de
		 * un bloque del archivo. El primer byte del archivo contiene
		 *  un identificador mÃ¡gico.
		 */
		ArchivoBase(string nombre, unsigned short tamBloque);
		
		/*Cierra el archivo*/
		virtual ~ArchivoBase();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		 
		/*Escribe el bloque en el archivo en la posiciÃ³n actual.
		  * Recibe un puntero a una estructura de datos de tamaÃ±o tamBloque.
		  * Devuelve ResFisica::ERROR_ESCRITURA si se produjo algun error al
		  * escribir el bloque; ResFisica::NO_ABIERTO si el archivo no fue 
		  * previamente abierto, o ResFisica::OK de no haberse encontrado errores.
		 */		 
		char escribir(const void* bloque);
		
		/* Lee del archivo el bloque en la posiciÃ³n actual y lo devuelve
		 * en 'bloque'. Devuelve ResFisica::ERROR_LECTURA si se produjo algun 
		 * error al leer el bloque solicitado; ResFisica::NO_ABIERTO si el 
		 * archivo no fue previamente abierto, o ResFisica::OK de no haberse
		 * producido errores.
		 */
		char leer(void* bloque);
		
		/*
		 * Devuelve true si se ha llegado al fin del archivo;
		 * false en caso contrario.
		*/
		bool fin();
		
		/* Devuelve la posición relativa del bloque actual o
		 * ResFisica::NO_ABIERTO si el archivo no fue previamente
		 * abierto.
		 */
		short posicion();
		
		/* Modifica la posición actual con el numero de bloque
		 * pasado por parametro. Si numBloque es invalido, devuelve
		 * ResFisica::ERROR_POSICION; en caso contrario, devuelve
		 * ResFisica::OK;
		*/
		char posicionarse(unsigned short numBloque);
		
		/* Se posiciona el final del archivo.
		 */
		void posicionarseFin();
		
		/*Devuelve en tamaño del archivo en bytes.
		 */
		size_t size();

	///////////////////////////////////////////////////////////////////////
	// Getters
	///////////////////////////////////////////////////////////////////////
		unsigned short getTamanioBloque();
		bool esValido();

}; /*Fin clase ArchivoBase*/

#endif /*ARCHIVOBASE_H_*/
