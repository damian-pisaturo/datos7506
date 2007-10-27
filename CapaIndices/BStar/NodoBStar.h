#ifndef NODOBSTAR_H_
#define NODOBSTAR_H_

#include "../Common/Nodo.h"

class NodoBStar : public Nodo {
	
	public:
		NodoBStar(unsigned int refNodo, unsigned char nivel, unsigned short tamanio);
		
		NodoBStar(unsigned int refNodo, unsigned char nivel, Clave* clave,
				  unsigned short tamanio);
		
		virtual ~NodoBStar() {}
	
		void insertarClave(Clave* &clave, char* codigo);
		
		void eliminarClave(Clave* clave, char* codigo);
		
		Nodo* siguiente(Clave* clave);
		
		unsigned short getTamanioEnDisco() const;
		
		//Devuelve el espacio (en bytes) destinado para almacenar claves (elementos).
		//Esta es una redefinición del método de la clase padre.
		virtual unsigned short getTamanioEspacioClaves() const;
		
		VectorConjuntos* split();

};

#endif /*NODOBSTAR_H_*/
