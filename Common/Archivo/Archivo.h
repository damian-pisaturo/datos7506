///////////////////////////////////////////////////////////////////////////
//	Archivo   : Archivo.h
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase Archivo.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#ifndef ARCHIVO_H_
#define ARCHIVO_H_
	
#include "../Tamanios.h"
#include <fstream>
#include <iostream>
using namespace std;


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Archivo
//			(Permite el manejo de archivo binarios, redefiniendo los 
// 			operadores << y >>).
///////////////////////////////////////////////////////////////////////////
class Archivo : public fstream
{
	private:
	///////////////////////////////////////////////////////////////////////
	// Atributo
	///////////////////////////////////////////////////////////////////////
		bool archivoValido; // Indica si el archivo es un archivo del proyecto valido
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		Archivo();
		Archivo(const string& nomArchivo);
		Archivo(const string& nomArchivo, unsigned int magicNumber);
		virtual ~Archivo();
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		/* Redefinicion del operador << para los tipos basicos y string.
		 */
		Archivo& operator<< (const int& val);
		Archivo& operator<< (const unsigned int& val);
		Archivo& operator<< (const short& val);
		Archivo& operator<< (const unsigned short& val);
		Archivo& operator<< (const char& val);
		Archivo& operator<< (const unsigned char& val);
		Archivo& operator<< (const float& val);
		Archivo& operator<< (const bool& val);
		Archivo& operator<< (const string& val);
		
		/* Redefinicion del operador >> para los tipos basicos y string.
		*/
		Archivo& operator>> (int& val);
		Archivo& operator>> (unsigned int& val);
		Archivo& operator>> (short& val);
		Archivo& operator>> (unsigned short& val);
		Archivo& operator>> (char& val);
		Archivo& operator>> (unsigned char& val);
		Archivo& operator>> (float& val);
		Archivo& operator>> (bool& val);
		Archivo& operator>> (string& val);
		 
		 /* Devuelve el tamaño en bytes del archivo.
		  */
		 size_t size();
		 
		 /* Coloca el puntero al comienzo del archivo.
		  */
		 void rewind();
		 
		 /* Coloca el puntero al final del archivo.
		  */
		 void end();
		 
		 bool esValido()
		 {
			 return this->archivoValido;
		 }
	
	private:
	///////////////////////////////////////////////////////////////////////
	// Metodo privado
	///////////////////////////////////////////////////////////////////////
		
		/* Crea y devuelve un identificador unico para el archivo actual.
		 */
		unsigned int generarID(const string &nomArchivo);
};

#endif /*ARCHIVO_H_*/
