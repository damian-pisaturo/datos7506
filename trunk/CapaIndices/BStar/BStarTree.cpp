#include "BStarTree.h"

BStarTree::BStarTree() {
	
}

BStarTree::~BStarTree() {
	
}

bool BStarTree::puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre, NodoBStar* &nodoHnoDer) {
	
	NodoBStar* nodoPadre = this->buscarPadre(nodoDestino);
	
	unsigned bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned bytesPropuestos = 0;
	
	if ( (bytesPropuestos = nodoPadre->puedeCeder(bytesRequeridos)) > 0 ){
		if ( nodoDestino->puedeRecibir(bytesPropuestos) ){
			SetClaves::iterator iterPadre = nodoPadre->getClaves()->begin();
			for(iter; (iter != nodoPadre->getClaves()->end()) && ((*iter)->getHijoDer() != nodoDestino->getPosicionEnArchivo()); ++iter);
			if ( (++iter) == nodoPadre->getClaves()->end() ) return false;
			//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
			NodoBStar* nodoHnoDer = new NodoBStar( (*iter)->getHijoDer() );
			
			if ( (bytesPropuestos = nodoHnoDer->puedeCeder(bytesPropuestos)) > 0 ){
				if ( nodoPadre->puedeRecibir(bytesPropuestos) ){
					return true;
				}
			}
		}
	}
	
	return false;
}

void pasarClaveDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer){
	
	SetClaves* set = nodoPadre->ceder( nodoDestino->obtenerBytesRequeridos(), true );
	nodoDestino->recibir(set);
	delete set;
	
	set = nodoHnoDer->ceder( nodoPadre->obtenerBytesRequeridos(), true );
	nodoPadre->recibir(set);
	delete set;
	
}