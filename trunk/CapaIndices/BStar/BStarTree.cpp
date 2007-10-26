#include "BStarTree.h"

BStarTree::BStarTree(unsigned tamanioNodo) {
	this->tamanioNodo = tamanioNodo;
	this->tamanioRaiz = 4*(tamanioNodo - NodoBStar::getTamanioHeader())/3;
	this->tamanioRaiz += NodoBStar::getTamanioHeader();
	this->nodoRaiz = NULL;
}

BStarTree::~BStarTree() {
	if (this->nodoRaiz) delete nodoRaiz;
}


void BStarTree::insertar(Clave* clave) {
	
	NodoBStar* nodoDestino = buscarLugar(clave);
	char codigo;
	
	nodoDestino->insertarClave(clave, &codigo);
	
	//La clave queda insertada independientemente de si hay OVERFLOW o no.
	if ( codigo == Codigo::OVERFLOW ){
		NodoBStar* nodoPadre;
		NodoBStar* nodoHnoDer;
		NodoBStar* nodoHnoIzq;
		if ( puedePasarseClaveIzquierda(nodoHnoDer, nodoPadre, nodoDestino) )
			pasarClaveDerecha(nodoHnoDer, nodoPadre, nodoDestino);
		else if ( puedePasarseClaveDerecha(nodoHnoIzq, nodoPadre, nodoDestino) )
			pasarClaveIzquierda(nodoHnoIzq, nodoPadre, nodoDestino);
		else //TODO No siempre tenemos hermano derecho. Ver que hacemos.
			split( nodoDestino, nodoHnoDer, nodoPadre );
		
	}
		
	
	
}
	

void BStarTree::eliminar(Clave* clave) {
	
	
	
}


void BStarTree::insertarInterno(Clave* clave) {
	
	
	
}


void BStarTree::eliminarInterno(Clave* clave) {
	
	
	
}


Clave* BStarTree::buscar(Clave* clave) const {
	
	NodoBStar* nodo = this->buscarLugar(clave);
	
	SetClaves::iterator iter = nodo->getClaves()->find(clave);
	
	if (iter != nodo->getClaves()->end())
		return *iter;
	
	return NULL;
	
}

		
void BStarTree::modificar(Clave* clave) {
	
	
	
}

//TODO Agregar el caso en que se trata de un extremo
bool BStarTree::puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
										 NodoBStar* &nodoHnoDer) const {
	
	nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
	
	unsigned bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned bytesPropuestosPadre = 0, bytesPropuestosHnoDer = 0;
	unsigned char clavesPropuestas = 0;
	
	
	if ( (bytesPropuestosPadre = nodoPadre->bytesACeder(bytesRequeridos, clavesPropuestas)) > 0 ) {
		
		if ( nodoDestino->puedeRecibir(bytesPropuestosPadre, 0) ) {
			
			//Se busca dentro del nodo padre la clave cuyo hijo derecho es nodoDestino
			SetClaves::iterator iterPadre;
			for(iterPadre = nodoPadre->getClaves()->begin();
				(iterPadre != nodoPadre->getClaves()->end()) &&
				((*iterPadre)->getHijoDer() != nodoDestino->getPosicionEnArchivo());
				++iterPadre);
			
			//Se verifica si nodoDestino tiene hermano derecho
			if ( iterPadre == nodoPadre->getClaves()->end() )
				iterPadre = nodoPadre->getClaves()->begin();
			else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) return false;
			
			
			//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
			//nodoHnoDer = new NodoBStar( (*iterPadre)->getHijoDer() );
			
			if ( (bytesPropuestosHnoDer = nodoHnoDer->bytesACeder(clavesPropuestas)) > 0 ) {
				
				if ( nodoPadre->puedeRecibir(bytesPropuestosHnoDer, bytesPropuestosPadre) ) {
					return true;
				}
				
			}
			
		}
		
	}
	
	return false;
}

void BStarTree::pasarClaveDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer){
	
	SetClaves* set = nodoPadre->ceder( nodoDestino->obtenerBytesRequeridos() );
	nodoDestino->recibir(set);
	delete set;
	
	set = nodoHnoDer->ceder( nodoPadre->obtenerBytesRequeridos() );
	nodoPadre->recibir(set);
	delete set;
	
}

//TODO Agregar el caso en que se trata de un extremo
bool BStarTree::puedePasarseClaveIzquierda(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
										   NodoBStar* &nodoHnoIzq) const {
	nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
		
	unsigned bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned bytesPropuestosPadre = 0, bytesPropuestosHnoIzq = 0;
	unsigned char clavesPropuestas = 0;
	
	if ( (bytesPropuestosPadre = nodoPadre->bytesACeder(bytesRequeridos, clavesPropuestas, false)) > 0 ) {
		
		if ( nodoDestino->puedeRecibir(bytesPropuestosPadre, 0) ) {
			
			//Se busca dentro del nodo padre la clave cuyo hijo derecho es nodoDestino
			SetClaves::iterator iterPadre;
			for(iterPadre = nodoPadre->getClaves()->begin();
				(iterPadre != nodoPadre->getClaves()->end()) &&
				((*iterPadre)->getHijoDer() != nodoDestino->getPosicionEnArchivo());
				++iterPadre);
			
			//Se verifica si nodoDestino tiene hermano izquierdo
			if ( iterPadre == nodoPadre->getClaves()->end() ) return false;
			else if ( iterPadre == nodoPadre->getClaves()->begin() ) {
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoIzq = new NodoBStar( nodoPadre->getHijoIzq );
			}
			else{
				//nodoHnoIzq = new NodoBStar( (*(--iterPadre))->getHijoDer() );
			}


			if ( (bytesPropuestosHnoIzq = nodoHnoIzq->bytesACeder(clavesPropuestas, false)) > 0 ) {
				
				if ( nodoPadre->puedeRecibir(bytesPropuestosHnoIzq, bytesPropuestosPadre) ) {
					return true;
				}
				
			}
			
		}
		
	}
	
	return false;
	
}


void BStarTree::pasarClaveIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq) {
	
	SetClaves* set = nodoPadre->ceder( nodoDestino->obtenerBytesRequeridos(), false );
	nodoDestino->recibir(set);
	delete set;
	
	set = nodoHnoIzq->ceder( nodoPadre->obtenerBytesRequeridos(), false );
	nodoPadre->recibir(set);
	delete set;
	
}


NodoBStar* BStarTree::buscarPadre(NodoBStar* padre, NodoBStar* hijo) const {
	
	NodoBStar *auxNodo = NULL, *nuevoNodo = NULL;
	Clave* claveOrientadora = NULL;

	if ( (!padre) || (!hijo) || (*hijo == *(this->nodoRaiz)) ) return NULL;
	
	if (padre->esPadre(hijo, claveOrientadora)) {
		
		//auxNodo = new NodoBStar(archivo, padre->getPosicionEnArchivo());
		
	} else {
		
    	if (claveOrientadora == NULL) {
    		//nuevoNodo = new NodoBStar(archivo, padre->getHijoizq());
    	} else {
    		//nuevoNodo = new NodoBStar(archivo, claveOrientadora->getHijoDer());
    	}
    	
    	auxNodo = buscarPadre(nuevoNodo, hijo);
    	
    	delete nuevoNodo;
    	
    }
	
	return auxNodo;
	
}


NodoBStar* BStarTree::buscarLugar(Clave* clave) const {
	
	if (!clave) return NULL;
	
	//Se supone que el nodo raíz ya se encuentra cargado en memoria.
	return buscarLugarRecursivo(this->nodoRaiz, clave);
	
}


NodoBStar* BStarTree::buscarLugarRecursivo(NodoBStar* nodo, Clave* clave) const {
	
	NodoBStar *nuevoNodo = NULL, *auxNodo = NULL;
	
	Clave* claveResultante = nodo->buscar(clave);
		
	if (claveResultante == NULL) {
		
		if (nodo->getHijoIzq() == 0) { //Nodo hoja
			//auxNodo = new NodoBStar(archivo, nodo->getPosicionEnArchivo());
		} else {
			//nuevoNodo = new NodoBStar(archivo, nodo->getHijoIzq());
			auxNodo = buscarLugarRecursivo(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	} else {
		
		if (claveResultante->getHijoDer() == 0) {//Nodo hoja
			//auxNodo = new NodoBStar(archivo, nodo->getPosicionEnArchivo());
		} else {
			//nuevoNodo = new NodoBStar(archivo, claveResultante->getHijoDer());
			auxNodo = buscarLugarRecursivo(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	}
	
	return auxNodo;

}

//Se supone que el nodo que recibe no es hoja.
NodoBStar* BStarTree::buscarMenorMayores(NodoBStar* nodo, Clave* clave) const {
	
	return this->buscarLugarRecursivo(nodo, clave);
	
}

void BStarTree::split(NodoBStar* nodoTarget, NodoBStar* nodoHnoDer, NodoBStar* nodoPadre){
	
}

