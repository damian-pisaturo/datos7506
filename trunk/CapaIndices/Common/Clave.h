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

#include "../../Common/Tamanios.h"
#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Clave;

typedef vector<Clave*> VectorClaves;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Clave (Abstracta)
//////////////////////////////////////////////////////////////////////////

class Clave
{
	protected:
	///////////////////////////////////////////////////////////////////////////
	// Atributos
	///////////////////////////////////////////////////////////////////////////
		//regRegistro
		/*En claves primarias: offset al registro del archivo de datos (indice griego).
		  En claves secundarias: offset a una lista con claves primarias (indice romano).
		*/
		unsigned int refRegistro;

		//hijoDer
		/*Referencia al nodo derecho del arbol (Claves mayores a la clave actual).*/
		unsigned int hijoDer;

		//valor
		/*Contenido de la clave dependiendo de su tipo.
		  Numerico, string, lista de claves (claves compuestas), etc.
		*/
		void* valor;


	public:
	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
		Clave() : refRegistro(0), hijoDer(0), valor(NULL) {}

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
		
		//El tamanio de la clave depende de si se va a insertar en un nodo interno o
		//en un nodo hoja. Representa el tamaño que ocupa la misma en disco.
		//Si hijoDer == 0, significa que la clave se va a insertar en un nodo hoja,
		//por lo cual no se tendra en cuenta el tamaño de este atributo. En cualquier
		//otro caso se considerará el tamaño del hijo derecho.
		//El valor 0 hace referencia al nodo Raíz, pero ningún nodo puede tener como hijo
		//al nodo raíz, por lo tanto utilizar este valor no presenta inconvenientes.
		virtual unsigned int getTamanioEnDisco(bool bstar) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor) = 0;
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const = 0;
		
		
		unsigned int getReferencia() const
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

		virtual void* getValor() const
		{
			return this->valor;
		}

		unsigned int getHijoDer() const
		{
			return this->hijoDer;
		}
		
		virtual void** getValorParaHash() const;
		
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
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;

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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;

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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;

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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;

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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;

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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;
		
		//Devuelve el char* contenido en el valor string.
		virtual void* getValor() const;

}; //Fin clase ClaveVariable.

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveFecha
// Descripcion: Implementa claves de tipo fecha (AAAAMMDD).
///////////////////////////////////////////////////////////////////////////
class ClaveFecha: public Clave
{
	public:
		
		typedef struct TFECHA {
			
			unsigned short anio;
			unsigned char mes;
			unsigned char dia;
			
			bool crear(unsigned char dia, unsigned char mes, unsigned short anio) {
				if ((anio > 9999) || (mes > 12) || (dia > 31)) {
					this->anio = 0;
					this->mes  = 0;
					this->dia  = 0;
					return false;
				} else {					
					this->anio = anio;
					this->mes  = mes;
					this->dia  = dia;				
				}
				return true;
			}
			
		} fecha;

	/////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	/////////////////////////////////////////////////////////////////////////
		ClaveFecha(const TFECHA &clave, unsigned int referencia = 0,
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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;

}; //Fin clase ClaveFecha.


//Definición del tipo de dato de la lista de claves utilizada en la clase
//ClaveCompuesta

typedef list<Clave*> ListaClaves;

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveCompuesta
// Descripcion: Implementa claves compuestas (lista de claves simples).
///////////////////////////////////////////////////////////////////////////

class ClaveCompuesta: public Clave
{
    //En esta clase el atributo void* apunta a una lista de claves (ListaClaves).
    //En esta lista las claves que componen la clave compuesta se almacenan
    //por orden de prioridad.
    	
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
		
		//Operador "Menor" utilizado para comprobar si una clave es menor a otra
		virtual bool operator < (const Clave& clave) const;

		//Operador "Igual" utilizado para comprobar si una clave es igual a otra
		virtual bool operator == (const Clave& clave) const;
		
		//Este método es virtual para que cada clase heredera pueda liberar la memoria
		//utilizada por el valor anterior, según el tipo de dato que utilice.
		virtual void setValor(void* nuevoValor);
		
		//Este método es virtual para que cada clase heredera devuelva el tamaño del
		//tipo de dato que usa internamente.
		virtual unsigned int getTamanioValor() const;
		
		//Devuelve la lista de claves simples que componen esta clave
		ListaClaves* getListaClaves() const;
		
		void** getValorParaHash() const;
		
	private:
		//Método para liberar la memoria utilizada por la lista y por todas las claves
		//que contiene
		void eliminarLista();
		
		//Método que calcula el tamaño total de todos los valores de las claves de la lista.
		unsigned int calcularTamanioValoresClaves(const ListaClaves& listaClaves) const;
		
		int getCantidadClaves() const;

}; //Fin clase ClaveCompuesta.


#endif /*CLAVE_H_*/
