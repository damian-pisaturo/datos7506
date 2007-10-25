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
		
		unsigned getTamanioEnDisco() const;

};

#endif /*NODOBSTAR_H_*/
