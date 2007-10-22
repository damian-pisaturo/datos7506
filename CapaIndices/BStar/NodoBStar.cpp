#include "NodoBStar.h"


void NodoBStar::insertarClave(Clave* &clave, char* codigo) {
	
	
	
}


void NodoBStar::eliminarClave(Clave* clave, char* codigo) {
	
	
	
}


Nodo* NodoBStar::siguiente(Clave* clave) {
	
	//Este metodo es llamado solamente si en este nodo no se encontro
	//la clave buscada.
	
	Nodo* nodo;
	Clave* claveResultante = this->getClaves()->findClave(clave);
	
	if (claveResultante == NULL) {
		//Cargar un nuevo nodo en memoria a partir del hijo izquierdo
		//de este nodo.
		//archivo->cargarNodo(nodo, this->refNodo);
	} else {
		//Cargar un nuevo nodo en memoria a partir del hijo derecho
		//de claveResultante.
		//archivo->cargarNodo(nodo, claveResultante->getHijoDer());
	}
	
	return nodo;
	
}
