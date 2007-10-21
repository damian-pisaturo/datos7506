#ifndef NODOBP_H_
#define NODOBP_H_

#include "Nodo.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: NodoBp (Implementa nodos de Arbol B+)
//////////////////////////////////////////////////////////////////////////
class NodoBp: public Nodo
{
	private:
		/*Referencia en nodos hoja al hermano derecho del nodo actual.
		 * Su valor en nodos del index-set es -1.
		 */
	//////////////////////////////////////////////////////////////////////
	// Atributos privados
	//////////////////////////////////////////////////////////////////////
		int hermanoSiguiente;
	
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		void insertarEnHoja(Clave* &clave, char* codigo);
		void insertarEnNodo(Clave* &clave, char* codigo);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructores
	//////////////////////////////////////////////////////////////////////
		NodoBp(int hijoIzq, unsigned int nivel, int hermanoSiguiente = -1): 
			Nodo(hijoIzq, nivel)
		{
			this->hermanoSiguiente = hermanoSiguiente;
		}
		
		NodoBp(int hijoIzq, unsigned int nivel, Clave* clave, int hermanoSiguiente = -1) : 
					Nodo(hijoIzq, nivel, clave)
		{
			this->hermanoSiguiente = hermanoSiguiente;
		}
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		virtual char insertarClave(Clave* &clave);	
		virtual char eliminarClave(Clave* clave);
	
	//////////////////////////////////////////////////////////////////////
	// Getters/Setters
	//////////////////////////////////////////////////////////////////////		
		int getHermanoSiguiente()
		{
			return this->hermanoSiguiente;
		}
		
		void setHermanoSiguiente(int referencia)
		{
			this->hermanoSiguiente = referencia;			
		}
}; //Fin clase NodoBp
v
#endif /*NODOBP_H_*/
