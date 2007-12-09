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
	
	this->getRaiz();
	this->nodoActual = NULL;
}


BPlusTree::~BPlusTree() {
	if (this->nodoRaiz) delete nodoRaiz;
	if (this->nodoActual) delete nodoActual;
}


NodoBPlus* BPlusTree::getRaiz()
{
	//Lee el primer bloque del archivo -> la raiz
	this->nodoRaiz = new NodoBPlus(0, 0, this->tamanioNodo);
	int resultado = indiceManager.leerBloque(0, this->nodoRaiz);
	if (resultado != ResultadosFisica::OK) this->nodoRaiz = NULL;

	return this->nodoRaiz;
}


void BPlusTree::primero()
{
	if (this->vacio()) return;
	
	if ( (this->nodoActual) && (this->nodoActual != this->nodoRaiz) ) delete this->nodoActual;
	
	//El nodo actual termina siendo el primer nodo hoja del arbol B+
	//(para recorridos)
	this->nodoActual = this->nodoRaiz;
	unsigned int refHijoIzq;
	//Busco hasta la hoja
	while(this->nodoActual->getNivel() != 0){
		refHijoIzq =  this->nodoActual->getHijoIzq();
		if (this->nodoActual != this->nodoRaiz) delete this->nodoActual;
		this->nodoActual = new NodoBPlus(0, 0, this->tamanioNodo);
		indiceManager.leerBloque(refHijoIzq, this->nodoActual);
	}

	this->iterClavesActual = this->nodoActual->getClaves()->begin();
}

void BPlusTree::mayorOIgual(Clave* clave){
	
	if ( (!clave) || (this->vacio()) ) return;
	
	if ( (this->nodoActual) && (this->nodoActual != this->nodoRaiz) ) delete this->nodoActual;
	
	this->nodoActual = this->buscarLugar(clave);
	this->iterClavesActual = this->nodoActual->getClaves()->find(clave);
	
	if (this->iterClavesActual == this->nodoActual->getClaves()->end()){

		Clave* auxClave = this->nodoActual->getClaves()->findClaveSiguiente(clave);

		if (auxClave)
			this->iterClavesActual = this->nodoActual->getClaves()->find(auxClave);
		else{
			--this->iterClavesActual;
			auxClave = siguiente();
			if (auxClave) delete auxClave;
		}
		
	} 
	
}

void BPlusTree::mayor(Clave* clave){
	
	if ( (!clave) || (this->vacio()) ) return;
	
	if ( (this->nodoActual) && (this->nodoActual != this->nodoRaiz) ) delete this->nodoActual;
	
	this->nodoActual = this->buscarLugar(clave);
	this->iterClavesActual = this->nodoActual->getClaves()->find(clave);
	
	Clave* auxClave = NULL;
	
	if (this->iterClavesActual != this->nodoActual->getClaves()->end()){
		auxClave = siguiente();
		if (auxClave) delete auxClave;
	}
	else{
		auxClave = this->nodoActual->getClaves()->findClaveSiguiente(clave);

		if (auxClave)
			this->iterClavesActual = this->nodoActual->getClaves()->find(auxClave);
		else{
			--this->iterClavesActual;
			auxClave = siguiente();
			if (auxClave) delete auxClave;
		}
		
	} 
	
}

Clave* BPlusTree::siguiente(){
	
	if (!this->nodoActual) return NULL;
	
	unsigned int refHnoDer;
	
	//Se usa un while para evitar problemas al encontrar un nodo vacío.
	while ( ((refHnoDer = this->nodoActual->getHnoDer()) != 0) && (this->iterClavesActual == this->nodoActual->getClaves()->end()) ) {
		delete this->nodoActual;
		this->nodoActual = new NodoBPlus(0, 0, this->tamanioNodo);
		indiceManager.leerBloque(refHnoDer, this->nodoActual);
		this->iterClavesActual = this->nodoActual->getClaves()->begin();
	}
	
	if (this->iterClavesActual != this->nodoActual->getClaves()->end()) return (*(this->iterClavesActual++))->copiar();
	
	return NULL;
}


bool BPlusTree::insertar(Clave* clave)
{	
	if (!clave) return false;
	
	if (this->vacio()) {
		
		this->nodoRaiz = new NodoBPlus(0, 0, clave, this->tamanioNodo);
		
		indiceManager.escribirBloque(this->nodoRaiz);
		
	} else {
		//Se busca el nodo hoja donde se debe insertar la clave
		NodoBPlus* nodoDestino = buscarLugar(clave);

		char codigo;

		nodoDestino->insertarClave(clave, &codigo);
		
		//Se verifica si la clave ya estaba insertada
		if (codigo == Codigo::NO_MODIFICADO) {
			delete nodoDestino;
			return false;
		}

		//La clave queda insertada independientemente de si hay OVERFLOW o no.
		
		insertarInterno(nodoDestino, &codigo, clave);

		if (*nodoDestino == *(this->nodoRaiz))
			*(this->nodoRaiz) = *nodoDestino;
		
		delete nodoDestino; //No se libera la memoria de this->nodoRaiz ya que nodoDestino
							//era una copia
		
	}
	
	return true;
}


void BPlusTree::insertarInterno(NodoBPlus* &nodoDestino, char* codigo, Clave* claveInsertada) {
	
	if (*codigo == Codigo::MODIFICADO){
		//Se actualiza en disco el nodo modificado.
		indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	}
	else if ( *codigo == Codigo::OVERFLOW ){
			
		NodoBPlus* nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino, claveInsertada);
		
		if (!nodoPadre){ //nodoDestino es la raiz.
			
			SetClaves* setClavesDerecho = nodoDestino->splitB(nodoDestino->getTamanioMinimo());
			Clave* clavePromocionada = NULL;
			NodoBPlus *nuevoNodoDerecho = NULL, *nuevaRaiz = NULL;
			
			
			if (nodoDestino->getNivel() == 0){ //Si es hoja
				clavePromocionada = (*(setClavesDerecho->begin()))->copiar();
				nuevoNodoDerecho = new NodoBPlus(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), this->tamanioNodo);
				nuevoNodoDerecho->setClaves(setClavesDerecho);
				indiceManager.escribirBloque(nuevoNodoDerecho);
				nodoDestino->setHnoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			}
			else{
				clavePromocionada = (*(setClavesDerecho->begin()));
				setClavesDerecho->erase(setClavesDerecho->begin());
				nuevoNodoDerecho = new NodoBPlus(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), this->tamanioNodo);
				nuevoNodoDerecho->setClaves(setClavesDerecho);
				indiceManager.escribirBloque(nuevoNodoDerecho);
			}
			
			//Se actualiza nodoDestino
			indiceManager.escribirBloque(nodoDestino);
			clavePromocionada->setHijoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			
			nuevaRaiz = new NodoBPlus(nodoDestino->getPosicionEnArchivo(), nodoDestino->getNivel() + 1,
									  clavePromocionada, this->tamanioNodo);
			
			//Se escribe la nueva raíz
			indiceManager.escribirBloque(0, nuevaRaiz);
			nuevaRaiz->setPosicionEnArchivo(0);
			*(this->nodoRaiz) = *nuevaRaiz;
			delete nuevoNodoDerecho;
			delete nuevaRaiz;
			*codigo = Codigo::MODIFICADO;
			
			
		}
		else{			
			//Se realiza el split del nodo con overflow
			Clave* clavePromocionada = this->split(nodoDestino);
			
			nodoPadre->insertarClave(clavePromocionada, codigo);
			
			if (*nodoPadre == *(this->nodoRaiz))
				*(this->nodoRaiz) = *nodoPadre;
			
			//Llamada recursiva para chequear overflow en el padre
			insertarInterno(nodoPadre, codigo, claveInsertada);
			
			delete nodoPadre;
		}
		
	}
	
}


bool BPlusTree::eliminar(Clave* clave) {

	if ( (!clave) || (this->vacio()) ) return false;
	
	NodoBPlus* nodoTarget = buscarLugar(clave);
	
	Clave* claveBuscada = nodoTarget->buscar(clave);

	char codigo = 0;
	
	if ( (claveBuscada) && (*claveBuscada == *clave) ) {
		nodoTarget->eliminarClave(claveBuscada, &codigo);
		this->eliminarInterno(nodoTarget, &codigo, clave); //resuelve underflow y escribe en disco
	}
	else {
		delete nodoTarget;
		return false;
	}

	if ((this->nodoRaiz) && (nodoTarget) && (*nodoTarget == *(this->nodoRaiz)))
		*(this->nodoRaiz) = *nodoTarget;

	delete nodoTarget;
	
	return true;
}


void BPlusTree::eliminarInterno(NodoBPlus* &nodoTarget, char* codigo, Clave* claveEliminada) {
	
	if (*codigo == Codigo::MODIFICADO) {
		//Se actualiza en disco el nodo modificado.
		indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);
	} else if (*codigo == Codigo::UNDERFLOW) {
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBPlus *nodoPadre = this->buscarPadre(this->nodoRaiz, nodoTarget, claveEliminada);
		NodoBPlus *nodoHnoDer = NULL, *nodoHnoIzq = NULL;

		if (!nodoPadre) {//nodoTarget es el nodo raíz, no se chequea underflow.
			*codigo = Codigo::MODIFICADO;
			if (nodoTarget->getCantidadClaves() == 0) {
				indiceManager.eliminarBloque(nodoTarget->getPosicionEnArchivo());
				delete this->nodoRaiz;
				this->nodoRaiz = NULL;
			} else {
				indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);
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
				
				if (!pudoRedistribuir) this->pasarMaximoPosibleHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer, clavePadreDer);
				
			} else { // tiene hermano izquierdo
				if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
					this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
				
				if (!pudoRedistribuir) this->pasarMaximoPosibleHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
				
			}
			
			if (!pudoRedistribuir) { //Se realiza la concatenación de 'nodoTarget' con un nodo hermano. Si es un nodo
									//interno, también se concatena con un separador del padre.

				bool pudoMerge = false;
				if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoPadre->getCantidadClaves() == 1) ) {
					if (nodoHnoDer){
						if (pudoMerge = this->puedeMerge(nodoTarget, nodoHnoDer, nodoPadre->obtenerPrimeraClave())){
							this->merge(nodoTarget, nodoHnoDer, nodoPadre->obtenerPrimeraClave());
							*nodoPadre = *nodoTarget;
							nodoPadre->setPosicionEnArchivo(0);
							if (nodoTarget->getNivel() == 0) nodoPadre->setHijoIzq(0);
							//Se escribe la nueva raiz
							indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
							//Se elimina de disco el bloque que ocupaba nodoTarget
							indiceManager.eliminarBloque(nodoTarget->getPosicionEnArchivo());
						}
							
					}
					else{
						if (pudoMerge = this->puedeMerge(nodoHnoIzq, nodoTarget, nodoPadre->obtenerPrimeraClave())){
							this->merge(nodoHnoIzq, nodoTarget, nodoPadre->obtenerPrimeraClave());
							*nodoPadre = *nodoHnoIzq;
							nodoPadre->setPosicionEnArchivo(0);
							if (nodoHnoIzq->getNivel() == 0) nodoPadre->setHijoIzq(0);
							//Se escribe la nueva raiz
							indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
							//Se elimina de disco el bloque que ocupaba nodoTarget
							indiceManager.eliminarBloque(nodoHnoIzq->getPosicionEnArchivo());
						}
					}
					
					if (!pudoMerge){
						*codigo = Codigo::MODIFICADO;
						this->eliminarInterno(nodoTarget, codigo, claveEliminada);
					}
				} else {
					
					if (nodoHnoDer) {
						if (pudoMerge = this->puedeMerge(nodoTarget, nodoHnoDer, clavePadreDer)){
							this->merge(nodoTarget, nodoHnoDer, clavePadreDer);
							nodoPadre->eliminarClave(clavePadreDer, codigo);
						}
					} else {
						if (pudoMerge = this->puedeMerge(nodoHnoIzq, nodoTarget, clavePadreIzq)){
							this->merge(nodoHnoIzq, nodoTarget, clavePadreIzq);
							nodoPadre->eliminarClave(clavePadreIzq, codigo);
						}
					}
	
					if (!pudoMerge){
						*codigo = Codigo::MODIFICADO;
						this->eliminarInterno(nodoTarget, codigo, claveEliminada);
					}
					else
						//Se resuelve posible underflow al eliminar la clave separadora. Si no hay underflow,
						//este método se encargará de escribir nodoPadre en disco.
						this->eliminarInterno(nodoPadre, codigo, claveEliminada);
					
				}		
			}
			
			if (nodoHnoDer) delete nodoHnoDer;
			if (nodoHnoIzq) delete nodoHnoIzq;
			
			if (*nodoPadre == *(this->nodoRaiz))
				*(this->nodoRaiz) = *nodoPadre;
			
			delete nodoPadre;
			nodoPadre = NULL;
		}
		
	}
	
}


Clave* BPlusTree::buscar(Clave* clave) const {
	
	if ( (!clave) || (this->vacio()) ) return NULL;
	
	NodoBPlus* nodo = this->buscarLugar(clave);
	
	SetClaves::iterator iter = nodo->getClaves()->find(clave);
	
	Clave* claveBuscada = NULL;
	
	if (iter != nodo->getClaves()->end())
		claveBuscada = (*iter)->copiar();
	
	delete nodo;
	
	return claveBuscada;
	
}

		
int BPlusTree::modificar(Clave* claveVieja, Clave* claveNueva) {
	
	int resultado = ResultadosIndices::CLAVE_NO_ENCONTRADA;
	
	Clave* claveBuscada = this->buscar(claveVieja);
	
	if (!claveBuscada) return resultado;
	
	if (*claveBuscada == *claveVieja) {	
	
		claveNueva->setReferencia(claveBuscada->getReferencia());
		
		//Si no hubo cambios en la clave no hago nada!
		if (*claveVieja == *claveNueva)
			resultado = ResultadosIndices::OK;
		else if (this->eliminar(claveVieja)) {
			if (this->insertar(claveNueva))
				resultado = ResultadosIndices::OK;
			else
				resultado = ResultadosIndices::CLAVE_DUPLICADA;
		}
	
	}
	
	delete claveBuscada;
	
	return resultado;
}


NodoBPlus* BPlusTree::buscarPadre(NodoBPlus* padre, NodoBPlus* hijo, Clave* claveNodoHijo) const {
	
	NodoBPlus *auxNodo = NULL, *nuevoNodo = NULL;
	Clave* claveOrientadora = NULL;

	if ( (!padre) || (!hijo) || (*hijo == *(this->nodoRaiz)) ) return NULL;
	
	if (padre->esPadre(hijo, claveOrientadora, claveNodoHijo)) {
		
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
    	
    	auxNodo = buscarPadre(nuevoNodo, hijo, claveNodoHijo);
    	
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
		
		if (nodo->getNivel() == 0) { //Nodo hoja
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
	
	//Actualizo nodoTarget
	indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);
	
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
	
	nodoIzquierdo->actualizarEspacioLibre();
	
	indiceManager.eliminarBloque(nodoDerecho->getPosicionEnArchivo());
	indiceManager.escribirBloque(nodoIzquierdo->getPosicionEnArchivo(), nodoIzquierdo);
	
//	delete nodoDerecho;
//	nodoDerecho = NULL;
	
}

bool BPlusTree::puedeMerge(NodoBPlus* nodoIzq, NodoBPlus* nodoDer, Clave* separador){

	SetClaves set;
	SetClaves::iterator iter;
	
	for (iter = nodoIzq->getClaves()->begin(); iter != nodoIzq->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	for (iter = nodoDer->getClaves()->begin(); iter != nodoDer->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	if (nodoIzq->getNivel() != 0) {
		set.insert(separador);
	}
	
	unsigned short i;
	iter = set.begin();
	
	for (i = 0; iter != set.end(); ++iter){
		i += (*iter)->getTamanioEnDisco(false);
	}
		
	bool puede = (i <= nodoIzq->getTamanioEspacioClaves());
	
	set.clear();
	return (puede);
}


void BPlusTree::pasarClaveHaciaIzquierda(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre, NodoBPlus* nodoHnoDer, Clave* clavePadre){
	
	char codigo;
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(false);
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
	
	//Se actualizan los nodos en disco
	indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	indiceManager.escribirBloque(nodoHnoDer->getPosicionEnArchivo(), nodoHnoDer);

}


void BPlusTree::pasarMaximoPosibleHaciaIzquierda(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre, NodoBPlus* nodoHnoDer, Clave* &clavePadre){

	char codigo;
	unsigned short bytesAEntregar;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(false);
	SetClaves* setIntercambio = NULL;
	Clave* clavePromocionada = NULL;
	
	bytesAEntregar = nodoHnoDer->obtenerBytesSobreMinimo();
	
	if (bytesAEntregar > 0){
		
		nodoPadre->extraerClave(clavePadre);
		
		if (nodoDestino->getNivel() == 0) {
		
			setIntercambio = nodoHnoDer->cederBytes(bytesAEntregar);
		
			clavePromocionada = nodoHnoDer->obtenerPrimeraClave()->copiar();
			
		} else {
			
			clavePadre->setHijoDer(nodoHnoDer->getHijoIzq());

			if ( tamanioClavePadre < bytesAEntregar )
				setIntercambio = nodoHnoDer->cederBytes(bytesAEntregar - tamanioClavePadre);
			
			clavePromocionada = nodoHnoDer->extraerPrimeraClave();
		
			nodoHnoDer->setHijoIzq(clavePromocionada->getHijoDer());
			
			nodoDestino->insertarClave(clavePadre, &codigo);
		}
			
		
		clavePromocionada->setHijoDer(nodoHnoDer->getPosicionEnArchivo());
	
		nodoPadre->insertarClave(clavePromocionada, &codigo);
		clavePadre = clavePromocionada;
			
		if ( setIntercambio ){
			nodoDestino->recibir(setIntercambio);
			delete setIntercambio;
		}
			
		//Se actualizan los nodos en disco
		indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
		indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
		indiceManager.escribirBloque(nodoHnoDer->getPosicionEnArchivo(), nodoHnoDer);
		
	}


}


void BPlusTree::pasarClaveHaciaDerecha(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre, NodoBPlus* nodoHnoIzq, Clave* clavePadre) {

	char codigo;
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(false);
	SetClaves* setIntercambio = NULL;
	Clave* clavePromocionada = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	
	if (nodoDestino->getNivel() == 0) {
	
		setIntercambio = nodoHnoIzq->cederBytes(bytesRequeridos, false);
		
		clavePromocionada = (*(setIntercambio->begin()))->copiar();
		
	} else {
	
		clavePadre->setHijoDer(nodoDestino->getHijoIzq());
		
		if ( tamanioClavePadre < bytesRequeridos) {
			setIntercambio = nodoHnoIzq->cederBytes(bytesRequeridos - tamanioClavePadre, false);
			clavePromocionada = *(setIntercambio->begin());
			setIntercambio->erase(setIntercambio->begin());
		}
		else 
			clavePromocionada = nodoHnoIzq->extraerUltimaClave();
		
		nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
		nodoDestino->insertarClave(clavePadre, &codigo);		
	}
		
	clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
	
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	if (setIntercambio) {
		nodoDestino->recibir(setIntercambio);
		delete setIntercambio;
	}

	//Se actualizan los nodos en disco
	indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	indiceManager.escribirBloque(nodoHnoIzq->getPosicionEnArchivo(), nodoHnoIzq);
	
}


void BPlusTree::pasarMaximoPosibleHaciaDerecha(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre, NodoBPlus* nodoHnoIzq, Clave* &clavePadre) {

	char codigo;
	unsigned short bytesAEntregar;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(false);
	SetClaves* setIntercambio = NULL;
	Clave* clavePromocionada = NULL;
	
	bytesAEntregar = nodoHnoIzq->obtenerBytesSobreMinimo(false);

	if (bytesAEntregar > 0){
	
		nodoPadre->extraerClave(clavePadre);
		
		if (nodoDestino->getNivel() == 0) {
		
			setIntercambio = nodoHnoIzq->cederBytes(bytesAEntregar, false);
			
			clavePromocionada = (*(setIntercambio->begin()))->copiar();

		} else {
		
			clavePadre->setHijoDer(nodoDestino->getHijoIzq());
			
			if ( tamanioClavePadre < bytesAEntregar ){
				setIntercambio = nodoHnoIzq->cederBytes(bytesAEntregar - tamanioClavePadre, false);
				clavePromocionada = *(setIntercambio->begin());
				setIntercambio->erase(setIntercambio->begin());
			}
			else clavePromocionada = nodoHnoIzq->extraerUltimaClave();
			
			nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
			nodoDestino->insertarClave(clavePadre, &codigo);

		}
				
		clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
		
		nodoPadre->insertarClave(clavePromocionada, &codigo);
		clavePadre = clavePromocionada;
		
		if (setIntercambio) {
			nodoDestino->recibir(setIntercambio);
			delete setIntercambio;
		}
							
		//Se actualizan los nodos en disco
		indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
		indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
		indiceManager.escribirBloque(nodoHnoIzq->getPosicionEnArchivo(), nodoHnoIzq);
		
	}

}

