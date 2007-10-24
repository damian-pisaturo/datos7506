#include "BStarTree.h"

BStarTree::BStarTree() {
	
}

BStarTree::~BStarTree() {
	
}


void BStarTree::insertar(Clave* clave) {
	
	
	
}
	

void BStarTree::eliminar(Clave* clave) {
	
	
	
}


void BStarTree::insertarInterno(Clave* clave) {
	
	
	
}


void BStarTree::eliminarInterno(Clave* clave) {
	
	
	
}


bool BStarTree::puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
										 NodoBStar* &nodoHnoDer) const {
	
	nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
	
	unsigned bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned bytesPropuestos = 0;
	
	if ( (bytesPropuestos = nodoPadre->puedeCeder(bytesRequeridos)) > 0 ) {
		
		if ( nodoDestino->puedeRecibir(bytesPropuestos) ) {
			
			//Se busca dentro del nodo padre la clave cuyo hijo derecho es nodoDestino
			SetClaves::iterator iterPadre;
			for(iterPadre = nodoPadre->getClaves()->begin();
				(iterPadre != nodoPadre->getClaves()->end()) &&
				((*iterPadre)->getHijoDer() != nodoDestino->getPosicionEnArchivo());
				++iterPadre);
			
			//Se verifica si nodoDestino tiene hermano derecho
			if ( (++iterPadre) == nodoPadre->getClaves()->end() ) return false;
			
			//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
			//nodoHnoDer = new NodoBStar( (*iterPadre)->getHijoDer() );
			
			if ( (bytesPropuestos = nodoHnoDer->puedeCeder(bytesPropuestos)) > 0 ) {
				
				if ( nodoPadre->puedeRecibir(bytesPropuestos) ) {
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


bool BStarTree::puedePasarseClaveIzquierda(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
										   NodoBStar* &nodoHnoIzq) const {
	
	return false;
	
}


void BStarTree::pasarClaveIzquierda() {
	
	
	
}


NodoBStar* BStarTree::buscarPadre(NodoBStar* padre, NodoBStar* hijo) const {
	
	NodoBStar *auxNodo = NULL, *nuevoNodo = NULL;
	Clave* claveOrientadora = NULL;
	
	//TODO Solucionar el tema de que cuando retorna la funcion, el puntero queda apuntando a basura
	
	if (padre->esPadre(hijo, claveOrientadora)) return padre;
	
    else {
    	if (claveOrientadora == NULL) {
    		//nuevoNodo = new NodoBStar(archivo, padre->getHijoizq());
    		auxNodo = buscarPadre(nuevoNodo, hijo);
    	} else {
    		//nuevoNodo = new NodoBStar(archivo, claveOrientadora->getHijoDer());
    		auxNodo = buscarPadre(nuevoNodo, hijo);
    	}
    	
		return auxNodo;
    }
	
}


NodoBStar* BStarTree::buscarLugar(Clave* clave) const {
	
	//Se supone que el nodo raíz ya se encuentra cargado en memoria.
	
	return buscarLugarRecursivo(this->nodoRaiz, clave);
	
}


NodoBStar* BStarTree::buscarLugarRecursivo(NodoBStar* nodo, Clave* clave) const {
	
	Clave* claveResultante = nodo->buscar(clave);
	NodoBStar* nuevoNodo = NULL;
	
	//TODO Solucionar el tema de que cuando retorna la funcion, el puntero queda apuntando a basura
	
	if (claveResultante == NULL) {
		if (nodo->getHijoIzq() == 0) //Nodo hoja
			return nodo;
		else {
			//nuevoNodo = new NodoBStar(archivo, nodo->getHijoIzq());
			return buscarLugarRecursivo(nuevoNodo, clave);
		}
	} else {
		if (claveResultante->getHijoDer() == 0) //Nodo hoja
			return nodo;
		else {
			//nuevoNodo = new NodoBStar(archivo, claveResultante->getHijoDer());
			return buscarLugarRecursivo(nuevoNodo, clave);
		}
	}

}


NodoBStar* BStarTree::buscarMenorMayores(NodoBStar* nodo) const {
	
	/*
	int result;
    tN_ABd auxnodo;

    AB_MoverCte(tree,IZQ,&result);
    if(result==0){
    	Buscar_menor_mayores(tree,narbol);
        AB_MoverCte(tree,PAD,&result); //no tira error
    }else{
    	AB_ElemCte(*tree,&auxnodo);
        AB_ModifCte(tree,narbol);
    	*narbol=auxnodo;
        AB_MoverCte(tree,DER,&result);
        //si tiene hijo derecho
        if(result==0){
        	AB_MoverCte(tree,PAD,&result); //no tira error
            Eliminar(tree);
        }else Post_Orden(tree);
    }
    */
    
	return NULL;
	
}
		

NodoBStar* BStarTree::buscarMayorMenores(NodoBStar* nodo) const {
	
	/*
	int result;
    tN_ABd auxnodo;

    AB_MoverCte(tree,DER,&result);
    if(result==0){
    	Buscar_mayor_menores(tree,narbol);
        AB_MoverCte(tree,PAD,&result); //no tira error
    }else{
    	AB_ElemCte(*tree,&auxnodo);
        AB_ModifCte(tree,narbol);
    	*narbol=auxnodo;
        AB_MoverCte(tree,IZQ,&result);
        //si tiene hijo izquierdo
        if(result==0){
        	AB_MoverCte(tree,PAD,&result); //no tira error
            Eliminar(tree);
        }else Post_Orden(tree);
    }
    */
	
	return NULL;
	
}

