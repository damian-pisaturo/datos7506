#include "BStarTree.h"


BStarTree::BStarTree(IndiceManager& indiceManager, unsigned short tamanioNodo)
	: BTree(indiceManager, tamanioNodo) {
	
	this->tamanioRaiz = 2*this->tamanioNodo;
	this->claveCorriente = NULL;
	this->getRaiz();
	
}

BStarTree::~BStarTree() {
	if (this->nodoRaiz) delete nodoRaiz;
	if (this->claveCorriente) delete this->claveCorriente;
}

NodoBStar* BStarTree::getRaiz()
{
	//Lee el primer registro del archivo -> la raiz
	this->nodoRaiz = new NodoBStar(0, 0, this->tamanioRaiz);
	
	int resultado = indiceManager.leerBloqueDoble(0, this->nodoRaiz);
	
	if (resultado != ResultadosFisica::OK) this->nodoRaiz = NULL;
	
	return this->nodoRaiz;
}

bool BStarTree::insertar(Clave* clave) {
	
	if (!clave) return false;
	
	if (this->vacio()) {
		
		this->nodoRaiz = new NodoBStar(0, 0, clave, this->tamanioRaiz);
		
		indiceManager.escribirBloqueDoble(this->nodoRaiz);
		
	} else {

		NodoBStar* nodoDestino = buscarLugar(clave);
	
		//si nodoDestino == NULL significa que la clave ya esta insertada
		if (!nodoDestino) return false;
		
		char codigo;
		
		nodoDestino->insertarClave(clave, &codigo);

		//La clave queda insertada independientemente de si hay OVERFLOW o no.
		insertarInterno(nodoDestino, &codigo, clave);

		if (*nodoDestino == *(this->nodoRaiz))
			*(this->nodoRaiz) = *nodoDestino;
		
		delete nodoDestino;
		
	}
	
	return true;

}


void BStarTree::insertarInterno(NodoBStar* &nodoDestino, char* codigo, Clave* claveInsertada) {

	if (*codigo == Codigo::MODIFICADO){
		
		if (*nodoDestino == *(this->nodoRaiz)) {
			
			//Se verifica si la raíz se debe splittear
			if (nodoDestino->getTamanioEnDiscoSetClaves() > nodoDestino->getTamanioEspacioClaves()) {
				
				*codigo = Codigo::OVERFLOW;
				
				this->insertarInterno(nodoDestino, codigo, claveInsertada);
				
			} else indiceManager.escribirBloqueDoble(nodoDestino->getPosicionEnArchivo(), nodoDestino);
			
		} else indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	}
	else if ( *codigo == Codigo::OVERFLOW ){
		
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoDestino y su hermano der
		Clave* clavePadreDer = NULL;
		NodoBStar* nodoPadre = this->buscarPadre(this->nodoRaiz, nodoDestino, claveInsertada);
		NodoBStar *nodoHnoDer = NULL, *nodoHnoIzq = NULL;
		
		if (!nodoPadre){ //nodoDestino es la raiz
			
			//A la raíz le setteo el tamaño de un nodo común porque se tiene que splittear en nodos comunes.
			nodoDestino->setTamanio(this->tamanioNodo);
			nodoDestino->setPosicionEnArchivo(1); //Le setteo una posición distinta de 0 para que no siga funcionando como la raiz
			
			SetClaves* setClavesDerecho = nodoDestino->splitB(nodoDestino->getTamanioMinimo());
			
			Clave* clavePromocionada = *(setClavesDerecho->begin());
			setClavesDerecho->erase(clavePromocionada); //Extrae la clave del conj. No libera la memoria.
			
			NodoBStar* nuevoNodoDerecho = new NodoBStar(clavePromocionada->getHijoDer(), nodoDestino->getNivel(), this->tamanioNodo);
			nuevoNodoDerecho->setClaves(setClavesDerecho);
			
			indiceManager.escribirBloque(nuevoNodoDerecho);
			//Se actualiza nodoDestino
			indiceManager.escribirBloque(nodoDestino); //Busca una nueva posicion para el nodoDestino (hijo izq de la raiz)
			
			clavePromocionada->setHijoDer(nuevoNodoDerecho->getPosicionEnArchivo());
			NodoBStar* nuevaRaiz = new NodoBStar(nodoDestino->getPosicionEnArchivo(), nodoDestino->getNivel() + 1, clavePromocionada, this->tamanioRaiz);
			nuevaRaiz->setPosicionEnArchivo(0);
			indiceManager.escribirBloqueDoble(0, nuevaRaiz);
			
			*(this->nodoRaiz) = *nuevaRaiz;
			
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
				nodoHnoDer = new NodoBStar(0, 0, this->tamanioNodo);
				indiceManager.leerBloque((*iterPadre)->getHijoDer(), nodoHnoDer);
				clavePadreDer = *iterPadre;
			} else if ( (++iterPadre) == nodoPadre->getClaves()->end() ) {
				//nodoDestino es el hijo derecho de la última clave del nodo
				//nodoDestino no tiene hermano derecho
				nodoHnoIzq = new NodoBStar(0, 0, this->tamanioNodo);
				if ((--iterPadre) == nodoPadre->getClaves()->begin()) {
					indiceManager.leerBloque(nodoPadre->getHijoIzq(), nodoHnoIzq);
					clavePadreIzq = *iterPadre;
				} else {
					indiceManager.leerBloque((*(--iterPadre))->getHijoDer(), nodoHnoIzq);
					clavePadreIzq = *(++iterPadre);
				}
			} else {
				
				if ( (--iterPadre) == nodoPadre->getClaves()->begin() ) {
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
					nodoPadre->extraerClave(clavePadreDer);
					vectorClaves = mergeSplitOverflow(nodoHnoDer, nodoDestino, clavePadreDer);
				} else {
					nodoPadre->extraerClave(clavePadreIzq);
					vectorClaves = mergeSplitOverflow(nodoDestino, nodoHnoIzq, clavePadreIzq);
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
				
				//Llamada recursiva para chequear overflow en el padre. Si no hay overflow, lo escribe en disco.
				insertarInterno(nodoPadre, codigo, claveInsertada);
			}
			
			if (*nodoPadre == *(this->nodoRaiz))
				*(this->nodoRaiz) = *nodoPadre;
			
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

	
	char codigoInsercion;
	char codigoEliminacion;
	
	if ( (claveBuscada) && (*claveBuscada == *clave) ){
		if (nodoTarget->getNivel() != 0){ //Nodo interno
			
			NodoBStar* nodoMenorDeLosMayores = this->buscarMenorMayores(nodoTarget, clave);
			Clave* primeraClave = nodoMenorDeLosMayores->obtenerPrimeraClave();
			if (!primeraClave){
				nodoMenorDeLosMayores = this->buscarMayorMenores(nodoTarget, clave);
				primeraClave = nodoMenorDeLosMayores->obtenerUltimaClave();
			}
			
			Clave* claveMenorDeLasMayores = primeraClave->copiar();
			nodoMenorDeLosMayores->eliminarClave(claveMenorDeLasMayores, &codigoEliminacion);
			claveMenorDeLasMayores->setHijoDer(claveBuscada->getHijoDer());
			nodoTarget->eliminarClave(claveBuscada, &codigoInsercion);
			nodoTarget->insertarClave(claveMenorDeLasMayores, &codigoInsercion);
			
			//Se resuelve el overflow en nodoTarget (en caso que haya).
			//Este método también escribe el nodo en disco.
			this->insertarInterno(nodoTarget, &codigoInsercion, claveMenorDeLasMayores);
			
			if ((this->nodoRaiz) && (*nodoTarget == *(this->nodoRaiz)))
				*(this->nodoRaiz) = *nodoTarget;
			
			//Se resuelve el underflow en nodoMenorDeLosMayores (en caso que haya).
			//Este método también escribe el nodo en disco.
			this->eliminarInterno(nodoMenorDeLosMayores, &codigoEliminacion, claveMenorDeLasMayores);
			delete nodoMenorDeLosMayores;
			
		}
		else { //NodoTarget es un nodo hoja
			nodoTarget->eliminarClave(claveBuscada, &codigoEliminacion);
			this->eliminarInterno(nodoTarget, &codigoEliminacion,clave); //resuelve underflow y escribe en disco
		
			if ((this->nodoRaiz) && (nodoTarget) && (*nodoTarget == *(this->nodoRaiz)))
				*(this->nodoRaiz) = *nodoTarget;

		}
	}
	else{
		if (nodoTarget) delete nodoTarget;
		return false;
	}
	
	
	if (nodoTarget) delete nodoTarget;
	

	return true;
}


void BStarTree::eliminarInterno(NodoBStar* &nodoTarget, char* codigo, Clave* claveEliminada) {

	if (*codigo == Codigo::MODIFICADO) {
		//Se actualizar en disco el nodo modificado.
		if (*nodoTarget == *(this->nodoRaiz))
			indiceManager.escribirBloqueDoble(nodoTarget->getPosicionEnArchivo(), nodoTarget);
		else
			indiceManager.escribirBloque(nodoTarget->getPosicionEnArchivo(), nodoTarget);
		
	} else if (*codigo == Codigo::UNDERFLOW) {
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano izq
		Clave* clavePadreIzq = NULL;
		//Puntero a la clave del nodo padre que se encuentra entre nodoTarget y su hermano der
		Clave* clavePadreDer = NULL;

		NodoBStar *nodoPadre = this->buscarPadre(this->nodoRaiz, nodoTarget, claveEliminada);
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
				indiceManager.eliminarBloqueDoble(nodoTarget->getPosicionEnArchivo());
				delete nodoRaiz;
				this->nodoRaiz = NULL;
			} else {
				indiceManager.escribirBloqueDoble(nodoTarget->getPosicionEnArchivo(), nodoTarget);
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
						
						if (!pudoRedistribuir)
							this->pasarMaximoPosibleHaciaIzquierda(nodoHnoDer, nodoPadre, nodoHnoHno, clavePadreIzq);
					}
				} else {
					if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
						this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
				}
				
				if (!pudoRedistribuir)
					this->pasarMaximoPosibleHaciaIzquierda(nodoTarget, nodoPadre, nodoHnoDer, clavePadreDer);
		
			} else { // tiene hermano izquierdo
				if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
					this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
				else { //'nodoTarget' no tiene hermano derecho
					//'nodoHnoHno' debe cederle a 'nodoHnoIzq' y luego 'nodoHnoIzq' debe cederle a 'nodoTarget'
					if ( (nodoHnoHno) && (nodoHnoHno->puedePasarClaveHaciaDer(nodoHnoIzq, nodoPadre, clavePadreDer)) ) {
						this->pasarClaveHaciaDerecha(nodoHnoIzq, nodoPadre, nodoHnoHno, clavePadreDer);
						if ( pudoRedistribuir = nodoHnoIzq->puedePasarClaveHaciaDer(nodoTarget, nodoPadre, clavePadreIzq))
							this->pasarClaveHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
						
						if (!pudoRedistribuir)
							this->pasarMaximoPosibleHaciaDerecha(nodoHnoIzq, nodoPadre, nodoHnoHno, clavePadreDer);
					}
					
				}
				if (!pudoRedistribuir)						
					this->pasarMaximoPosibleHaciaDerecha(nodoTarget, nodoPadre, nodoHnoIzq, clavePadreIzq);
									
			}
			
			if (!pudoRedistribuir) { //Se realiza la concatenación de 'nodoTarget' con dos nodos hermanos y dos claves
				//del nodo padre. Luego se realiza un split, generándose dos nodos y una clave a promocionar
				//hacia el nodo padre.
				Clave* clavePromocionada = NULL;
				if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoHnoDer) && (!nodoHnoIzq) && (!nodoHnoHno) ) {
					if (puedeMerge(nodoTarget, nodoHnoDer, nodoPadre))
						this->merge(nodoTarget, nodoHnoDer, nodoPadre);
					else{
						*codigo = Codigo::MODIFICADO;
						this->eliminarInterno(nodoTarget, codigo, claveEliminada);
					}
						
				}
				else if ( (*nodoPadre == *(this->nodoRaiz)) && (nodoHnoIzq) && (!nodoHnoDer) && (!nodoHnoHno) ){
					if (puedeMerge(nodoHnoIzq, nodoTarget, nodoPadre)){
						this->merge(nodoHnoIzq, nodoTarget, nodoPadre);
					}
					else{
						*codigo = Codigo::MODIFICADO;
						this->eliminarInterno(nodoTarget, codigo, claveEliminada);
					}
				}
				else {
					bool pudoRealizarMerge = false;
					if ((nodoHnoIzq) && (nodoHnoDer)){
						if (pudoRealizarMerge = this->puedeMergeSplitUnderflow(nodoHnoIzq, nodoTarget, nodoHnoDer, clavePadreIzq, clavePadreDer))
							clavePromocionada = this->mergeSplitUnderflow(nodoHnoIzq, nodoTarget, nodoHnoDer, clavePadreIzq, clavePadreDer);
					}
					
					else if (!nodoHnoIzq){
						if (nodoHnoHno){
							if (pudoRealizarMerge = this->puedeMergeSplitUnderflow(nodoTarget, nodoHnoDer, nodoHnoHno, clavePadreDer, clavePadreIzq))
								clavePromocionada = this->mergeSplitUnderflow(nodoTarget, nodoHnoDer, nodoHnoHno, clavePadreDer, clavePadreIzq);
						}
					}
					else if (nodoHnoHno){
						if (pudoRealizarMerge = this->puedeMergeSplitUnderflow(nodoHnoHno, nodoHnoIzq, nodoTarget, clavePadreDer, clavePadreIzq))
							clavePromocionada = this->mergeSplitUnderflow(nodoHnoHno, nodoHnoIzq, nodoTarget, clavePadreDer, clavePadreIzq);
					}
					
					if (pudoRealizarMerge){
						nodoPadre->eliminarClave(clavePadreIzq, codigo);
						nodoPadre->eliminarClave(clavePadreDer, codigo);
						char codigoInsertar;
						nodoPadre->insertarClave(clavePromocionada, &codigoInsertar);
					
						if (nodoPadre->tieneOverflow()) //Se resuelve posible overflow al insertar la clave promocionada
							this->insertarInterno(nodoPadre, &codigoInsertar, clavePromocionada);
						//Sino, se resuelve posible underflow al eliminar las claves separadoras. Si no hay underflow,
						//este método se encargará de escribir nodoPadre en disco.
						else
							this->eliminarInterno(nodoPadre, codigo, claveEliminada);
						
					}
					else{
						*codigo = Codigo::MODIFICADO;
						this->eliminarInterno(nodoTarget, codigo, claveEliminada);
					}
					
				}
			}
			
			if (nodoHnoDer) delete nodoHnoDer;
			if (nodoHnoIzq) delete nodoHnoIzq;
			if (nodoHnoHno) delete nodoHnoHno;
			
			if ( (nodoPadre) && (*nodoPadre == *(this->nodoRaiz)) )
				*(this->nodoRaiz) = *nodoPadre;
			
			if (nodoPadre) delete nodoPadre;	
		}
		
	}
	
}


Clave* BStarTree::buscar(Clave* clave) const {
	
	if ( (!clave) || (this->vacio()) ) return NULL;
	
	NodoBStar* nodo = this->buscarClave(this->nodoRaiz, clave);
	
	if (!nodo) return NULL;
	
	SetClaves::iterator iter = nodo->getClaves()->find(clave);
	
	Clave* claveBuscada = NULL;
		
	if (iter != nodo->getClaves()->end())
		claveBuscada = (*iter)->copiar();
	
	delete nodo;
	
	return claveBuscada;
	
}

		
int BStarTree::modificar(Clave* claveVieja, Clave* claveNueva) {
	
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


void BStarTree::pasarClaveHaciaIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer, Clave* &clavePadre){
	
	char codigo;
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(true);
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
	
	//Se actualizan los nodos
	indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	indiceManager.escribirBloque(nodoHnoDer->getPosicionEnArchivo(), nodoHnoDer);
	
	if (*nodoPadre == *(this->nodoRaiz))
		indiceManager.escribirBloqueDoble(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	else
		indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	
	clavePadre = clavePromocionada;

}


void BStarTree::pasarMaximoPosibleHaciaIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer, Clave* &clavePadre){
	
	char codigo;
	unsigned short bytesAEntregar;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(true);
	SetClaves* setIntercambio = NULL;
	
	if (nodoHnoDer->getTamanioEnDiscoSetClaves() > nodoHnoDer->getTamanioMinimo()){
		bytesAEntregar = nodoHnoDer->getTamanioEnDiscoSetClaves() - nodoHnoDer->getTamanioMinimo();
	
		nodoPadre->extraerClave(clavePadre);
		clavePadre->setHijoDer(nodoHnoDer->getHijoIzq());
		
		if ( tamanioClavePadre < bytesAEntregar){
			setIntercambio = nodoHnoDer->cederBytes(bytesAEntregar - tamanioClavePadre);
			nodoDestino->recibir(setIntercambio);
			delete setIntercambio;
		}
		
		Clave* clavePromocionada = nodoHnoDer->extraerPrimeraClave();
		nodoHnoDer->setHijoIzq(clavePromocionada->getHijoDer());
		clavePromocionada->setHijoDer(nodoHnoDer->getPosicionEnArchivo());
		nodoPadre->insertarClave(clavePromocionada, &codigo);
		
		nodoDestino->insertarClave(clavePadre, &codigo);
		
		//Se actualizan los nodos
		indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
		indiceManager.escribirBloque(nodoHnoDer->getPosicionEnArchivo(), nodoHnoDer);
		
		if (*nodoPadre == *(this->nodoRaiz))
			indiceManager.escribirBloqueDoble(nodoPadre->getPosicionEnArchivo(), nodoPadre);
		else
			indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
		
		clavePadre = clavePromocionada;
		
	}

}


void BStarTree::pasarClaveHaciaDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq, Clave* &clavePadre) {
	
	char codigo;
	
	unsigned short bytesRequeridos = nodoDestino->obtenerBytesRequeridos();
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(true);
	SetClaves* setIntercambio = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	clavePadre->setHijoDer(nodoDestino->getHijoIzq());
	
	if ( tamanioClavePadre < bytesRequeridos){
		setIntercambio = nodoHnoIzq->cederBytes(bytesRequeridos - tamanioClavePadre, false);
		nodoDestino->recibir(setIntercambio);
		delete setIntercambio;
	}
	
	Clave* clavePromocionada = nodoHnoIzq->extraerUltimaClave();
	nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
	clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	nodoDestino->insertarClave(clavePadre, &codigo);
	
	//Se actualizan los nodos
	indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	indiceManager.escribirBloque(nodoHnoIzq->getPosicionEnArchivo(), nodoHnoIzq);
	
	if (*nodoPadre == *(this->nodoRaiz))
		indiceManager.escribirBloqueDoble(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	else
		indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	
	clavePadre = clavePromocionada;

}


void BStarTree::pasarMaximoPosibleHaciaDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq, Clave* &clavePadre) {
	
	char codigo;
	
	unsigned short bytesAEntregar;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(true);
	SetClaves* setIntercambio = NULL;
	
	if (nodoHnoIzq->getTamanioEnDiscoSetClaves() > nodoHnoIzq->getTamanioMinimo()){
		bytesAEntregar = nodoHnoIzq->getTamanioEnDiscoSetClaves() - nodoHnoIzq->getTamanioMinimo();
			
		nodoPadre->extraerClave(clavePadre);
		clavePadre->setHijoDer(nodoDestino->getHijoIzq());
		
		if ( tamanioClavePadre < bytesAEntregar){
			setIntercambio = nodoHnoIzq->cederBytes(bytesAEntregar - tamanioClavePadre, false);
			nodoDestino->recibir(setIntercambio);
			delete setIntercambio;
		}
		
		Clave* clavePromocionada = nodoHnoIzq->extraerUltimaClave();
		nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
		clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
		nodoPadre->insertarClave(clavePromocionada, &codigo);
		
		nodoDestino->insertarClave(clavePadre, &codigo);
		
		//Se actualizan los nodos
		indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
		indiceManager.escribirBloque(nodoHnoIzq->getPosicionEnArchivo(), nodoHnoIzq);
		
		if (*nodoPadre == *(this->nodoRaiz))
			indiceManager.escribirBloqueDoble(nodoPadre->getPosicionEnArchivo(), nodoPadre);
		else
			indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);
		
		clavePadre = clavePromocionada;
	}

}


void BStarTree::recibirClaveDesdeDerecha(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoDer, Clave* &clavePadre){
	
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
	
	//Se actualizan los nodos en disco
	indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	indiceManager.escribirBloque(nodoHnoDer->getPosicionEnArchivo(), nodoHnoDer);
	
	if (*nodoPadre == *(this->nodoRaiz))
		indiceManager.escribirBloqueDoble(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	else
		indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);

	clavePadre = clavePromocionada;
}


void BStarTree::recibirClaveDesdeIzquierda(NodoBStar* nodoDestino, NodoBStar* nodoPadre, NodoBStar* nodoHnoIzq, Clave* &clavePadre){

	char codigo;
	unsigned short cantClavesSobrantes;
	unsigned short bytesSobrantes = nodoHnoIzq->obtenerBytesSobrantes(cantClavesSobrantes, false);
	SetClaves* setIntercambio = NULL;
	
	nodoPadre->extraerClave(clavePadre);
	clavePadre->setHijoDer(nodoDestino->getHijoIzq());
	
	setIntercambio = nodoHnoIzq->cederBytes(bytesSobrantes, false);

	Clave* clavePromocionada = *(setIntercambio->begin());

	setIntercambio->erase(setIntercambio->begin());

	nodoDestino->setHijoIzq(clavePromocionada->getHijoDer());
	clavePromocionada->setHijoDer(nodoDestino->getPosicionEnArchivo());
	nodoPadre->insertarClave(clavePromocionada, &codigo);
	
	nodoDestino->recibir(setIntercambio);
	nodoDestino->insertarClave(clavePadre, &codigo);

	delete setIntercambio;
	
	//Se actualizan los nodos en disco
	indiceManager.escribirBloque(nodoDestino->getPosicionEnArchivo(), nodoDestino);
	indiceManager.escribirBloque(nodoHnoIzq->getPosicionEnArchivo(), nodoHnoIzq);
	
	if (*nodoPadre == *(this->nodoRaiz))
		indiceManager.escribirBloqueDoble(nodoPadre->getPosicionEnArchivo(), nodoPadre);
	else
		indiceManager.escribirBloque(nodoPadre->getPosicionEnArchivo(), nodoPadre);

	clavePadre = clavePromocionada;
	
}


NodoBStar* BStarTree::buscarPadre(NodoBStar* padre, NodoBStar* hijo, Clave* claveNodoHijo) const {

	NodoBStar *auxNodo = NULL, *nuevoNodo = NULL;
	Clave* claveOrientadora = NULL;

	if ( (!padre) || (!hijo) || (*hijo == *(this->nodoRaiz)) ) return NULL;
	
	if (padre->esPadre(hijo, claveOrientadora, claveNodoHijo)) {
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
    	
    	auxNodo = buscarPadre(nuevoNodo, hijo, claveNodoHijo);
    	
    	delete nuevoNodo;
    	
    }

	return auxNodo;
	
}


NodoBStar* BStarTree::buscarClave(NodoBStar* nodo, Clave* clave) const {
	
	NodoBStar *nuevoNodo = NULL, *auxNodo = NULL;
		
	Clave* claveResultante = nodo->buscar(clave);

	if (claveResultante && (*claveResultante == *clave))
		return (NodoBStar*)nodo->copiar();
	
	if (claveResultante == NULL) {
		
		if (nodo->getNivel() == 0) { //Nodo hoja
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
		
		if (nodo->getNivel() == 0) { //Nodo hoja
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
		
		if (*claveResultante == *clave) return NULL; //La clave ya estaba insertada
		
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
	
	NodoBStar *nuevoNodo = NULL, *auxNodo = NULL;
	
	Clave* claveResultante = nodo->buscar(clave);
		
	if (claveResultante == NULL) {
		
		if (nodo->getNivel() == 0) { //Nodo hoja
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
NodoBStar* BStarTree::buscarMayorMenores(NodoBStar* nodo, Clave* clave) const {
	
	NodoBStar *nuevoNodo = NULL, *auxNodo = NULL;
	
	Clave* claveResultante = nodo->getClaves()->findClaveAnterior(clave);
		
	if (claveResultante == NULL) {
		
		if (nodo->getNivel() == 0) { //Nodo hoja
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




VectorClaves* BStarTree::mergeSplitOverflow(NodoBStar* nodoTarget, NodoBStar* nodoHno, Clave* clavePadre){
	
	clavePadre->setHijoDer(/*nodoHno->getHijoIzq()*/nodoTarget->getHijoIzq()); //TODO: EN BETA.
	
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
	
	(*vectorClaves)[0] = ultimaClaveNuevoNodo;   
	(*vectorClaves)[1] = ultimaClaveNodoHno;    
	//Agrego una tercera clave que sólo se utiliza para guardar la referencia
	//en disco al nuevoNodo.
	(*vectorClaves)[2] = new ClaveEntera(0,0,nuevoNodo->getPosicionEnArchivo());  
	
	
	delete nuevoNodo;

	return vectorClaves;
}

bool BStarTree::puedeMerge(NodoBStar* nodoHijoIzq, NodoBStar* nodoHijoDer, NodoBStar* nodoPadre){
	SetClaves set;
	SetClaves::iterator iter;
	
	for (iter = nodoHijoIzq->getClaves()->begin(); iter != nodoHijoIzq->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	for (iter = nodoHijoDer->getClaves()->begin(); iter != nodoHijoDer->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	for (iter = nodoPadre->getClaves()->begin(); iter != nodoPadre->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	unsigned short i;
	iter = set.begin();
	
	for (i = 0; iter != set.end(); ++iter){
		i += (*iter)->getTamanioEnDisco(true);
	}
	
	i -= Tamanios::TAMANIO_REFERENCIA*nodoPadre->getClaves()->size();
		
	bool puede = (i <= nodoPadre->getTamanioEspacioClaves());
	
	set.clear();
	
	return (puede);

}

//Método utilizado para mergear la raíz con dos hijos
void BStarTree::merge(NodoBStar* &nodoHijoIzq, NodoBStar* &nodoHijoDer, NodoBStar* nodoPadre) {

	nodoPadre->setHijoIzq(nodoHijoIzq->getHijoIzq());
	nodoPadre->obtenerPrimeraClave()->setHijoDer(nodoHijoDer->getHijoIzq());
	nodoPadre->setNivel(nodoPadre->getNivel() - 1);
	
	nodoPadre->merge(nodoHijoIzq, nodoHijoDer, NULL, NULL);
	nodoPadre->actualizarEspacioLibre();
	
	indiceManager.escribirBloqueDoble(nodoPadre->getPosicionEnArchivo(), nodoPadre);

	//Se elimina de disco a los nodos nodoHijoDer y nodoHijoIzq
	indiceManager.eliminarBloque(nodoHijoIzq->getPosicionEnArchivo());
	indiceManager.eliminarBloque(nodoHijoDer->getPosicionEnArchivo());
	
	delete nodoHijoIzq;
	nodoHijoIzq = NULL;
	
	delete nodoHijoDer;
	nodoHijoDer = NULL;

}


bool BStarTree::puedeMergeSplitUnderflow(NodoBStar* nodoTarget, NodoBStar* nodoHno1, NodoBStar* nodoHno2,
										 Clave* clavePadre1, Clave* clavePadre2) {	
	
	SetClaves set;
	SetClaves::iterator iter;
	
	for (iter = nodoTarget->getClaves()->begin(); iter != nodoTarget->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	for (iter = nodoHno1->getClaves()->begin(); iter != nodoHno1->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	for (iter = nodoHno2->getClaves()->begin(); iter != nodoHno2->getClaves()->end(); ++iter)
		set.insert(*iter);
	
	set.insert(clavePadre1);
	set.insert(clavePadre2);
	
	unsigned short i = 0;
	unsigned short tamanioEspacioClaves = nodoTarget->getTamanioEspacioClaves();
	iter = set.begin();
	
	for(i = 0; (iter != set.end()) && (i < tamanioEspacioClaves); ++iter){
		i += (*iter)->getTamanioEnDisco(true);
		if ( ( (*(*iter) == *clavePadre1) || (*(*iter) == *clavePadre2) )  && (nodoTarget->getNivel() == 0) )
			i -= Tamanios::TAMANIO_REFERENCIA;
	}
	
	if (i <= tamanioEspacioClaves) ++iter;
	
	for (i = 0; iter != set.end(); ++iter){
		i += (*iter)->getTamanioEnDisco(true);
		if ( ( (*(*iter) == *clavePadre1) || (*(*iter) == *clavePadre2) )  && (nodoTarget->getNivel() == 0) )
			i -= Tamanios::TAMANIO_REFERENCIA;
	}
	
	bool puede = (i <= tamanioEspacioClaves);
	
	set.clear();

	return (puede);

	
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

void BStarTree::primero(){
	if (this->claveCorriente) delete this->claveCorriente;
	this->claveCorriente = buscarPrimero(this->nodoRaiz);
}


Clave* BStarTree::siguiente(){
	Clave* auxClave = NULL;
	if (this->claveCorriente){
		auxClave = this->claveCorriente->copiar();
		delete this->claveCorriente;
		this->claveCorriente = buscarSiguiente(this->nodoRaiz, auxClave);
	}
	return auxClave;
}

Clave* BStarTree::buscarPrimero(NodoBStar* nodo) const {
	
	NodoBStar *nuevoNodo = NULL;
	Clave* auxClave = NULL;
		
	if (nodo->getNivel() == 0) {//Nodo hoja
		auxClave = nodo->obtenerPrimeraClave();
		if (auxClave) return auxClave->copiar();
		else return NULL;
	}
	else {
		nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
		indiceManager.leerBloque(nodo->getHijoIzq(), nuevoNodo);
		auxClave = buscarPrimero(nuevoNodo);
		delete nuevoNodo;
	}
	
	return auxClave;
	
}

Clave* BStarTree::buscarSiguiente(NodoBStar* nodo, Clave* clave) const {

	NodoBStar* nuevoNodo = NULL;
		
	Clave* claveResultante = nodo->buscar(clave);
	Clave* auxClave = NULL;
	
	if (claveResultante && (*claveResultante == *clave)){
	
		if (nodo->getNivel() == 0){
			auxClave = nodo->getClaves()->findClaveSiguiente(claveResultante);
			if (!auxClave) return NULL;
			return auxClave->copiar();
		}
		else{
			nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
			
			if (claveResultante == NULL) indiceManager.leerBloque(nodo->getHijoIzq(), nuevoNodo);
			else indiceManager.leerBloque(claveResultante->getHijoDer(), nuevoNodo);
			
			auxClave = buscarPrimero(nuevoNodo);
			delete nuevoNodo;
			return auxClave;
		}

	}
	
	if (claveResultante == NULL) {

		if (nodo->getNivel() == 0) { //Nodo hoja
			return NULL;
		} else {
			nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(nodo->getHijoIzq(), nuevoNodo);
			auxClave = buscarSiguiente(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	} else {
			
		if (claveResultante->getHijoDer() == 0) {//Nodo hoja
			return NULL;
		} else {
			nuevoNodo = new NodoBStar(0, 0, this->tamanioNodo);
			indiceManager.leerBloque(claveResultante->getHijoDer(), nuevoNodo);
			auxClave = buscarSiguiente(nuevoNodo, clave);
			delete nuevoNodo;
		}
		
	}
	
	if (!auxClave){
		auxClave = nodo->getClaves()->findClaveSiguiente(clave);
		if (auxClave) return auxClave->copiar();
	}
	
	return auxClave;
	
}




