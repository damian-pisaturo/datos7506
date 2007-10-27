#include "NodoBStar.h"

NodoBStar::NodoBStar(unsigned int refNodo, unsigned char nivel, unsigned short tamanio)
		 : Nodo(refNodo, nivel, tamanio) {
	
	if (nivel = 0) //Nodo hoja (le descuento el tamaño que ocupa la referencia al hijo izq)
		this->setTamanioMinimo(2*(tamanio-this->getTamanioHeader()-Tamanios::TAMANIO_REFERENCIA)/3);
	else //Nodo interno
		this->setTamanioMinimo(2*(tamanio-this->getTamanioHeader())/3);
	
}


NodoBStar::NodoBStar(unsigned int refNodo, unsigned char nivel, Clave* clave, unsigned short tamanio)
		  : Nodo(refNodo, nivel, clave, tamanio) {
	
	if (nivel = 0) //Nodo hoja (le descuento el tamaño que ocupa la referencia al hijo izq)
		this->setTamanioMinimo(2*(tamanio-this->getTamanioHeader()-Tamanios::TAMANIO_REFERENCIA)/3);
	else //Nodo interno
		this->setTamanioMinimo(2*(tamanio-this->getTamanioHeader())/3);
	
}


void NodoBStar::insertarClave(Clave* &clave, char* codigo) {
	
	
	char cod = Codigo::NO_MODIFICADO;
	
	//Se verifica si la clave está en el conjunto
	if (this->getClaves()->find(clave) != this->getClaves()->end()) {
		*codigo = cod;
		return;
	}
	
	//Insercion ordenada de la clave en el conjunto
	this->getClaves()->insert(clave);
	
	//Si hay espacio suficiente para la nueva clave ...
	if (this->getEspacioLibre() > clave->getTamanioEnDisco()){
		this->actualizarEspacioLibre(clave, true);
		cod = Codigo::MODIFICADO;
	
		//TODO Escribir a disco (por ahora parece que no vamos a escribir aca, lo hacemos desde afuera
	    //Sobreescribe el nodo, actualizando las modificaciones
	    //archivoIndice->sobreescribirNodo(this);
	}else{ //No hay espacio libre suficiente para insertar la clave...
		 cod = Codigo::OVERFLOW;
	}

	*codigo = cod;
}

//Siempre eliminara de hojas, se encarga el arbol de que el dato a eliminar este en ella.
void NodoBStar::eliminarClave(Clave* clave, char* codigo) {
	
	SetClaves* set = this->getClaves();
	SetClaves::iterator iter = set->find(clave);
	
	if (iter != set->end()) { //Se encontró la clave
		//Se libera la memoria utilizada por la clave
		delete *iter;
		//Se elimina el puntero a la clave que estaba dentro del conjunto
		set->erase(iter);
		//Se actualiza el espacio libre del nodo
		this->actualizarEspacioLibre(clave, false);
		
		
		if (this->getEspacioLibre() > this->getTamanioMinimo())
	   		*codigo = Codigo::UNDERFLOW;
	 	else
	 		*codigo = Codigo::MODIFICADO;
	 	
	}
	
	*codigo = Codigo::NO_MODIFICADO;
}

unsigned short NodoBStar::getTamanioEnDisco() const {
	
	//El tamanio se inicializa con los bytes ocupados por el atributo de
	//espacio libre y por el de nivel.
	unsigned short tamanio = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_NIVEL;

	tamanio += this->getTamanioEnDiscoSetClaves();
	
	if (this->getNivel() != 0) tamanio += Tamanios::TAMANIO_REFERENCIA;
	
	return tamanio;
	
}


Nodo* NodoBStar::siguiente(Clave* clave) {
	
	//Este metodo es llamado solamente si en este nodo no se encontro
	//la clave buscada.
	
	Nodo* nodo;
	Clave* claveResultante = this->getClaves()->findClave(clave);
	
	if (claveResultante == NULL) {
		//Cargar un nuevo nodo en memoria a partir del hijo izquierdo
		//de este nodo.
		//nodo = new NodoBStar(archivo, this->getHijoIzq());
	} else {
		//Cargar un nuevo nodo en memoria a partir del hijo derecho
		//de claveResultante.
		//nodo = new NodoBStar(archivo, claveResultante->getHijoDer());
	}
	
	return nodo;
	
}


//Devuelve el espacio (en bytes) destinado para almacenar claves (elementos)
unsigned short NodoBStar::getTamanioEspacioClaves() const {
	if (this->getNivel() == 0) //Nodo hoja ==> Tengo que sumarle el espacio que se le descuenta por la referencia al hijo izq
		return (this->getTamanio() - this->getTamanioHeader() + Tamanios::TAMANIO_REFERENCIA);
	else return Nodo::getTamanioEspacioClaves();
}


VectorConjuntos* NodoBStar::split(){
	return this->getClaves()->splitBStar(this->getTamanioMinimo());
}
