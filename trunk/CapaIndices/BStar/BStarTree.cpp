#include "BStarTree.h"

BStarTree::BStarTree(unsigned short tamanioNodo) {
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
	
	insertarInterno(nodoDestino, &codigo);
	
	delete nodoDestino;

}


void BStarTree::insertarInterno(NodoBStar* &nodoDestino, char* codigo) {
	
	if (*codigo == Codigo::MODIFICADO){
		//TODO actualizar en disco el nodo modificado.
		//archivoIndice->sobreescribirNodo(nodoDestino);
	}
	else if ( *codigo == Codigo::OVERFLOW ){
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBStar* nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
		NodoBStar *nodoHnoDer = NULL, *nodoHnoIzq = NULL;
		
		if (!nodoPadre){ //nodoDestino es la raiz.
			SetClaves* setClavesDerecho = nodoDestino->splitB(nodoDestino->getTamanioMinimo());
			nodoDestino->setTamanio(this->tamanioNodo);
			Clave* clavePromocionada = *(setClavesDerecho->begin());
			setClavesDerecho->erase(clavePromocionada);
			NodoBStar* nuevoNodoDerecho = new NodoBStar(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), nodoDestino->getTamanio());
			nuevoNodoDerecho->setClaves(setClavesDerecho);
			clavePromocionada->setHijoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			NodoBStar* nuevaRaiz = new NodoBStar(nodoDestino->getPosicionEnArchivo(), nodoDestino->getNivel() + 1, this->tamanioRaiz);
			//TODO escritura especial de la raiz
			//TODO escribir 'nuevoNodoDerecho'
			delete nuevoNodoDerecho;
			delete nuevaRaiz;
			*codigo = Codigo::MODIFICADO;
		}
		else{
			
			//Se buscan los hermanos derecho e izquierdo de 'nodoDestino'
			
			//Se busca dentro del nodo padre la clave cuyo hijo derecho es nodoDestino
			SetClaves::iterator iterPadre;
			for(iterPadre = nodoPadre->getClaves()->begin();
				(iterPadre != nodoPadre->getClaves()->end()) &&
				((*iterPadre)->getHijoDer() != nodoDestino->getPosicionEnArchivo());
				++iterPadre);
			
			//Se verifica si nodoDestino tiene hermano derecho y hermano izquierdo
			if ( iterPadre == nodoPadre->getClaves()->end() ) {
				//nodoDestino es el hijo izquierdo de nodoPadre
				//nodoDestino no tiene hermano izquierdo;
				iterPadre = nodoPadre->getClaves()->begin();
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoDer = new NodoBStar( (*iterPadre)->getHijoDer() );
				clavePadreDer = *iterPadre;
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoDestino es el hijo derecho de la última clave del nodo
				//nodoDestino no tiene hermano derecho
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoIzq = new NodoBStar( (*(--(--iterPadre)))->getHijoDer() );
				clavePadreIzq = *(++iterPadre);
			} else {
				if ( iterPadre == nodoPadre->getClaves()->begin() ) {
					//nodoHnoIzq = new NodoBStar( nodoPadre->getHijoIzq() );
					clavePadreIzq = *iterPadre;
				}
				else{
					//nodoHnoIzq = new NodoBStar( (*(--iterPadre))->getHijoDer() );
					clavePadreIzq = *(++iterPadre);
				}
				//nodoHnoDer = new NodoBStar( (*iterPadre)->getHijoDer() );
				clavePadreDer = *iterPadre;
			}
			
			//Fin de la búsqueda de los nodos hermanos de 'nodoDestino'
			
			//Se intenta hacer una redistribución de claves con el hermano derecho de 'nodoDestino'.
			//Si esto no es posible, se intenta hacer una redistribución con el hermano izquierdo.
			if ( (nodoHnoDer) && (nodoHnoDer->puedeRecibirClaveDesdeIzq(nodoDestino, nodoPadre)) ) 
				this->recibirClaveDesdeIzquierda(nodoHnoDer, nodoPadre, nodoDestino);
			else if ( (nodoHnoIzq) && (nodoHnoIzq->puedeRecibirClaveDesdeDer(nodoDestino, nodoPadre)) )
				this->recibirClaveDesdeDerecha(nodoHnoIzq, nodoPadre, nodoDestino);
			else { //Se realiza el split del nodo con overflow con un sibling completo y
				//con la clave padre.
				VectorClaves* vectorClaves;
				if (nodoHnoDer) {
					vectorClaves = mergeSplitOverflow(nodoHnoDer, nodoDestino, clavePadreDer);
					nodoPadre->extraerClave(clavePadreDer);
				} else {
					vectorClaves = mergeSplitOverflow(nodoDestino, nodoHnoIzq, clavePadreIzq);
					nodoPadre->extraerClave(clavePadreIzq);
				}
				
				nodoPadre->insertarClave((*vectorClaves)[0], codigo);
				nodoPadre->insertarClave((*vectorClaves)[1], codigo);
				
	
				if (!nodoPadre->tieneOverflow()) *codigo = Codigo::MODIFICADO;
				
				if (nodoHnoDer) delete nodoHnoDer;
				if (nodoHnoIzq) delete nodoHnoIzq;
				
				insertarInterno(nodoPadre, codigo);
			}
		}
		
		delete nodoPadre;
	}
	
	
}


bool BStarTree::eliminar(Clave* clave) {
	
	NodoBStar* nodoTarget = buscarLugar(clave);
	Clave* claveBuscada = nodoTarget->buscar(clave);
	
	char codigo;
	
	if ( (claveBuscada) && (*claveBuscada == *clave) ){
		if (nodoTarget->getNivel() != 0){ //Nodo interno
			NodoBStar* nodoMenorDeLosMayores = this->buscarMenorMayores(nodoTarget, clave);
			Clave* claveMenorDeLasMayores = nodoMenorDeLosMayores->obtenerPrimeraClave()->copiar();
			claveMenorDeLasMayores->setHijoDer(claveBuscada->getHijoDer());
			nodoTarget->eliminarClave(claveBuscada, &codigo);
			nodoTarget->insertarClave(claveMenorDeLasMayores, &codigo);
			//TODO Actualizar 'nodoTarget' en disco
			nodoMenorDeLosMayores->eliminarClave(claveMenorDeLasMayores, &codigo);
			this->eliminarInterno(nodoMenorDeLosMayores, &codigo); //resuelve underflow
			delete nodoMenorDeLosMayores;
		}
		else { //NodoTarget es un nodo hoja
			nodoTarget->eliminarClave(claveBuscada, &codigo);
			this->eliminarInterno(nodoTarget, &codigo); //resuelve underflow
		}
	}
	else{
		delete nodoTarget;
		return false;
	}
	
	delete nodoTarget;
	
	return true;
}


void BStarTree::eliminarInterno(NodoBStar* nodoTarget, char* codigo) {

	if (*codigo == Codigo::MODIFICADO) {
		//TODO actualizar en disco el nodo modificado.
		//archivoIndice->sobreescribirNodo(nodoTarget);
	} else if (*codigo == Codigo::UNDERFLOW) {
		
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBStar *nodoPadre = this->buscarPadre(this->nodoRaiz, nodoTarget);
		NodoBStar *nodoHnoDer = NULL, *nodoHnoIzq = NULL, *nodoHnoHno = NULL;
		//'nodoHnoHno' se utiliza para apuntar al hno siguiente al hermano derecho de 'nodoTarget'
		//o para apuntar al hermano anterior al hermano izquierdo de 'nodoTarget'
		
		if (!nodoPadre) //nodoTarget es el nodo raíz
			*codigo = Codigo::MODIFICADO;
		else{
		
			//Se buscan los hermanos derecho e izquierdo de 'nodoTarget'
			
			//Se busca dentro del nodo padre la clave cuyo hijo derecho es nodoTarget
			SetClaves::iterator iterPadre;
			for(iterPadre = nodoPadre->getClaves()->begin();
				(iterPadre != nodoPadre->getClaves()->end()) &&
				((*iterPadre)->getHijoDer() != nodoTarget->getPosicionEnArchivo());
				++iterPadre);
			
			//Se verifica si nodoTarget tiene hermano derecho y hermano izquierdo
			if ( iterPadre == nodoPadre->getClaves()->end() ) {
				//nodoTarget es el hijo izquierdo de nodoPadre
				//nodoTarget no tiene hermano izquierdo;
				//Se tendrían que cargar el hno derecho y el hno siguiente al hno derecho
				iterPadre = nodoPadre->getClaves()->begin();
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoDer = new NodoBStar( (*iterPadre)->getHijoDer() );
				clavePadreDer = *iterPadre;
				if ((++iterPadre) != nodoPadre->getClaves()->end()) {
					//nodoHnoHno = new NodoBStar( (*iterPadre)->getHijoDer() );
					clavePadreIzq = *iterPadre;
				}
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoTarget es el hijo derecho de la última clave del nodo
				//nodoTarget no tiene hermano derecho
				if ( (--iterPadre) == nodoPadre->getClaves()->begin() ) {
					//nodoHnoIzq = new NodoBStar( nodoPadre->getHijoIzq() );
					clavePadreIzq = *iterPadre;
				}
				else{
					//nodoHnoIzq = new NodoBStar( (*(--iterPadre))->getHijoDer() );
					clavePadreIzq = *(++iterPadre);
					if ((--iterPadre) == nodoPadre->getClaves()->begin()) {
						//nodoHnoHno = new NodoBStar( nodoPadre->getHijoIzq() );
						clavePadreDer = *iterPadre;
					} else {
						//nodoHnoHno = new NodoBStar( (*(--iterPadre))->getHijoDer() );
						clavePadreDer = *(++iterPadre);
					}
				}	
			} else {
				--iterPadre; //Lo decremento xq se incrementó en el 'else if' anterior
				if ( iterPadre == nodoPadre->getClaves()->begin() ) {
					//nodoHnoIzq = new NodoBStar( nodoPadre->getHijoIzq() );
					clavePadreIzq = *iterPadre;
				}
				else{
					//nodoHnoIzq = new NodoBStar( (*(--iterPadre))->getHijoDer() );
					clavePadreIzq = *(++iterPadre);
				}
				//nodoHnoDer = new NodoBStar( (*(++iterPadre))->getHijoDer() );
				clavePadreDer = *iterPadre;
			}
			
			//Fin de la búsqueda de los nodos hermanos de 'nodoTarget'
			
			//Se intenta hacer una redistribución de claves con el hermano derecho de 'nodoTarget'.
			//Si esto no es posible, se intenta hacer una redistribución con el hermano izquierdo.
			//Si se tratara de un extremo se mira el hermano del hermano.
			bool pudoRedistribuir = false;
			if (nodoHnoDer) {
				if ( pudoRedistribuir = nodoHnoDer->puedePasarClaveHaciaIzq(nodoTarget, nodoPadre))
					this->pasarClaveHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer);
				else if (!nodoHnoIzq) {
					//'nodoHnoHno' debe cederle a 'nodoHnoDer' y luego 'nodoHnoDer' debe cederle a 'nodoTarget'
					if ( (nodoHnoHno) && (nodoHnoHno->puedePasarClaveHaciaIzq(nodoHnoDer, nodoPadre)) ) {
						this->pasarClaveHaciaIzquierda(nodoHnoDer, nodoPadre, nodoHnoHno);
						if ( pudoRedistribuir = nodoHnoDer->puedePasarClaveHaciaIzq(nodoTarget, nodoPadre))
							this->pasarClaveHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer);
					}
				} else {
					if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre))
						this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq);
				}
			} else { // tiene hermano izquierdo
				if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre))
					this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq);
				else { //'nodoTarget' no tiene hermano derecho
					//'nodoHnoHno' debe cederle a 'nodoHnoIzq' y luego 'nodoHnoIzq' debe cederle a 'nodoTarget'
					if ( (nodoHnoHno) && (nodoHnoHno->puedePasarClaveHaciaDer(nodoHnoIzq, nodoPadre)) ) {
						this->pasarClaveHaciaDerecha(nodoHnoIzq, nodoPadre, nodoHnoHno);
						if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre))
							this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq);
					}
				}
			}
			
			if (!pudoRedistribuir) { //Se realiza la concatenación de 'nodoTarget' con dos nodos hermanos y dos claves
				//del nodo padre. Luego se realiza un split, generándose dos nodos y una clave a promocionar
				//hacia el nodo padre.
				
				Clave* clavePromocionada = NULL;
				
				if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoHnoDer)&&(!nodoHnoIzq)&&(!nodoHnoHno)) this->merge(nodoTarget, nodoHnoDer, this->nodoRaiz);
				else if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoHnoIzq)&&(!nodoHnoDer)&&(!nodoHnoHno))this->merge(nodoTarget, nodoHnoIzq, this->nodoRaiz);
				else {
					if ((nodoHnoIzq) && (nodoHnoDer))
						clavePromocionada = this->mergeSplitUnderflow(nodoTarget, nodoHnoIzq, nodoHnoDer, clavePadreIzq, clavePadreDer);
					else if (!(nodoHnoIzq))
						if (nodoHnoHno)
							clavePromocionada = this->mergeSplitUnderflow(nodoTarget, nodoHnoDer, nodoHnoHno, clavePadreDer, clavePadreIzq);
					else if (nodoHnoHno)
						clavePromocionada = this->mergeSplitUnderflow(nodoTarget, nodoHnoIzq, nodoHnoHno, clavePadreIzq, clavePadreDer);
					
					nodoPadre->extraerClave(clavePadreIzq);
					nodoPadre->extraerClave(clavePadreDer);
					nodoPadre->insertarClave(clavePromocionada, codigo);
					
					this->insertarInterno(nodoPadre, codigo); //Se resuelve posible overflow al insertar la clave promocionada.
					
				}		
			}
			
			if (nodoHnoDer) delete nodoHnoDer;
			if (nodoHnoIzq) delete nodoHnoIzq;
			if (nodoHnoHno) delete nodoHnoHno;
	
		}
		
		delete nodoPadre;
	}
	
}


Clave* BStarTree::buscar(Clave* clave) const {
	
	NodoBStar* nodo = this->buscarLugar(clave);
	
	SetClaves::iterator iter = nodo->getClaves()->find(clave);
	
	if (iter != nodo->getClaves()->end())
		return *iter;
	
	return NULL;
	
}

		
bool BStarTree::modificar(Clave* claveVieja, Clave* claveNueva) {
	
	if ( eliminar(claveVieja) ) insertar(claveNueva);
	else return false;
	
	return true;
}


bool BStarTree::puedePasarseClaveDerecha(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
										 NodoBStar* &nodoHnoDer) const {
	
	nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
	
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPadre = 0, bytesPropuestosHnoDer = 0;
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

void BStarTree::pasarClaveHaciaIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer){
	
	SetClaves* setDescenso = nodoPadre->cederBytes( nodoDestino->obtenerBytesRequeridos() );
	unsigned short cantClaves = setDescenso->size();
	SetClaves* setAscenso = nodoHnoDer->cederBytes(cantClaves);
	
	for(SetClaves::iterator iterDescenso = setDescenso->begin(), iterAscenso = setAscenso->begin();
		(iterDescenso != setDescenso->end()) && (iterAscenso != setAscenso->end());
		++iterDescenso, ++iterAscenso ){
		(*iterAscenso)->setHijoDer((*iterDescenso)->getHijoDer());
		(*iterDescenso)->setHijoDer(0);
	}
	
	nodoDestino->recibir(setDescenso);
	nodoPadre->recibir(setAscenso);
	
	delete setDescenso;
	delete setAscenso; 
	
}


void BStarTree::recibirClaveDesdeDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer){
	
	unsigned short cantClaves = 0;
	unsigned short bytesSobrantes = nodoHnoDer->obtenerBytesSobrantes(cantClaves);
	
	SetClaves* setDescenso = nodoPadre->cederClaves(cantClaves);
	SetClaves* setAscenso = nodoHnoDer->cederBytes(bytesSobrantes);
	
	for(SetClaves::iterator iterDescenso = setDescenso->begin(), iterAscenso = setAscenso->begin();
		(iterDescenso != setDescenso->end()) && (iterAscenso != setAscenso->end());
		++iterDescenso, ++iterAscenso ){
		(*iterAscenso)->setHijoDer((*iterDescenso)->getHijoDer());
		(*iterDescenso)->setHijoDer(0);
	}
	
	nodoDestino->recibir(setDescenso);
	nodoPadre->recibir(setAscenso);
	
	delete setDescenso;
	delete setAscenso;
	
}


bool BStarTree::puedePasarseClaveIzquierda(NodoBStar* nodoDestino, NodoBStar* &nodoPadre,
										   NodoBStar* &nodoHnoIzq) const {
	nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
		
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPadre = 0, bytesPropuestosHnoIzq = 0;
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


void BStarTree::pasarClaveHaciaDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq) {
	
	SetClaves* setDescenso = nodoPadre->cederBytes( nodoDestino->obtenerBytesRequeridos(), false );
	unsigned short cantClaves = setDescenso->size();
	SetClaves* setAscenso = nodoHnoIzq->cederClaves(cantClaves, false);
	
	for(SetClaves::iterator iterDescenso = setDescenso->begin(), iterAscenso = setAscenso->begin();
		(iterDescenso != setDescenso->end()) && (iterAscenso != setAscenso->end());
		++iterDescenso, ++iterAscenso ){
		(*iterAscenso)->setHijoDer((*iterDescenso)->getHijoDer());
		(*iterDescenso)->setHijoDer(0);
	}
	
	nodoDestino->recibir(setDescenso);
	nodoPadre->recibir(setAscenso);
	
	delete setDescenso;
	delete setAscenso;
	
}


void BStarTree::recibirClaveDesdeIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq){
	
	unsigned short cantClaves = 0;
	unsigned short bytesSobrantes = nodoHnoIzq->obtenerBytesSobrantes(cantClaves);
	
	SetClaves* setDescenso = nodoPadre->cederClaves(cantClaves);
	SetClaves* setAscenso = nodoHnoIzq->cederBytes(bytesSobrantes);
	
	for(SetClaves::iterator iterDescenso = setDescenso->begin(), iterAscenso = setAscenso->begin();
		(iterDescenso != setDescenso->end()) && (iterAscenso != setAscenso->end());
		++iterDescenso, ++iterAscenso ){
		(*iterAscenso)->setHijoDer((*iterDescenso)->getHijoDer());
		(*iterDescenso)->setHijoDer(0);
	}
	
	nodoDestino->recibir(setDescenso);
	nodoPadre->recibir(setAscenso);
	
	delete setDescenso;
	delete setAscenso;
	
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

VectorClaves* BStarTree::mergeSplitOverflow(NodoBStar* nodoTarget, NodoBStar* nodoHno, Clave* clavePadre){
	
	nodoTarget->merge(nodoHno, clavePadre);
	
	VectorConjuntos* vectorConjuntos = nodoTarget->split();
	
	NodoBStar* nuevoNodo = new NodoBStar(nodoHno->getHijoIzq(), nodoHno->getNivel(), nodoHno->getTamanio());
	
	nuevoNodo->setClaves((*vectorConjuntos)[0]);
	nodoHno->setClaves((*vectorConjuntos)[1]);
	
	delete vectorConjuntos;
	
	Clave* ultimaClaveNuevoNodo = nuevoNodo->obtenerUltimaClave(); 
	Clave* ultimaClaveNodoHno = nodoHno->obtenerUltimaClave();
	
	nuevoNodo->extraerUltimaClave();
	nodoHno->extraerUltimaClave();
	
	nodoHno->setHijoIzq(ultimaClaveNuevoNodo->getHijoDer());
	nodoTarget->setHijoIzq(ultimaClaveNodoHno->getHijoDer());

	ultimaClaveNuevoNodo->setHijoDer(nodoHno->getPosicionEnArchivo());
	ultimaClaveNodoHno->setHijoDer(nodoTarget->getPosicionEnArchivo());
	
	VectorClaves* vectorClaves = new VectorClaves(2);
	
	vectorClaves->push_back(ultimaClaveNuevoNodo);
	vectorClaves->push_back(ultimaClaveNodoHno);
	
	//TODO escribir nuevoNodo en disco y actualizar el nodoHno y nodoTarget
	
	delete nuevoNodo;
	
	return vectorClaves;
}

//Por el momento se utiliza cuando se tiene una raiz con dos hijos y se mergea todo en una unica raiz.
//En un futuro podria llegar a implementarse para nodos internos.
void BStarTree::merge(NodoBStar* nodoHijoIzq, NodoBStar* nodoHijoDer, NodoBStar* nodoPadre) {
	
	nodoPadre->setHijoIzq(0);
	nodoPadre->obtenerPrimeraClave()->setHijoDer(0);
	nodoPadre->setNivel(0);
	
	nodoPadre->merge(nodoHijoIzq, nodoHijoDer, NULL, NULL);

	//TODO eliminar de disco a los nodos nodoHijoDer y nodoHijoIzq
	delete nodoHijoIzq;
	delete nodoHijoDer;
	
}


Clave* BStarTree::mergeSplitUnderflow(NodoBStar* nodoTarget, NodoBStar* nodoHno1, NodoBStar* nodoHno2,
						Clave* clavePadre1, Clave* clavePadre2) {
	
	nodoTarget->merge(nodoHno1, nodoHno2, clavePadre1, clavePadre2);
	SetClaves* setClavesMayores = nodoTarget->splitB( nodoTarget->getTamanioMinimo() );
	Clave* clavePromocionada = *(setClavesMayores->begin());
	setClavesMayores->erase(setClavesMayores->begin()); 
	
	clavePadre1->setHijoDer(0);
	clavePadre2->setHijoDer(0);
	
	clavePromocionada->setHijoDer(nodoHno1->getPosicionEnArchivo());
	
	//TODO eliminar de disco el nodo nodoHno2
	delete nodoHno2;
	
	return clavePromocionada;
}
