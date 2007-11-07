#include "BStarTree.h"


BStarTree::BStarTree(IndiceManager& indiceManager, unsigned short tamanioNodo)
	: BTree(indiceManager, tamanioNodo) {
	
	this->nodoRaiz = NULL;
	this->tamanioRaiz = 4*(tamanioNodo - NodoBStar::getTamanioHeader())/3;
	this->tamanioRaiz += NodoBStar::getTamanioHeader();
}

BStarTree::~BStarTree() {
	if (this->nodoRaiz) delete nodoRaiz;
}


void BStarTree::insertar(Clave* clave) {
	
	if (!clave) return;
	
	if (this->vacio()) {
		
		this->nodoRaiz = new NodoBStar(0, 0, clave, this->tamanioRaiz);
		
		indiceManager.escribirBloque(this->nodoRaiz);
		
	} else {
		
		NodoBStar* nodoDestino = buscarLugar(clave);
		
		char codigo;
		
		nodoDestino->insertarClave(clave, &codigo);
		
		//La clave queda insertada independientemente de si hay OVERFLOW o no.
		
		insertarInterno(nodoDestino, &codigo);
		
		delete nodoDestino;
		
	}

}


void BStarTree::insertarInterno(NodoBStar* &nodoDestino, char* codigo) {
	
	if (*codigo == Codigo::MODIFICADO){
		indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	}
	else if ( *codigo == Codigo::OVERFLOW ){
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBStar* nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
		NodoBStar *nodoHnoDer = NULL, *nodoHnoIzq = NULL;
		
		if (!nodoPadre){ //nodoDestino es la raiz.
			//La raíz no tiene tamaño mínimo, entonces a splitB le paso el tamaño mínimo de un nodo común.
			unsigned short tamanioMinimo = 2*(this->tamanioNodo - Nodo::getTamanioHeader())/3;
			SetClaves* setClavesDerecho = nodoDestino->splitB(tamanioMinimo);
			//Ahora nodoDestino deja de ser la raíz, por lo cual pasa a tener el tamaño de un nodo común.
			nodoDestino->setTamanio(this->tamanioNodo);
			Clave* clavePromocionada = *(setClavesDerecho->begin());
			setClavesDerecho->erase(clavePromocionada); //Extrae la clave del conj. No libera la memoria.
			NodoBStar* nuevoNodoDerecho = new NodoBStar(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), this->tamanioNodo);
			nuevoNodoDerecho->setClaves(setClavesDerecho);
			indiceManager.escribirBloque(nuevoNodoDerecho);
			clavePromocionada->setHijoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			NodoBStar* nuevaRaiz = new NodoBStar(nodoDestino->getPosicionEnArchivo(), nodoDestino->getNivel() + 1, this->tamanioRaiz);
			//TODO escritura especial de la raiz
			delete nuevoNodoDerecho;
			this->nodoRaiz = nuevaRaiz; //No se libera la memoria ocupada por el nodo raíz, ya que siempre
										//lo tenemos cargado en memoria.
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
				nodoHnoDer = new NodoBStar(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*iterPadre)->getHijoDer(), nodoHnoDer);
				clavePadreDer = *iterPadre;
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoDestino es el hijo derecho de la última clave del nodo
				//nodoDestino no tiene hermano derecho
				nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*(--(--iterPadre)))->getHijoDer(), nodoHnoIzq);
				clavePadreIzq = *(++iterPadre);
			} else {
				if ( iterPadre == nodoPadre->getClaves()->begin() ) {
					nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
					indiceManager.leerBloque(nodoPadre->getHijoIzq(), nodoHnoIzq);
					clavePadreIzq = *iterPadre;
				}
				else{
					nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
					indiceManager.leerBloque((*(--iterPadre))->getHijoDer(), nodoHnoIzq);
					clavePadreIzq = *(++iterPadre);
				}
				nodoHnoDer = new NodoBStar(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*iterPadre)->getHijoDer(), nodoHnoDer);
				clavePadreDer = *iterPadre;
			}
			
			//Fin de la búsqueda de los nodos hermanos de 'nodoDestino'
			
			//Se intenta hacer una redistribución de claves con el hermano derecho de 'nodoDestino'.
			//Si esto no es posible, se intenta hacer una redistribución con el hermano izquierdo.
			if ( (nodoHnoDer) && (nodoHnoDer->puedeRecibirClaveDesdeIzq(nodoDestino, nodoPadre, clavePadreDer)) ) 
				this->recibirClaveDesdeIzquierda(nodoHnoDer, nodoPadre, nodoDestino, clavePadreDer);
			else if ( (nodoHnoIzq) && (nodoHnoIzq->puedeRecibirClaveDesdeDer(nodoDestino, nodoPadre, clavePadreIzq)) )
				this->recibirClaveDesdeDerecha(nodoHnoIzq, nodoPadre, nodoDestino, clavePadreIzq);
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
				
				SetClaves::iterator iterClave = nodoPadre->getClaves()->find((*vectorClaves)[0]);
				
				//Se settea la referencia al nuevo nodo generado a causa del split 
				if (iterClave == nodoPadre->getClaves()->begin())
					nodoPadre->setHijoIzq((*vectorClaves)[2]->getHijoDer());
				else (*(--iterClave))->setHijoDer((*vectorClaves)[2]->getHijoDer());
				
				//Libero la memoria de la clave que se utilizaba para guardar la referencia al nuevo nodo
				delete (*vectorClaves)[2];
				
				if (nodoHnoDer) delete nodoHnoDer;
				if (nodoHnoIzq) delete nodoHnoIzq;
				
				//Llamada recursiva para chequear overflow en el padre
				insertarInterno(nodoPadre, codigo);
			}
			
			delete nodoPadre;
		}
		
	}
	
	
}


bool BStarTree::eliminar(Clave* clave) {
	
	if ( (!clave) || (this->vacio()) ) return false;
	
	NodoBStar* nodoTarget = buscarClave(this->nodoRaiz, clave);
	
	//Se verifica si se encontró el nodo que puede llegar a contener la clave
	if (!nodoTarget) return false;
	
	Clave* claveBuscada = nodoTarget->buscar(clave);
	
	char codigo;
	
	if ( (claveBuscada) && (*claveBuscada == *clave) ){
		if (nodoTarget->getNivel() != 0){ //Nodo interno
			NodoBStar* nodoMenorDeLosMayores = this->buscarMenorMayores(nodoTarget, clave);
			Clave* claveMenorDeLasMayores = nodoMenorDeLosMayores->obtenerPrimeraClave()->copiar();
			claveMenorDeLasMayores->setHijoDer(claveBuscada->getHijoDer());
			nodoTarget->eliminarClave(claveBuscada, &codigo);
			nodoTarget->insertarClave(claveMenorDeLasMayores, &codigo);
			//Se resuelve el overflow en nodoTarget (en caso que haya).
			//Este método también escribe el nodo en disco.
			this->insertarInterno(nodoTarget, &codigo);
			nodoMenorDeLosMayores->eliminarClave(claveMenorDeLasMayores, &codigo);
			//Se resuelve el underflow en nodoMenorDeLosMayores (en caso que haya).
			//Este método también escribe el nodo en disco.
			this->eliminarInterno(nodoMenorDeLosMayores, &codigo);
			delete nodoMenorDeLosMayores;
		}
		else { //NodoTarget es un nodo hoja
			nodoTarget->eliminarClave(claveBuscada, &codigo);
			this->eliminarInterno(nodoTarget, &codigo); //resuelve underflow y escribe en disco
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
		//Se actualizar en disco el nodo modificado.
		indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);
	} else if (*codigo == Codigo::UNDERFLOW) {
		
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBStar *nodoPadre = this->buscarPadre(this->nodoRaiz, nodoTarget);
		NodoBStar *nodoHnoDer = NULL, *nodoHnoIzq = NULL, *nodoHnoHno = NULL;
		
		//'nodoHnoHno' se utiliza para apuntar al hno siguiente al hermano derecho de 'nodoTarget'
		//o para apuntar al hermano anterior al hermano izquierdo de 'nodoTarget'
		//En caso que nodoTarget sea un nodo extremo, clavePadreDer se utilzará para apuntar a
		//la clave del nodo padre que se encuentra de separador entre nodoTarget y su hermano derecho,
		//y clavePadreIzq se utilizará para apuntar a la clave entre nodoHnoDer y su hermano derecho.
		//Si nodoTarget es un nodo extremo que sólo tiene hermanos izquierdos, clavePadreIzq se utilzará
		//para apuntar a la clave del nodo padre que se encuentra de separador entre nodoTarget y su 
		//hermano izquierdo, y clavePadreDer se utilizará para apuntar a la clave entre nodoHnoIzq y
		//su hermano izquierdo.
		
		if (!nodoPadre) {//nodoTarget es el nodo raíz, no se chequea underflow.
			*codigo = Codigo::MODIFICADO;
			if (nodoTarget->getCantidadClaves() == 0) {
				//Se "elimina" la raíz de disco.
				//La memoria utilizada por el nodo se libera al final del método eliminar
				indiceManager.eliminarBloque(nodoTarget->getPosicionEnArchivo());
				this->nodoRaiz = NULL;
			} else {
				//TODO Escritura especial de la raíz
			}
		} else{
		
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
				nodoHnoDer = new NodoBStar(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*iterPadre)->getHijoDer(), nodoHnoDer);
				clavePadreDer = *iterPadre;
				if ((++iterPadre) != nodoPadre->getClaves()->end()) {
					nodoHnoHno = new NodoBStar(0, 0, this->tamanioNodo);
					indiceManager.leerBloque((*iterPadre)->getHijoDer(), nodoHnoHno);
					clavePadreIzq = *iterPadre;
				}
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoTarget es el hijo derecho de la última clave del nodo
				//nodoTarget no tiene hermano derecho
				if ( (--iterPadre) == nodoPadre->getClaves()->begin() ) {
					nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
					indiceManager.leerBloque(nodoPadre->getHijoIzq(), nodoHnoIzq);
					clavePadreIzq = *iterPadre;
				}
				else{
					nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
					indiceManager.leerBloque((*(--iterPadre))->getHijoDer(), nodoHnoIzq);
					clavePadreIzq = *(++iterPadre);
					if ((--iterPadre) == nodoPadre->getClaves()->begin()) {
						nodoHnoHno = new NodoBStar(0, 0, this->tamanioNodo);
						indiceManager.leerBloque(nodoPadre->getHijoIzq(), nodoHnoHno);
						clavePadreDer = *iterPadre;
					} else {
						nodoHnoHno = new NodoBStar(0, 0, this->tamanioNodo);
						indiceManager.leerBloque((*(--iterPadre))->getHijoDer(), nodoHnoHno);
						clavePadreDer = *(++iterPadre);
					}
				}	
			} else {
				--iterPadre; //Lo decremento xq se incrementó en el 'else if' anterior
				if ( iterPadre == nodoPadre->getClaves()->begin() ) {
					nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
					indiceManager.leerBloque(nodoPadre->getHijoIzq(), nodoHnoIzq);
					clavePadreIzq = *iterPadre;
				}
				else{
					nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
					indiceManager.leerBloque((*(--iterPadre))->getHijoDer(), nodoHnoIzq);
					clavePadreIzq = *(++iterPadre);
				}
				nodoHnoDer = new NodoBStar(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*(++iterPadre))->getHijoDer(), nodoHnoDer);
				clavePadreDer = *iterPadre;
			}
			
			//Fin de la búsqueda de los nodos hermanos de 'nodoTarget'
			
			//Se intenta hacer una redistribución de claves con el hermano derecho de 'nodoTarget'.
			//Si esto no es posible, se intenta hacer una redistribución con el hermano izquierdo.
			//Si se tratara de un extremo se mira el hermano del hermano.
			bool pudoRedistribuir = false;
			if (nodoHnoDer) {
				if ( pudoRedistribuir = nodoHnoDer->puedePasarClaveHaciaIzq(nodoTarget, nodoPadre, clavePadreDer))
					this->pasarClaveHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer, clavePadreDer);
				else if (!nodoHnoIzq) {
					//'nodoHnoHno' debe cederle a 'nodoHnoDer' y luego 'nodoHnoDer' debe cederle a 'nodoTarget'
					if ( (nodoHnoHno) && (nodoHnoHno->puedePasarClaveHaciaIzq(nodoHnoDer, nodoPadre, clavePadreIzq)) ) {
						this->pasarClaveHaciaIzquierda(nodoHnoDer, nodoPadre, nodoHnoHno, clavePadreIzq);
						if ( pudoRedistribuir = nodoHnoDer->puedePasarClaveHaciaIzq(nodoTarget, nodoPadre, clavePadreDer))
							this->pasarClaveHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer, clavePadreDer);
					}
				} else {
					if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
						this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
				}
			} else { // tiene hermano izquierdo
				if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
					this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
				else { //'nodoTarget' no tiene hermano derecho
					//'nodoHnoHno' debe cederle a 'nodoHnoIzq' y luego 'nodoHnoIzq' debe cederle a 'nodoTarget'
					if ( (nodoHnoHno) && (nodoHnoHno->puedePasarClaveHaciaDer(nodoHnoIzq, nodoPadre, clavePadreDer)) ) {
						this->pasarClaveHaciaDerecha(nodoHnoIzq, nodoPadre, nodoHnoHno, clavePadreDer);
						if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
							this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
					}
				}
			}
			
			if (!pudoRedistribuir) { //Se realiza la concatenación de 'nodoTarget' con dos nodos hermanos y dos claves
				//del nodo padre. Luego se realiza un split, generándose dos nodos y una clave a promocionar
				//hacia el nodo padre.
				
				Clave* clavePromocionada = NULL;
				
				if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoHnoDer) && (!nodoHnoIzq) && (!nodoHnoHno) )
					this->merge(nodoTarget, nodoHnoDer, this->nodoRaiz);
				else if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoHnoIzq) && (!nodoHnoDer) && (!nodoHnoHno) )
					this->merge(nodoTarget, nodoHnoIzq, this->nodoRaiz);
				else {
					if ((nodoHnoIzq) && (nodoHnoDer))
						clavePromocionada = this->mergeSplitUnderflow(nodoHnoIzq, nodoTarget, nodoHnoDer, clavePadreIzq, clavePadreDer);
					else if (!(nodoHnoIzq))
						if (nodoHnoHno)
							clavePromocionada = this->mergeSplitUnderflow(nodoTarget, nodoHnoDer, nodoHnoHno, clavePadreDer, clavePadreIzq);
					else if (nodoHnoHno)
						clavePromocionada = this->mergeSplitUnderflow(nodoHnoHno, nodoHnoIzq, nodoTarget, clavePadreDer, clavePadreIzq);
					
					nodoPadre->eliminarClave(clavePadreIzq, codigo);
					nodoPadre->eliminarClave(clavePadreDer, codigo);
					char codigoInsertar;
					nodoPadre->insertarClave(clavePromocionada, &codigoInsertar);
					
					if (nodoPadre->tieneOverflow()) //Se resuelve posible overflow al insertar la clave promocionada
						this->insertarInterno(nodoPadre, &codigoInsertar);
					//Sino, se resuelve posible underflow al eliminar las claves separadoras. Si no hay underflow,
					//este método se encargará de escribir nodoPadre en disco.
					else this->eliminarInterno(nodoPadre, codigo);
					
				}		
			}
			
			if (nodoHnoDer) delete nodoHnoDer;
			if (nodoHnoIzq) delete nodoHnoIzq;
			if (nodoHnoHno) delete nodoHnoHno;
			
			delete nodoPadre;
		}
		
	}
	
}


Clave* BStarTree::buscar(Clave* clave) const {
	
	if ( (!clave) || (this->vacio()) ) return NULL;
	
	NodoBStar* nodo = this->buscarClave(this->nodoRaiz, clave);
	
	if (!nodo) return NULL;
	
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


void BStarTree::pasarClaveHaciaIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer, Clave* clavePadre){
	
	char codigo;
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	SetClaves* setIntercambio = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	clavePadre->setHijoDer(nodoHnoDer->getHijoIzq());
	
	if ( tamanioClavePadre < bytesRequeridos){
		setIntercambio = nodoHnoDer->cederBytes(bytesRequeridos - tamanioClavePadre);
		nodoDestino->recibir(setIntercambio);
		delete setIntercambio;
	}
	
	Clave* clavePromocionada = nodoHnoDer->extraerPrimeraClave();
	nodoHnoDer->setHijoIzq(clavePromocionada->getHijoDer());
	clavePromocionada->setHijoDer(nodoHnoDer->getPosicionEnArchivo());
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	nodoDestino->insertarClave(clavePadre, &codigo);

}


void BStarTree::pasarClaveHaciaDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq, Clave* clavePadre) {
	
	char codigo;
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	SetClaves* setIntercambio = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	clavePadre->setHijoDer(nodoDestino->getHijoIzq());
	
	if ( tamanioClavePadre < bytesRequeridos){
		setIntercambio = nodoHnoIzq->cederBytes(bytesRequeridos - tamanioClavePadre);
		nodoDestino->recibir(setIntercambio);
		delete setIntercambio;
	}
	
	Clave* clavePromocionada = nodoHnoIzq->extraerUltimaClave();
	nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
	clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	nodoDestino->insertarClave(clavePadre, &codigo);	
	
}


void BStarTree::recibirClaveDesdeDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer, Clave* clavePadre){
	
	char codigo;
	unsigned short cantClavesSobrantes;
	unsigned short bytesSobrantes = nodoHnoDer->obtenerBytesSobrantes(cantClavesSobrantes);
	SetClaves* setIntercambio = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	clavePadre->setHijoDer(nodoHnoDer->getHijoIzq());
	
	setIntercambio = nodoHnoDer->cederBytes(bytesSobrantes);
	
	Clave* clavePromocionada = *(setIntercambio->rbegin());
	setIntercambio->erase(--(setIntercambio->end()));
	
	nodoHnoDer->setHijoIzq(clavePromocionada->getHijoDer());
	clavePromocionada->setHijoDer(nodoHnoDer->getPosicionEnArchivo());
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	nodoDestino->recibir(setIntercambio);
	nodoDestino->insertarClave(clavePadre, &codigo);

	delete setIntercambio;
		
}


void BStarTree::recibirClaveDesdeIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq, Clave* clavePadre){

	
	char codigo;
	unsigned short cantClavesSobrantes;
	unsigned short bytesSobrantes = nodoHnoIzq->obtenerBytesSobrantes(cantClavesSobrantes);
	SetClaves* setIntercambio = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	clavePadre->setHijoDer(nodoDestino->getHijoIzq());
	
	setIntercambio = nodoHnoIzq->cederBytes(bytesSobrantes);
	
	Clave* clavePromocionada = *(setIntercambio->begin());
	setIntercambio->erase(setIntercambio->begin());
	
	nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
	clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	nodoDestino->recibir(setIntercambio);
	nodoDestino->insertarClave(clavePadre, &codigo);

	delete setIntercambio;
	
}


NodoBStar* BStarTree::buscarPadre(NodoBStar* padre, NodoBStar* hijo) const {
	
	NodoBStar *auxNodo = NULL, *nuevoNodo = NULL;
	Clave* claveOrientadora = NULL;

	if ( (!padre) || (!hijo) || (*hijo == *(this->nodoRaiz)) ) return NULL;
	
	if (padre->esPadre(hijo, claveOrientadora)) {
		
		auxNodo = new NodoBStar(0, 0, this->tamanioNodo);
		//Devuelvo una copia del padre
		*auxNodo = *padre; //Operador =
		
	} else {
		
    	if (claveOrientadora == NULL) {
    		nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
    		indiceManager.leerBloque(padre->getHijoIzq(), nuevoNodo);
    	} else {
    		nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
    		indiceManager.leerBloque(claveOrientadora->getHijoDer(), nuevoNodo);
    	}
    	
    	auxNodo = buscarPadre(nuevoNodo, hijo);
    	
    	delete nuevoNodo;
    	
    }
	
	return auxNodo;
	
}


NodoBStar* BStarTree::buscarClave(NodoBStar* nodo, Clave* clave) const {
	
	NodoBStar *nuevoNodo = NULL, *auxNodo = NULL;
		
	Clave* claveResultante = nodo->buscar(clave);
	
	if (claveResultante && (*claveResultante == *clave))
		return nodo;
	
	if (claveResultante == NULL) {
		
		if (nodo->getHijoIzq() == 0) { //Nodo hoja
			return NULL;
		} else {
			nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(nodo->getHijoIzq(), nuevoNodo);
			auxNodo = buscarClave(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	} else {
		
		if (claveResultante->getHijoDer() == 0) {//Nodo hoja
			return NULL;
		} else {
			nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(claveResultante->getHijoDer(), nuevoNodo);
			auxNodo = buscarClave(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
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
			auxNodo = new NodoBStar(0, 0, this->tamanioNodo);
			//Devuelvo una copia de 'nodo'
			*auxNodo = *nodo; //Operador =
		} else {
			nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(nodo->getHijoIzq(), nuevoNodo);
			auxNodo = buscarLugarRecursivo(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	} else {
		
		if (claveResultante->getHijoDer() == 0) {//Nodo hoja
			auxNodo = new NodoBStar(0, 0, this->tamanioNodo);
			//Devuelvo una copia de 'nodo'
			*auxNodo = *nodo; //Operador =
		} else {
			nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(claveResultante->getHijoDer(), nuevoNodo);
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
	
	VectorConjuntos* vectorConjuntos = nodoTarget->split(); //En nodoTarget quedan las claves mayores
	
	NodoBStar* nuevoNodo = new NodoBStar(nodoHno->getHijoIzq(), nodoHno->getNivel(), nodoHno->getTamanio());
	
	nuevoNodo->setClaves((*vectorConjuntos)[0]);
	nodoHno->setClaves((*vectorConjuntos)[1]);
	
	delete vectorConjuntos;
	
	Clave* ultimaClaveNuevoNodo = nuevoNodo->obtenerUltimaClave(); 
	Clave* ultimaClaveNodoHno = nodoHno->obtenerUltimaClave();
	
	//Se extraen las claves a promocionar
	nuevoNodo->extraerUltimaClave();
	nodoHno->extraerUltimaClave();
	
	nodoHno->setHijoIzq(ultimaClaveNuevoNodo->getHijoDer());
	nodoTarget->setHijoIzq(ultimaClaveNodoHno->getHijoDer());

	ultimaClaveNuevoNodo->setHijoDer(nodoHno->getPosicionEnArchivo());
	ultimaClaveNodoHno->setHijoDer(nodoTarget->getPosicionEnArchivo());
	
	//Se escribe nuevoNodo en disco y se actualiza nodoHno y nodoTarget
	indiceManager.escribirBloque(nuevoNodo);
	indiceManager.escribirBloque(nodoHno->getPosicionEnArchivo(), nodoHno);
	indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);
	
	
	VectorClaves* vectorClaves = new VectorClaves(3);
	
	vectorClaves->push_back(ultimaClaveNuevoNodo);
	vectorClaves->push_back(ultimaClaveNodoHno);
	//Agrego una tercera clave que sólo se utiliza para guardar la referencia
	//en disco al nuevoNodo.
	vectorClaves->push_back(new ClaveEntera(0,0,nuevoNodo->getPosicionEnArchivo()));
	
	delete nuevoNodo;
	
	return vectorClaves;
}

//Método utilizado para mergear la raíz con dos hijos
void BStarTree::merge(NodoBStar* &nodoHijoIzq, NodoBStar* &nodoHijoDer, NodoBStar* nodoPadre) {
	
	nodoPadre->setHijoIzq(0);
	nodoPadre->obtenerPrimeraClave()->setHijoDer(0);
	nodoPadre->setNivel(0);
	
	nodoPadre->merge(nodoHijoIzq, nodoHijoDer, NULL, NULL);
	
	//TODO Escritura especial de la raíz

	//Se elimina de disco a los nodos nodoHijoDer y nodoHijoIzq
	indiceManager.eliminarBloque(nodoHijoIzq->getPosicionEnArchivo());
	indiceManager.eliminarBloque(nodoHijoDer->getPosicionEnArchivo());
	
	delete nodoHijoIzq;
	nodoHijoIzq = NULL;
	
	delete nodoHijoDer;
	nodoHijoDer = NULL;
	
}


Clave* BStarTree::mergeSplitUnderflow(NodoBStar* nodoTarget, NodoBStar* nodoHno1, NodoBStar* &nodoHno2,
									  Clave* clavePadre1, Clave* clavePadre2) {
	
	Clave* copiaClavePadre1 = clavePadre1->copiar();
	Clave* copiaClavePadre2 = clavePadre2->copiar();
	
	copiaClavePadre1->setHijoDer(nodoHno1->getHijoIzq());
	copiaClavePadre2->setHijoDer(nodoHno2->getHijoIzq());
		
	nodoTarget->merge(nodoHno1, nodoHno2, copiaClavePadre1, copiaClavePadre2);
	//Se realiza el split indicándole al método que a nodoTarget lo deje lleno y que el resto
	//de las claves las devuelva en setClavesMayores.
	SetClaves* setClavesMayores = nodoTarget->splitB( nodoTarget->getTamanioEspacioClaves() );
	Clave* clavePromocionada = *(setClavesMayores->begin());
	setClavesMayores->erase(setClavesMayores->begin());
	
	nodoHno1->setClaves(setClavesMayores);
	nodoHno1->setHijoIzq(clavePromocionada->getHijoDer());
	clavePromocionada->setHijoDer(nodoHno1->getPosicionEnArchivo());
		
	//Se actualiza nodoTarget
	indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);	
	//Se actualiza nodoHno1
	indiceManager.escribirBloque(nodoHno1->getPosicionEnArchivo(), nodoHno1);
	//se elimina de disco el nodo nodoHno2
	indiceManager.eliminarBloque(nodoHno2->getPosicionEnArchivo());
	
	delete nodoHno2;
	nodoHno2 = NULL;
	
	return clavePromocionada;
}

