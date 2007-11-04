///////////////////////////////////////////////////////////////////////////
//	Archivo   : BPlusTree.cpp
//  Namespace : CapaIndice
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de la clase BPlusTree.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "BPlusTree.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BPlusTree (Implementa Arboles B+ en disco)
///////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Contructor/Destructor
//////////////////////////////////////////////////////////////////////
BPlusTree::BPlusTree(IndiceManager& indiceManager, unsigned short tamanioNodo)
	: BTree(indiceManager, tamanioNodo) {
	
	this->nodoRaiz = NULL;
	this->nodoActual = NULL;
}


BPlusTree::~BPlusTree() {
	if (this->nodoRaiz) delete nodoRaiz;
	if (this->nodoActual) delete nodoActual;
}


NodoBPlus* BPlusTree::getRaiz() const
{
	//Lee el primer registro del archivo -> la raiz
	//TODO Cargar la raiz desde disco!!
	//return new NodoBPlus(archivoIndice, 0);
	return NULL;
}


void BPlusTree::primero()
{
	if (this->vacio()) return;
	
	/*El nodo actual termina siendo el primer nodo hoja del arbol B+
	(para recorridos)*/	
	this->nodoActual = this->nodoRaiz;
	unsigned int refHijoIzq;
	/*Busco hasta la hoja*/
	while(this->nodoActual->getNivel() != 0){
		refHijoIzq =  this->nodoActual->getHijoIzq();
		if (this->nodoActual != this->nodoRaiz) delete this->nodoActual;
		this->nodoActual = new NodoBPlus(0, 0, this->tamanioNodo);
		indiceManager.leerBloque(refHijoIzq, this->nodoActual);
	}
}


void BPlusTree::insertar(Clave* clave)
{
	if (!clave) return;
	
	if (this->vacio()) {
		
		this->nodoRaiz = new NodoBPlus(0, 0, clave, this->tamanioNodo);
		
		//TODO Escribir la raíz en disco
		
	} else {
		
		//Se busca el nodo hoja donde se debe insertar la clave
		NodoBPlus* nodoDestino = buscarLugar(clave);
		
		char codigo;
		
		nodoDestino->insertarClave(clave, &codigo);
		
		//La clave queda insertada independientemente de si hay OVERFLOW o no.
		
		insertarInterno(nodoDestino, &codigo);
		
		delete nodoDestino;
		
	}
}


void BPlusTree::insertarInterno(NodoBPlus* &nodoDestino, char* codigo) {
	
	if (*codigo == Codigo::MODIFICADO){
		//Se actualiza en disco el nodo modificado.
		indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	}
	else if ( *codigo == Codigo::OVERFLOW ){
		NodoBPlus* nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
		
		if (!nodoPadre){ //nodoDestino es la raiz.
			SetClaves* setClavesDerecho = nodoDestino->splitB(nodoDestino->getTamanioMinimo());
			Clave* clavePromocionada = (*(setClavesDerecho->begin()))->copiar();
			NodoBPlus* nuevoNodoDerecho = new NodoBPlus(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), this->tamanioNodo);
			nuevoNodoDerecho->setClaves(setClavesDerecho);
			indiceManager.escribirBloque(nuevoNodoDerecho);
			clavePromocionada->setHijoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			NodoBPlus* nuevaRaiz = new NodoBPlus(nodoDestino->getPosicionEnArchivo(), nodoDestino->getNivel() + 1, this->tamanioNodo);
			//TODO escritura de la raiz
			nodoDestino->setHnoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			delete nuevoNodoDerecho;
			this->nodoRaiz = nuevaRaiz; //No se libera la memoria ocupada por el nodo raíz, ya que siempre
										//lo tenemos cargado en memoria.
			*codigo = Codigo::MODIFICADO;
		}
		else{
			//Se realiza el split del nodo con overflow
			Clave* clavePromocionada = this->split(nodoDestino);
			
			nodoPadre->insertarClave(clavePromocionada, codigo);
			
			//Llamada recursiva para chequear overflow en el padre
			insertarInterno(nodoPadre, codigo);
			
			delete nodoPadre;
		}
		
	}
	
}


bool BPlusTree::eliminar(Clave* clave) {
	
	if ( (!clave) || (this->vacio()) ) return false;
	
	NodoBPlus* nodoTarget = buscarLugar(clave);
	Clave* claveBuscada = nodoTarget->buscar(clave);
	
	char codigo;
	
	if ( (claveBuscada) && (*claveBuscada == *clave) ){
		nodoTarget->eliminarClave(claveBuscada, &codigo);
		this->eliminarInterno(nodoTarget, &codigo); //resuelve underflow y escribe en disco
	}
	else{
		delete nodoTarget;
		return false;
	}
	
	delete nodoTarget;
	
	return true;
}


void BPlusTree::eliminarInterno(NodoBPlus* nodoTarget, char* codigo) {

	if (*codigo == Codigo::MODIFICADO) {
		//Se actualiza en disco el nodo modificado.
		indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);
	} else if (*codigo == Codigo::UNDERFLOW) {
		
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBPlus *nodoPadre = this->buscarPadre(this->nodoRaiz, nodoTarget);
		NodoBPlus *nodoHnoDer = NULL, *nodoHnoIzq = NULL;
		
		if (!nodoPadre) {//nodoTarget es el nodo raíz, no se chequea underflow.
			*codigo = Codigo::MODIFICADO;
			if (nodoTarget->getCantidadClaves() == 0) {
				//TODO Se "elimina" la raíz de disco.
				//La memoria utilizada por el nodo se libera al final del método eliminar
				this->nodoRaiz = NULL;
			} else {
				//TODO Escritura de la raíz
			}
		} else {
		
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
				//Se tendrían que cargar el hno derecho
				iterPadre = nodoPadre->getClaves()->begin();
				nodoHnoDer = new NodoBPlus(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*iterPadre)->getHijoDer(), nodoHnoDer);
				clavePadreDer = *iterPadre;
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoTarget es el hijo derecho de la última clave del nodo
				//nodoTarget no tiene hermano derecho
				if ( (--iterPadre) == nodoPadre->getClaves()->begin() ) {
					nodoHnoIzq = new NodoBPlus(0, 0, this->tamanioNodo);
					indiceManager.leerBloque(nodoPadre->getHijoIzq(), nodoHnoIzq);
					clavePadreIzq = *iterPadre;
				}
				else{
					nodoHnoIzq = new NodoBPlus(0, 0, this->tamanioNodo);
					indiceManager.leerBloque((*(--iterPadre))->getHijoDer(), nodoHnoIzq);
					clavePadreIzq = *(++iterPadre);
				}	
			} else {
				--iterPadre; //Lo decremento xq se incrementó en el 'else if' anterior
				if ( iterPadre == nodoPadre->getClaves()->begin() ) {
					nodoHnoIzq = new NodoBPlus(0, 0, this->tamanioNodo);
					indiceManager.leerBloque(nodoPadre->getHijoIzq(), nodoHnoIzq);
					clavePadreIzq = *iterPadre;
				}
				else{
					nodoHnoIzq = new NodoBPlus(0, 0, this->tamanioNodo);
					indiceManager.leerBloque((*(--iterPadre))->getHijoDer(), nodoHnoIzq);
					clavePadreIzq = *(++iterPadre);
				}
				nodoHnoDer = new NodoBPlus(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*(++iterPadre))->getHijoDer(), nodoHnoDer);
				clavePadreDer = *iterPadre;
			}
			
			//Fin de la búsqueda de los nodos hermanos de 'nodoTarget'
			
			//Se intenta hacer una redistribución de claves con el hermano derecho de 'nodoTarget'.
			//Si esto no es posible, se intenta hacer una redistribución con el hermano izquierdo.
			bool pudoRedistribuir = false;
			
			if (nodoHnoDer) {
				if ( pudoRedistribuir = nodoHnoDer->puedePasarClaveHaciaIzq(nodoTarget, nodoPadre, clavePadreDer))
					this->pasarClaveHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer, clavePadreDer);
				else if (nodoHnoIzq) {
					if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
						this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
				}
			} else { // tiene hermano izquierdo
				if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
					this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
			}
			
			if (!pudoRedistribuir) { //Se realiza la concatenación de 'nodoTarget' con un nodo hermano. Si es un nodo
									//interno, también se concatena con un separador del padre.
				
				if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoPadre->getCantidadClaves() == 1) ) {
					if (nodoHnoDer) this->merge(nodoTarget, nodoHnoDer, nodoPadre->obtenerPrimeraClave());
					else this->merge(nodoTarget, nodoHnoIzq, nodoPadre->obtenerPrimeraClave());
					this->nodoRaiz = (NodoBPlus*)nodoTarget->copiar();
					//TODO Reemplazar la raíz vieja por la nueva
				} else {
					
					if (nodoHnoDer) {
						this->merge(nodoTarget, nodoHnoDer, clavePadreDer);
						nodoPadre->eliminarClave(clavePadreDer, codigo);
					} else {
						this->merge(nodoHnoIzq, nodoTarget, clavePadreIzq);
						nodoPadre->eliminarClave(clavePadreIzq, codigo);
					}
					
					//Se resuelve posible underflow al eliminar la clave separadora. Si no hay underflow,
					//este método se encargará de escribir nodoPadre en disco.
					this->eliminarInterno(nodoPadre, codigo);
					
				}		
			}
			
			if (nodoHnoDer) delete nodoHnoDer;
			if (nodoHnoIzq) delete nodoHnoIzq;
			
			delete nodoPadre;
		}
		
	}
	
}


Clave* BPlusTree::buscar(Clave* clave) const {
	
	if ( (!clave) || (this->vacio()) ) return NULL;
	
	NodoBPlus* nodo = this->buscarLugar(clave);
	
	SetClaves::iterator iter = nodo->getClaves()->find(clave);
	
	if (iter != nodo->getClaves()->end())
		return *iter;
	
	return NULL;
	
}

		
bool BPlusTree::modificar(Clave* claveVieja, Clave* claveNueva) {
	
	if ( eliminar(claveVieja) ) insertar(claveNueva);
	else return false;
	
	return true;
}


NodoBPlus* BPlusTree::buscarPadre(NodoBPlus* padre, NodoBPlus* hijo) const {
	
	NodoBPlus *auxNodo = NULL, *nuevoNodo = NULL;
	Clave* claveOrientadora = NULL;

	if ( (!padre) || (!hijo) || (*hijo == *(this->nodoRaiz)) ) return NULL;
	
	if (padre->esPadre(hijo, claveOrientadora)) {
		
		auxNodo = new NodoBPlus(0, 0, this->tamanioNodo);
		//Devuelvo una copia del padre
		*auxNodo = *padre; //Operador =
		
	} else {
		
    	if (claveOrientadora == NULL) {
    		nuevoNodo = new NodoBPlus(0, 0, this->tamanioNodo);
    		indiceManager.leerBloque(padre->getHijoIzq(), nuevoNodo);
    	} else {
    		nuevoNodo = new NodoBPlus(0, 0, this->tamanioNodo);
    		indiceManager.leerBloque(claveOrientadora->getHijoDer(), nuevoNodo);
    	}
    	
    	auxNodo = buscarPadre(nuevoNodo, hijo);
    	
    	delete nuevoNodo;
    	
    }
	
	return auxNodo;
	
}


NodoBPlus* BPlusTree::buscarLugar(Clave* clave) const {
	
	if (!clave) return NULL;
	
	//Se supone que el nodo raíz ya se encuentra cargado en memoria.
	return buscarLugarRecursivo(this->nodoRaiz, clave);
	
}


NodoBPlus* BPlusTree::buscarLugarRecursivo(NodoBPlus* nodo, Clave* clave) const {
	
	NodoBPlus *nuevoNodo = NULL, *auxNodo = NULL;
	
	Clave* claveResultante = nodo->buscar(clave);
		
	if (claveResultante == NULL) {
		
		if (nodo->getHijoIzq() == 0) { //Nodo hoja
			auxNodo = new NodoBPlus(0, 0, this->tamanioNodo);
			//Devuelvo una copia de 'nodo'
			*auxNodo = *nodo;
		} else {
			nuevoNodo = new NodoBPlus(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(nodo->getHijoIzq(), nuevoNodo);
			auxNodo = buscarLugarRecursivo(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	} else {
		
		if (claveResultante->getHijoDer() == 0) {//Nodo hoja
			auxNodo = new NodoBPlus(0, 0, this->tamanioNodo);
			//Devuelvo una copia de 'nodo'
			*auxNodo = *nodo;
		} else {
			nuevoNodo = new NodoBPlus(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(claveResultante->getHijoDer(), nuevoNodo);
			auxNodo = buscarLugarRecursivo(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	}
	
	return auxNodo;

}


Clave* BPlusTree::split(NodoBPlus* nodoTarget) {
	
	Clave* clavePromocionada;
	SetClaves* clavesMayores = nodoTarget->splitB(nodoTarget->getTamanioMinimo()); //En nodoTarget quedan las claves menores
	NodoBPlus* nuevoNodo = 	NULL;
	
	if (nodoTarget->getNivel() == 0){ //Si es hoja
		nuevoNodo = new NodoBPlus(nodoTarget->getHnoDer(), nodoTarget->getNivel(), nodoTarget->getTamanio());
		nuevoNodo->setClaves(clavesMayores);
		indiceManager.escribirBloque(nuevoNodo);
		nodoTarget->setHnoDer(nuevoNodo->getPosicionEnArchivo());
		clavePromocionada = (*(clavesMayores->begin()))->copiar();
	}
	else{
		clavePromocionada = (*(clavesMayores->begin()));
		clavesMayores->erase(clavesMayores->begin());
		nuevoNodo = new NodoBPlus(clavePromocionada->getHijoDer(), nodoTarget->getNivel(), nodoTarget->getTamanio());
		nuevoNodo->setClaves(clavesMayores);
		indiceManager.escribirBloque(nuevoNodo);
	}
	
	clavePromocionada->setHijoDer(nuevoNodo->getPosicionEnArchivo());
	delete nuevoNodo;
	
	return clavePromocionada;
}


void BPlusTree::merge(NodoBPlus* nodoIzquierdo, NodoBPlus* &nodoDerecho, Clave* separador) {
	
	if (nodoIzquierdo->getNivel() == 0) { //nodoIzquierdo es un nodo hoja
		nodoIzquierdo->merge(nodoDerecho, NULL);
		nodoIzquierdo->setHnoDer(nodoDerecho->getHnoDer());
	} else {
		separador->setHijoDer(nodoDerecho->getHijoIzq());
		nodoIzquierdo->merge(nodoDerecho, separador->copiar());
	}
	
	indiceManager.eliminarBloque(nodoDerecho->getPosicionEnArchivo());
	indiceManager.escribirBloque(nodoIzquierdo->getPosicionEnArchivo(), nodoIzquierdo);
	
	delete nodoDerecho;
	nodoDerecho = NULL;
	
}


void BPlusTree::pasarClaveHaciaIzquierda(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre, NodoBPlus* nodoHnoDer, Clave* clavePadre){
	
	char codigo;
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	SetClaves* setIntercambio = NULL;
	Clave* clavePromocionada = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	
	if (nodoDestino->getNivel() == 0) {
	
		setIntercambio = nodoHnoDer->cederBytes(bytesRequeridos);
		
		clavePromocionada = nodoHnoDer->obtenerPrimeraClave()->copiar();
		
	} else {
	
		clavePadre->setHijoDer(nodoHnoDer->getHijoIzq());
		
		if ( tamanioClavePadre < bytesRequeridos)
			setIntercambio = nodoHnoDer->cederBytes(bytesRequeridos - tamanioClavePadre);

		clavePromocionada = nodoHnoDer->extraerPrimeraClave();
	
		nodoHnoDer->setHijoIzq(clavePromocionada->getHijoDer());
		
		nodoDestino->insertarClave(clavePadre, &codigo);
		
	}
		
	clavePromocionada->setHijoDer(nodoHnoDer->getPosicionEnArchivo());
	
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	if (setIntercambio) {
		nodoDestino->recibir(setIntercambio);
		delete setIntercambio;
	}

}


void BPlusTree::pasarClaveHaciaDerecha(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre, NodoBPlus* nodoHnoIzq, Clave* clavePadre) {

	char codigo;
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	SetClaves* setIntercambio = NULL;
	Clave* clavePromocionada = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	
	if (nodoDestino->getNivel() == 0) {
	
		setIntercambio = nodoHnoIzq->cederBytes(bytesRequeridos, false);
		
		clavePromocionada = (*(setIntercambio->begin()))->copiar();
		
	} else {
	
		clavePadre->setHijoDer(nodoDestino->getHijoIzq());
		
		if ( tamanioClavePadre < bytesRequeridos)
			setIntercambio = nodoHnoIzq->cederBytes(bytesRequeridos - tamanioClavePadre, false);

		clavePromocionada = *(setIntercambio->begin());
		
		setIntercambio->erase(setIntercambio->begin());
	
		nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
		
		nodoDestino->insertarClave(clavePadre, &codigo);
		
	}
		
	clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
	
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	if (setIntercambio) {
		nodoDestino->recibir(setIntercambio);
		delete setIntercambio;
	}
	
}

