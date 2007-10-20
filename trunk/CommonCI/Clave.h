///////////////////////////////////////////////////////////////////////////
//	Archivo   : CClave.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase CCClave.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel.
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura
///////////////////////////////////////////////////////////////////////////

#ifndef CLAVE_H_
#define CLAVE_H_

#include <iostream>
using namespace std;

#include "Lista.h"


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: CClave (Abstracta)
//////////////////////////////////////////////////////////////////////////

class CClave
{	
	private:
	///////////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////////
		//regRegistro
		/*En claves primarias: offset al registro del archivo de datos (indice griego).
		  En claves secundarias: offset a una lista con claves primarias (indice romano).
		*/
		unsigned int refRegistro; 

		//hijoDer
		/*Referencia al nodo derecho del arbol 
		  (Claves mayores/menores a la clave actual).
		*/
		int hijoDer; 
		
		//valor
		/*Contenido de la clave dependiendo de su tipo.
		  Numerico, string, lista de claves (claves compuestas), etc.
		*/
		void* valor;
	
	protected:		
		unsigned int tamanio;
	
	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
		CClave()
		{
			hijoDer = -1;
			refRegistro = 0;
			valor = NULL;
		}

		virtual ~CClave(){};

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
		/*Compara la CClave actual con otraCClave, 
		  dependiendo el tipo de CClave.
		*/
		virtual char comparar(CClave* otraClave) = 0;
		
		/*Devuelve una copia de la CClave*/
		virtual CClave* copiar() = 0;
		
		/*Imprime la CClave por salida*/
		virtual void imprimir(ostream& salida) = 0;

	///////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////////
		virtual unsigned int obtenerReferencia()
		{
			return refRegistro;
		}
		
		virtual void setValor(void* nuevoValor)
		{
			this->valor = nuevoValor;
		}
		
		virtual void setReferencia(unsigned int referencia)
		{
			this->refRegistro = referencia;
		}

		virtual void setHijoDer(int hijoDer)
		{
			this->hijoDer = hijoDer;
		}
		
		virtual void* getValor()
		{
			return this->valor;
		}
		
		virtual unsigned int getTamanio()
		{
			return this->tamanio;
		}

		virtual int getHijoDer()
		{
			return this->hijoDer;
		}
}; //Fin clase CClave (Abstracta)
 
///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveEntera 
// Descripcion: Implementa claves de tipo enteras (longitud fija).
///////////////////////////////////////////////////////////////////////////
class CClaveEntera: public CClave
{
	public:		
	///////////////////////////////////////////////////////////////////////////
	// Constructores/Destructor
	///////////////////////////////////////////////////////////////////////////
 		CClaveEntera();
 		CClaveEntera(int clave, unsigned int referencia, int hijoDer = -1);

		virtual ~CClaveEntera();

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
		/*Devuelve una copia de la CClave*/
 		virtual CClave* copiar();
		
		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave, 
		  1 si es mayor
		*/
		virtual char comparar(CClave* otraClave);
			
		virtual void imprimir(ostream& salida);
			 
}; //Fin clase CClaveEntera


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveBoolean
// Descripcion: Implementa claves de tipo booleanas (verdadero/falso).
///////////////////////////////////////////////////////////////////////////
class CClaveBoolean: public CClave
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		CClaveBoolean();
		CClaveBoolean(bool clave, unsigned int referencia, int hijoDer = -1);
		virtual ~CClaveBoolean();

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
 		/*Devuelve una copia de la clave*/
 		virtual CClave* copiar();
		
		/*Devuelve 0 si es igual, 1 si el valor booleano es distinto.*/
		virtual char comparar(CClave* otraClave);
		
		virtual void imprimir(ostream& salida);			
			 
}; //Fin clase CClaveBoolean.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveChar
// Descripcion: Implementa claves de tipo char.
///////////////////////////////////////////////////////////////////////////
class CClaveChar: public CClave
{
	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
		CClaveChar::CClaveChar();
		CClaveChar(char clave, unsigned int referencia, int hijoDer = -1);
		virtual ~CClaveChar();

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
		
 		/*Devuelve una copia de la clave*/
 		virtual CClave* copiar();
		
		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(CClave* otraClave);
		
		virtual void imprimir(ostream& salida);			
			 
}; //Fin clase CClaveChar.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveShort
// Descripcion: Implementa claves de tipo short.
///////////////////////////////////////////////////////////////////////////
class CClaveShort: public CClave
{
	public:
	////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	////////////////////////////////////////////////////////////////////////
		CClaveShort();
		CClaveShort(short clave, unsigned int referencia, int hijoDer = -1);
		virtual ~CClaveShort();

	////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	////////////////////////////////////////////////////////////////////////
		
 		/*Devuelve una copia de la clave*/
 		virtual CClave* copiar();
		
		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(CClave* otraClave);
		
		virtual void imprimir(ostream& salida);			
			 
}; //Fin clase CClaveShort.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveReal
// Descripcion: Implementa claves de tipo float.
///////////////////////////////////////////////////////////////////////////
class CClaveReal: public CClave
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		CClaveReal();
		CClaveReal(float clave, unsigned int referencia, int hijoDer = -1);
		virtual ~CClaveReal();

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////		
 		/*Devuelve una copia de la clave*/
 		virtual CClave* copiar();
		
		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(CClave* otraClave);
		
		virtual void imprimir(ostream& salida);			
			 
}; //Fin clase CClaveReal.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveVariable
// Descripcion: Implementa claves de longitud variable.
///////////////////////////////////////////////////////////////////////////
class CClaveVariable: public CClave
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		//Longitud maxima de la cadena a almacenar.
		//static const unsigned char longMaxCadena = 40; 
		
		/*La longitud actual de la cadena puede obtenerse mediante
		  el metodo size() de la clase string. Aunque no este como atributo, 
		  debe persistirse en disco.
		*/

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		CClaveVariable(string clave, unsigned int referencia, int hijoDer = -1);
		virtual ~CClaveVariable();

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////		
 		/*Devuelve una copia de la clave*/
 		virtual CClave* copiar();
		
		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(CClave* otraClave);
		
		virtual void imprimir(ostream& salida);			
			 
}; //Fin clase CClaveVariable.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveFecha
// Descripcion: Implementa claves de tipo fecha (AAAAMMDD).
///////////////////////////////////////////////////////////////////////////
class CClaveFecha: public CClave
{
	private:

		/*ATTENZIONE!
			NO SE SI EL FORMATO CORRESPONDE AL QUE ESTA EN EL
			ENUNCIADO (pareciera querer decir que la fecha es un string 
			de 6 bytes o un entero del estilo "20071119"), PERO DE ESTA FORMA
			SON BASTANTE SENCILLAS LAS COMPARACIONES Y EL SIZE DEL DATO ES MINIMO.
		*/
		typedef struct TFECHA{
			short anio;
			char mes;
			char dia;

			bool crear(short a, char m, char d)
			{
				if ((a > 9999) || (m > 12) || (d > 31)) 
					return false;
				else{					
					this->anio = a;
					this->mes  = m;
					this->dia  = d;				
				}

				return true;
			}
		}fecha;

	public:
	/////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	/////////////////////////////////////////////////////////////////////////
		CClaveFecha();
		CClaveFecha(fecha* clave, unsigned int referencia, int hijoDer = -1);
		virtual ~CClaveFecha();

	/////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	/////////////////////////////////////////////////////////////////////////
		
 		/*Devuelve una copia de la clave*/
 		virtual CClave* copiar();
		
		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(CClave* otraClave);
		
		virtual void imprimir(ostream& salida);			
			 
}; //Fin clase CClaveFecha.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveCompuesta
// Descripcion: Implementa claves compuestas (lista de claves).
///////////////////////////////////////////////////////////////////////////
			
class CClaveCompuesta: public CClave
{

	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructores/Destructor
	///////////////////////////////////////////////////////////////////////////
		ClaveCompuesta(CListaClaves* listaClaves, int hijoDer = -1);
		virtual ~CClaveCompuesta();

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
 			
		/*Devuelve una copia de la CClave*/
		virtual CClave* copiar();
 			
		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor. Compara secuencialmente en el orden de la lista
		  de claves.
		*/
		virtual char comparar(CClave* otraClave);

		virtual void imprimir(ostream& salida);
}; //Fin clase CClaveCompuesta.


#endif /*CLAVE_H_*/
