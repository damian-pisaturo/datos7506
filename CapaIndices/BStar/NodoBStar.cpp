#include "NodoBStar.h"

NodoBStar::NodoBStar(unsigned int refNodo, unsigned char nivel, unsigned short tamanio)
		 : Nodo(refNodo, nivel, tamanio, true) {
	
}


NodoBStar::NodoBStar(unsigned int refNodo, unsigned char nivel, Clave* clave, unsigned short tamanio)
		  : Nodo(refNodo, nivel, clave, tamanio, true) {
	
}


unsigned short NodoBStar::getTamanioEnDisco() const {
	
	//El tamanio se inicializa con los bytes ocupados por el atributo de
	//espacio libre y por el de nivel.
	unsigned short tamanio = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_NIVEL;

	tamanio += this->getTamanioEnDiscoSetClaves();
	
	if (this->getNivel() != 0) tamanio += Tamanios::TAMANIO_REFERENCIA;
	
	return tamanio;
	
}


unsigned short NodoBStar::getTamanioEspacioClaves() const {
			
	if (this->getNivel() == 0) //Nodo hoja ==> Tengo que sumarle el espacio que se le descuenta por la referencia al hijo izq
		return (this->getTamanio() - Nodo::getTamanioHeader() + Tamanios::TAMANIO_REFERENCIA);
	else return (this->getTamanio() - Nodo::getTamanioHeader());
	
}


unsigned short NodoBStar::getTamanioMinimo() const {
	
	return (2*(this->getTamanioEspacioClaves())/3);
	
}


VectorConjuntos* NodoBStar::split(){
	VectorConjuntos* vecConj = this->getClaves()->splitBStar(this->getTamanioMinimo(), this->getTamanioEspacioClaves());
	this->actualizarEspacioLibre();
	return vecConj;
}


Nodo* NodoBStar::copiar() const {
	
	NodoBStar* nodoCopia = new NodoBStar(this->getHijoIzq(), this->getNivel(), this->getTamanio());
	SetClaves* setClavesCopia = new SetClaves();
	*setClavesCopia = *(this->getClaves());
	nodoCopia->setClaves(setClavesCopia);
	nodoCopia->setPosicionEnArchivo(this->getPosicionEnArchivo());
	nodoCopia->setEspacioLibre(this->getEspacioLibre());
	return nodoCopia;
	
}

