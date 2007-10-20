///////////////////////////////////////////////////////////////////////////
//	Archivo   : Clave.h
//  Namespace : CapaIndice
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de la clase Clave.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura
///////////////////////////////////////////////////////////////////////////

#ifndef CLAVE_H_
#define CLAVE_H_

#include <iostream>
#include <list>

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Clave (Abstracta)
//////////////////////////////////////////////////////////////////////////

class Clave
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
		  (Claves mayores a la clave actual).
		*/
		unsigned int hijoDer;

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
		Clave() : refRegistro(0), hijoDer(0), valor(NULL), tamanio(0) {}

		virtual ~Clave() {}

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
		/*Compara la Clave actual con otraClave,
		  dependiendo el tipo de Clave.
		*/
		virtual char comparar(Clave* otraClave) = 0;

		/*Devuelve una copia de la Clave*/
		virtual Clave* copiar() = 0;

		/*Imprime la Clave por salida*/
		virtual void imprimir(ostream& salida) = 0;

		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const = 0;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const = 0;

	///////////////////////////////////////////////////////////////////////////
	// Getters/Setters
	///////////////////////////////////////////////////////////////////////////
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor) = 0;
		
		unsigned int obtenerReferencia() const
		{
			return refRegistro;
		}

		void setReferencia(unsigned int referencia)
		{
			this->refRegistro = referencia;
		}

		void setHijoDer(unsigned int hijoDer)
		{
			this->hijoDer = hijoDer;
		}

		void* getValor() const
		{
			return this->valor;
		}

		unsigned int getTamanio() const
		{
			return this->tamanio;
		}

		unsigned int getHijoDer() const
		{
			return this->hijoDer;
		}
}; //Fin clase Clave (Abstracta)


//Estructura necesaria para comparar dos objetos de tipo Clave, dentro de un
//contenedor de la STL
struct Comparador {
  bool operator()(const Clave* c1, const Clave* c2) const {
    return (*c1 < *c2);
  }
};


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveEntera
// Descripcion: Implementa claves de tipo enteras (longitud fija).
///////////////////////////////////////////////////////////////////////////
class ClaveEntera: public Clave
{
	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructores/Destructor
	///////////////////////////////////////////////////////////////////////////
 		ClaveEntera();
 		ClaveEntera(int clave, unsigned int referencia = 0,
                                unsigned int hijoDer = 0);

		virtual ~ClaveEntera();

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
		/*Devuelve una copia de la Clave*/
 		virtual Clave* copiar();

		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveEntera


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveBoolean
// Descripcion: Implementa claves de tipo booleanas (verdadero/falso).
///////////////////////////////////////////////////////////////////////////
class ClaveBoolean: public Clave
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ClaveBoolean();
		ClaveBoolean(bool clave, unsigned int referencia = 0,
									unsigned int hijoDer = 0);
		virtual ~ClaveBoolean();

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

 		/*Devuelve una copia de la clave*/
 		virtual Clave* copiar();

 		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
 		1 si es mayor*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveBoolean.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveChar
// Descripcion: Implementa claves de tipo char.
///////////////////////////////////////////////////////////////////////////
class ClaveChar: public Clave
{
	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
		ClaveChar();
		ClaveChar(char clave, unsigned int referencia = 0,
								unsigned int hijoDer = 0);
		virtual ~ClaveChar();

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////

 		/*Devuelve una copia de la clave*/
 		virtual Clave* copiar();

		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveChar.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveShort
// Descripcion: Implementa claves de tipo short.
///////////////////////////////////////////////////////////////////////////
class ClaveShort: public Clave
{
	public:
	////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	////////////////////////////////////////////////////////////////////////
		ClaveShort();
		ClaveShort(short clave, unsigned int referencia = 0,
									unsigned int hijoDer = 0);
		virtual ~ClaveShort();

	////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	////////////////////////////////////////////////////////////////////////

 		/*Devuelve una copia de la clave*/
 		virtual Clave* copiar();

		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveShort.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveReal
// Descripcion: Implementa claves de tipo float.
///////////////////////////////////////////////////////////////////////////
class ClaveReal: public Clave
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ClaveReal();
		ClaveReal(float clave, unsigned int referencia = 0,
								unsigned int hijoDer = 0);
		virtual ~ClaveReal();

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
 		/*Devuelve una copia de la clave*/
 		virtual Clave* copiar();

		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveReal.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveVariable
// Descripcion: Implementa claves de longitud variable.
///////////////////////////////////////////////////////////////////////////
class ClaveVariable: public Clave
{
	public:
	///////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////
		//Longitud maxima de la cadena a almacenar.
		static const unsigned char longMaxCadena = 40;

		/*La longitud actual de la cadena puede obtenerse mediante
		  el metodo size() de la clase string. Aunque no esté como atributo,
		  debe persistirse en disco.
		*/

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ClaveVariable(const string& clave, unsigned int referencia = 0,
											unsigned int hijoDer = 0);
		virtual ~ClaveVariable();

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
 		/*Devuelve una copia de la clave*/
 		virtual Clave* copiar();

		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveVariable.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveFecha
// Descripcion: Implementa claves de tipo fecha (AAAAMMDD).
///////////////////////////////////////////////////////////////////////////
class ClaveFecha: public Clave
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
		ClaveFecha();
		ClaveFecha(fecha* clave, unsigned int referencia = 0,
									unsigned int hijoDer = 0);
		virtual ~ClaveFecha();

	/////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	/////////////////////////////////////////////////////////////////////////

 		/*Devuelve una copia de la clave*/
 		virtual Clave* copiar();

		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor
		*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveFecha.


//Definición del tipo de dato de la lista de claves utilizada en la clase
//ClaveCompuesta

typedef list<Clave*> ListaClaves;

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveCompuesta
// Descripcion: Implementa claves compuestas (lista de claves).
///////////////////////////////////////////////////////////////////////////

class ClaveCompuesta: public Clave
{
    private:
    	//En esta lista las claves que componen la clave compuesta se almacenan
    	//por orden de prioridad
        ListaClaves listaClaves;
	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructores/Destructor
	///////////////////////////////////////////////////////////////////////////
		ClaveCompuesta(const ListaClaves& listaClaves,
						unsigned int referencia = 0,
						unsigned int hijoDer = 0);
		virtual ~ClaveCompuesta();

	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////

		/*Devuelve una copia de la Clave*/
		virtual Clave* copiar();

		/*Devuelve 0 si es igual, -1 si la clave es menor a la otraClave,
		  1 si es mayor. Compara secuencialmente en el orden de la lista
		  de claves.
		*/
		virtual char comparar(Clave* otraClave);

		virtual void imprimir(ostream& salida);

}; //Fin clase ClaveCompuesta.


#endif /*CLAVE_H_*/
