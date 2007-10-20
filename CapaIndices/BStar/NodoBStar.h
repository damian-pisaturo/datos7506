#ifndef NODOBSTAR_H_
#define NODOBSTAR_H_

#include "../Common/Nodo.h"

class NodoBStar : public Nodo {
	public:
		NodoBStar(int hijoIzq, int nivel);
		NodoBStar(int hijoIzq,int nivel,Clave* clave);
		NodoBStar(int referencia);
		virtual ~NodoBStar();
	
		char insertarClave(Clave* &clave);
		char eliminarClave(Clave* clave);
		Clave* buscar(Clave* claveBuscada);
};

#endif /*NODOBSTAR_H_*/
