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
BPlusTree::BPlusTree(unsigned short tamanioNodo) {
	this->tamanioNodo = tamanioNodo;
	this->nodoRaiz = NULL;
	this->nodoActual = NULL;
	//this->archivoIndice = archivo;
}


BPlusTree::~BPlusTree() {
	if (this->nodoRaiz) delete nodoRaiz;
	if (this->nodoActual) delete nodoActual;
}


NodoBPlus* BPlusTree::getRaiz() const
{
	//Lee el primer registro del archivo -> la raiz
	//TODO Cargar un nodo desde disco!!
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
		//TODO Leer nodo de disco
		//this->nodoActual = new NodoBPlus(this->archivoIndice, refHijoIzq);
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
		//TODO actualizar en disco el nodo modificado.
		//archivoIndice->sobreescribirNodo(nodoDestino);
	}
	else if ( *codigo == Codigo::OVERFLOW ){
		NodoBPlus* nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
		
		if (!nodoPadre){ //nodoDestino es la raiz.
			SetClaves* setClavesDerecho = nodoDestino->splitB(nodoDestino->getTamanioMinimo());
			Clave* clavePromocionada = (*(setClavesDerecho->begin()))->copiar();
			NodoBPlus* nuevoNodoDerecho = new NodoBPlus(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), this->tamanioNodo);
			nuevoNodoDerecho->setClaves(setClavesDerecho);
			//TODO escribir 'nuevoNodoDerecho' en disco para obtener su posicion en el archivo.
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
		//TODO actualizar en disco el nodo modificado.
		//archivoIndice->sobreescribirNodo(nodoTarget);
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
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoDer = new NodoBPlus( (*iterPadre)->getHijoDer() );
				clavePadreDer = *iterPadre;
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoTarget es el hijo derecho de la última clave del nodo
				//nodoTarget no tiene hermano derecho
				if ( (--iterPadre) == nodoPadre->getClaves()->begin() ) {
					//nodoHnoIzq = new NodoBPlus( nodoPadre->getHijoIzq() );
					clavePadreIzq = *iterPadre;
				}
				else{
					//nodoHnoIzq = new NodoBPlus( (*(--iterPadre))->getHijoDer() );
					clavePadreIzq = *(++iterPadre);
				}	
			} else {
				--iterPadre; //Lo decremento xq se incrementó en el 'else if' anterior
				if ( iterPadre == nodoPadre->getClaves()->begin() ) {
					//nodoHnoIzq = new NodoBPlus( nodoPadre->getHijoIzq() );
					clavePadreIzq = *iterPadre;
				}
				else{
					//nodoHnoIzq = new NodoBPlus( (*(--iterPadre))->getHijoDer() );
					clavePadreIzq = *(++iterPadre);
				}
				//nodoHnoDer = new NodoBPlus( (*(++iterPadre))->getHijoDer() );
				clavePadreDer = *iterPadre;
			}
			
			//Fin de la búsqueda de los nodos hermanos de 'nodoTarget'
			
			//Se intenta hacer una redistribución de claves con el hermano derecho de 'nodoTarget'.
			//Si esto no es posible, se intenta hacer una redistribución con el hermano izquierdo.
			bool pudoRedistribuir = false;
			//TODO Implementar un método similar a puedePasarClaveHaciaXXX para las hojas del B+
			if (nodoHnoDer) {
				if ( pudoRedistribuir = nodoHnoDer->puedePasarClaveHaciaIzq(nodoTarget, nodoPadre))
					this->pasarClaveHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer);
				else if (nodoHnoIzq) {
					if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre))
						this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq);
				}
			} else { // tiene hermano izquierdo
				if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre))
					this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq);
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
		
		//auxNodo = new NodoBPlus(archivo, padre->getPosicionEnArchivo());
		
	} else {
		
    	if (claveOrientadora == NULL) {
    		//nuevoNodo = new NodoBPlus(archivo, padre->getHijoizq());
    	} else {
    		//nuevoNodo = new NodoBPlus(archivo, claveOrientadora->getHijoDer());
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
			//auxNodo = new NodoBPlus(archivo, nodo->getPosicionEnArchivo());
		} else {
			//nuevoNodo = new NodoBPlus(archivo, nodo->getHijoIzq());
			auxNodo = buscarLugarRecursivo(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	} else {
		
		if (claveResultante->getHijoDer() == 0) {//Nodo hoja
			//auxNodo = new NodoBPlus(archivo, nodo->getPosicionEnArchivo());
		} else {
			//nuevoNodo = new NodoBPlus(archivo, claveResultante->getHijoDer());
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
		//TODO Escribir a disco el nuevo nodo!! nico puto
		nodoTarget->setHnoDer(nuevoNodo->getPosicionEnArchivo());
		clavePromocionada = (*(clavesMayores->begin()))->copiar();
	}
	else{
		clavePromocionada = (*(clavesMayores->begin()));
		clavesMayores->erase(clavesMayores->begin());
		nuevoNodo = new NodoBPlus(clavePromocionada->getHijoDer(), nodoTarget->getNivel(), nodoTarget->getTamanio());
		nuevoNodo->setClaves(clavesMayores);
		//TODO Escribir a disco el nuevo nodo!! nico reputo
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
	
	//TODO borrar de disco al nodoDerecho. NiCO3
	//TODO actualizar en disco al nodoIzquierdo, que ya no debería tener más underflow, je.
	
	delete nodoDerecho;
	nodoDerecho = NULL;
	
}


void BPlusTree::pasarClaveHaciaIzquierda(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre,
										 NodoBPlus* nodoHnoDer){
	
	if (nodoDestino->getNivel() == 0) {
		SetClaves* setIntercambio = nodoHnoDer->cederBytes( nodoDestino->obtenerBytesRequeridos() );
		nodoDestino->recibir(setIntercambio);
		delete setIntercambio;
		//TODO Actualizar el separador en el nodo padre y chequear underflow y overflow!
	} else {
		SetClaves* setDescenso = nodoPadre->cederBytes( nodoDestino->obtenerBytesRequeridos() );
		unsigned short cantClaves = setDescenso->size();
		SetClaves* setAscenso = nodoHnoDer->cederClaves(cantClaves);
		unsigned int nuevoHijoIzq = 0;
		
		for(SetClaves::iterator iterDescenso = setDescenso->begin(), iterAscenso = setAscenso->begin();
			(iterDescenso != setDescenso->end()) && (iterAscenso != setAscenso->end());
			++iterDescenso, ++iterAscenso ) {
			
			nuevoHijoIzq = (*iterAscenso)->getHijoDer();
			(*iterAscenso)->setHijoDer((*iterDescenso)->getHijoDer());
			(*iterDescenso)->setHijoDer(nodoHnoDer->getHijoIzq());
			nodoHnoDer->setHijoIzq(nuevoHijoIzq);
			
		}
		
		nodoDestino->recibir(setDescenso);
		nodoPadre->recibir(setAscenso);
		
		delete setDescenso;
		delete setAscenso;
	}
	
}


void BPlusTree::pasarClaveHaciaDerecha(NodoBPlus* nodoDestino, NodoBPlus* nodoPadre,
									   NodoBPlus* nodoHnoIzq) {
	
	SetClaves* setDescenso = nodoPadre->cederBytes( nodoDestino->obtenerBytesRequeridos(), false );
	unsigned short cantClaves = setDescenso->size();
	SetClaves* setAscenso = nodoHnoIzq->cederClaves(cantClaves, false);
	unsigned int nuevoHijoIzq = 0;
	
	for(SetClaves::iterator iterDescenso = setDescenso->begin(), iterAscenso = setAscenso->begin();
		(iterDescenso != setDescenso->end()) && (iterAscenso != setAscenso->end());
		++iterDescenso, ++iterAscenso ) {
		
		nuevoHijoIzq = (*iterAscenso)->getHijoDer();
		(*iterAscenso)->setHijoDer((*iterDescenso)->getHijoDer());
		(*iterDescenso)->setHijoDer(nodoDestino->getHijoIzq());
		nodoDestino->setHijoIzq(nuevoHijoIzq);
		
	}
	
	nodoDestino->recibir(setDescenso);
	nodoPadre->recibir(setAscenso);
	
	delete setDescenso;
	delete setAscenso;
	
}
