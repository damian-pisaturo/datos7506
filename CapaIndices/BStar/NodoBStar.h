#ifndef NODOBSTAR_H_
#define NODOBSTAR_H_

#include "../Common/Nodo.h"

class NodoBStar : public Nodo {
	
	public:
		NodoBStar(unsigned int refNodo, unsigned int nivel) : Nodo(refNodo, nivel) {}
		NodoBStar(unsigned int refNodo, unsigned int nivel, Clave* clave) : Nodo(refNodo, nivel, clave) {}
		virtual ~NodoBStar() {}
	
		void insertarClave(Clave* &clave, char* codigo);
		void eliminarClave(Clave* clave, char* codigo);
		Nodo* siguiente(Clave* clave);
		
};

#endif /*NODOBSTAR_H_*/
