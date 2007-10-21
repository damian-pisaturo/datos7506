#include "NodoBStar.h"

NodoBStar::NodoBStar(int hijoIzq, int nivel):Nodo(hijoIzq, nivel) {	
	
}

NodoBStar::NodoBStar(int hijoIzq,int nivel,Clave* clave):Nodo(hijoIzq, nivel, clave) {
	
}

NodoBStar::NodoBStar(int referencia):Nodo(referencia){
	
}

NodoBStar::~NodoBStar() {
	
}

char NodoBStar::insertarClave(Clave* &clave) {
	return 0;
}

char NodoBStar::eliminarClave(Clave* clave){
	return 0;
}

