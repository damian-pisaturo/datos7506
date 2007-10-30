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

#include "../Common/Tamanios.h"

#include <iostream>
#include <fstream>

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: CodArchivo
//			(Clase utilitaria con constantes de error y resultado de 
//			metodos para la clase ArchivoBase).
///////////////////////////////////////////////////////////////////////////
class CodArchivo
{	
	public:
		static const char OK               = -1;
		static const char NO_ABIERTO       = -2;
		static const char ERROR_ESCRITURA  = -3;
		static const char ERROR_LECTURA    = -4;
		static const char ERROR_POSICION   = -5;
		static const char BLOQUES_OCUPADOS = -6;
		
}; /*Fin clase CodArchivo*/

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
		fstream archivo;  //Referencia al archivo
	    unsigned short tamBloque;  //Tamaño en bytes del bloque

	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		
		/*Abre el archivo. Si no existe lo crea. Los primeros
		 * Tamanios::TAMANIO_LONGITUD bytes contienen el tamaño de
		 * un bloque del archivo. 
		 */
		ArchivoBase(string nombre, unsigned short tamBloque);
		
		/*Cierra el archivo*/
		~ArchivoBase();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		 
		/*Escribe el bloque en el archivo en la posición actual.
		  * Recibe un puntero a una estructura de datos de tamaño tamBloque.
		  * Devuelve CodArchivo::ERROR_ESCRITURA si se produjo algun error al
		  * escribir el bloque; CodArchivo::NO_ABIERTO si el archivo no fue 
		  * previamente abierto, o CodArchivo::OK de no haberse encontrado errores.
		 */		 
		char escribir(const void* bloque);
		
		/* Lee del archivo el bloque en la posición actual y lo devuelve
		 * en 'bloque'. Devuelve CodArchivo::ERROR_LECTURA si se produjo algun 
		 * error al leer el bloque solicitado; CodArchivo::NO_ABIERTO si el 
		 * archivo no fue previamente abierto, o CodArchivo::OK de no haberse
		 * producido errores.
		 */
		char leer(void* bloque);
		
		/*
		 * Devuelve true si se ha llegado al fin del archivo;
		 * false en caso contrario.
		*/
		bool fin();
		
		/* Devuelve la posición relativa del bloque actual o
		 * CodArchivo::NO_ABIERTO si el archivo no fue previamente
		 * abierto.
		 */
		short posicion();
		
		/* Modifica la posición actual con el numero de bloque
		 * pasado por parametro. Si numBloque es invalido, devuelve
		 * CodArchivo::ERROR_POSICION; en caso contrario, devuelve
		 * CodArchivo::OK;
		*/
		char posicionarse(unsigned int numBloque);		

		/* Se posiciona al final del archivo.
		*/
		void posicionarseFin();

	///////////////////////////////////////////////////////////////////////
	// Getter
	///////////////////////////////////////////////////////////////////////
		unsigned short getTamanioBloque();
		
}; /*Fin clase ArchivoBase*/

#endif /*ARCHIVOBASE_H_*/
