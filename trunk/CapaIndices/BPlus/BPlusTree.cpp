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
	/*Lee el primer registro del archivo -> la raiz*/
	return new NodoBPlus(/*archivoIndice,*/0);
}


void BPlusTree::primero()
{
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
	//Se busca el nodo hoja donde se debe insertar la clave
	NodoBPlus* nodoDestino = buscarLugar(clave);
	char codigo;
	
	nodoDestino->insertarClave(clave, &codigo);
	
	//La clave queda insertada independientemente de si hay OVERFLOW o no.
	
	insertarInterno(nodoDestino, &codigo);
	
	delete nodoDestino;
}


void BPlusTree::insertarInterno(NodoBPlus* &nodoDestino, char* codigo) {
	
	if (*codigo == Codigo::MODIFICADO){
		//TODO actualizar en disco el nodo modificado.
		//archivoIndice->sobreescribirNodo(nodoDestino);
	}
	else if ( *codigo == Codigo::OVERFLOW ){
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBPlus* nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino);
		NodoBPlus *nodoHnoDer = NULL, *nodoHnoIzq = NULL;
		
		if (!nodoPadre){ //nodoDestino es la raiz.
			//La raíz no tiene tamaño mínimo, entonces a splitB le paso el tamaño mínimo de un nodo común.
			unsigned short tamanioMinimo = 2*(this->tamanioNodo - Nodo::getTamanioHeader())/3;
			SetClaves* setClavesDerecho = nodoDestino->splitB(tamanioMinimo);
			//Ahora nodoDestino deja de ser la raíz, por lo cual pasa a tener el tamaño de un nodo común.
			nodoDestino->setTamanio(this->tamanioNodo);
			Clave* clavePromocionada = *(setClavesDerecho->begin());
			setClavesDerecho->erase(clavePromocionada); //Extrae la clave del conj. No libera la memoria.
			NodoBPlus* nuevoNodoDerecho = new NodoBPlus(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), this->tamanioNodo);
			nuevoNodoDerecho->setClaves(setClavesDerecho);
			//TODO escribir 'nuevoNodoDerecho' en disco para obtener su posicion en el archivo.
			clavePromocionada->setHijoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			NodoBPlus* nuevaRaiz = new NodoBPlus(nodoDestino->getPosicionEnArchivo(), nodoDestino->getNivel() + 1, this->tamanioRaiz);
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
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoDer = new NodoBPlus( (*iterPadre)->getHijoDer() );
				clavePadreDer = *iterPadre;
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoDestino es el hijo derecho de la última clave del nodo
				//nodoDestino no tiene hermano derecho
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoIzq = new NodoBPlus( (*(--(--iterPadre)))->getHijoDer() );
				clavePadreIzq = *(++iterPadre);
			} else {
				if ( iterPadre == nodoPadre->getClaves()->begin() ) {
					//nodoHnoIzq = new NodoBPlus( nodoPadre->getHijoIzq() );
					clavePadreIzq = *iterPadre;
				}
				else{
					//nodoHnoIzq = new NodoBPlus( (*(--iterPadre))->getHijoDer() );
					clavePadreIzq = *(++iterPadre);
				}
				//nodoHnoDer = new NodoBPlus( (*iterPadre)->getHijoDer() );
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


bool BPlusTree::eliminar(Clave* clave) {
	
	NodoBPlus* nodoTarget = buscarLugar(clave);
	Clave* claveBuscada = nodoTarget->buscar(clave);
	
	char codigo;
	
	if ( (claveBuscada) && (*claveBuscada == *clave) ){
		if (nodoTarget->getNivel() != 0){ //Nodo interno
			NodoBPlus* nodoMenorDeLosMayores = this->buscarMenorMayores(nodoTarget, clave);
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
		NodoBPlus *nodoHnoDer = NULL, *nodoHnoIzq = NULL, *nodoHnoHno = NULL;
		
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
			//TODO Escritura especial de la raíz
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
				//TODO El constructor debe devolver un nodo a partir de una referencia a disco.
				//nodoHnoDer = new NodoBPlus( (*iterPadre)->getHijoDer() );
				clavePadreDer = *iterPadre;
				if ((++iterPadre) != nodoPadre->getClaves()->end()) {
					//nodoHnoHno = new NodoBPlus( (*iterPadre)->getHijoDer() );
					clavePadreIzq = *iterPadre;
				}
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
					if ((--iterPadre) == nodoPadre->getClaves()->begin()) {
						//nodoHnoHno = new NodoBPlus( nodoPadre->getHijoIzq() );
						clavePadreDer = *iterPadre;
					} else {
						//nodoHnoHno = new NodoBPlus( (*(--iterPadre))->getHijoDer() );
						clavePadreDer = *(++iterPadre);
					}
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
					nodoPadre->insertarClave(clavePromocionada, codigo);
					
					if (nodoPadre->tieneOverflow()) //Se resuelve posible overflow al insertar la clave promocionada
						this->insertarInterno(nodoPadre, codigo);
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


Clave* BPlusTree::buscar(Clave* clave) const {
	
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

