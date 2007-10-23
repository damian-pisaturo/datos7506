#include "NodoBStar.h"


void NodoBStar::insertarClave(Clave* &clave, char* codigo) {
	
	
	char cod = Codigo::NO_MODIFICADO;
			
	//Insercion ordenada de la clave en el conjunto
	this->getClaves()->insert(clave);
	
	//Si hay espacio suficiente para la nueva clave ...
	if (this->getEspacioLibre() > clave->getTamanioEnDisco()){
		this->actualizarEspacioLibre(clave, true);
		cod = Codigo::MODIFICADO;
	
		//TODO Escribir a disco
	    //Sobreescribe el nodo, actualizando las modificaciones
	    //archivoIndice->sobreescribirNodo(this);
	//No hay espacio libre suficiente para insertar la clave...
	}else{
		//TODO 97.98% seguro de que vuela esto que esta comentado.
	/*	
		//TODO Ver el temita del hijo izquierdo.
		//El hijo izquierdo tendria que ser el hijo dereche de la clave promocionada.
		Nodo* nuevoNodo = new NodoBStar(this->getRefNodo(), this->getNivel());
		
		//Condicion para overflow, devuelve la cantidad de claves que 
		//deben quedar en el nodo que se va a dividir
		//o sea la mitad y si el numero es impar la parte mas chica
		
		//splitBStar() devuelve un vector de nuevos conjuntos (ya creados)
		//con las claves mayores (cada uno 2/3 lleno)
		//TODO Reemplazar "10" por el tamanio en bytes minimo
		//de claves en un nodo (condicion de underflow).
		VectorConjuntos* vecConj = this->getClaves()->splitBStar(10);
        
		SetClaves* setNuevo = (*vecConj)[0];
		
		//Se le settea el conjunto de claves proveniente de la division al nuevoNodo
		nuevoNodo->setClaves(setNuevo);
		
		//Se actualiza el espacio libre
		this->actualizarEspacioLibre(setNuevo, false);
		//Corrección debido a que la division de conjuntos se hace luego de 
		//efectuada la insercion
		this->actualizarEspacioLibre(clave, true);
		
		//TODO URGENTE VER EL TEMITA DEL ACCESO A DISCOOO !
		//Lo grabo en el archivo  
		//archivoIndice->grabarNuevoNodo(nuevoNodo);
		
		//Obtengo la primer clave del conjunto del nuevo nodo
		//La quito del conjunto, asi no se borra cuando borro el nodo
		//OBSERVACION: El nodo ya fue grabado, y despues de esto se destruye
		//(no se pierde info)
		SetClaves::iterator iter = nuevoNodo->getClaves()->begin();
		clave = *iter;
		nuevoNodo->getClaves()->erase(iter);
		
		//El hijo izquierdo del nuevo nodo es el hijo derecho de la clave promovida
		 nuevoNodo->setHijoIzq(clave->getHijoDer());
		 
		 //Lo grabo en el archivo
		 //TODO SUPERMERCADO DISCO.
		 //archivoIndice->grabarNuevoNodo(nuevoNodo);
		 clave->setHijoDer(nuevoNodo->getPosicionEnArchivo());                
		 
		 //Se borra de memoria el nuevo nodo creado
		 delete nuevoNodo;
		 */
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
		
		/*
		if (this->espacioLibre > this->condicionMinima(archivo))
	   		*codigo = Codigo::UNDERFLOW;
	 	else
	 		*codigo = Codigo::MODIFICADO;
	 	*/
	}
	
	*codigo = Codigo::NO_MODIFICADO;
}


bool NodoBStar::puedeCeder(unsigned bytesRequeridos, bool izquierda){
	
	unsigned sumaBytesRequeridos = 0;
	unsigned sumaBytesRestantes = 0;
	
	if (izquierda){
		SetClaves::iterator iter;
		for (iter = this->getClaves()->begin(); (iter != this->getClaves()->end()) && (sumaBytesRequeridos < bytesRequeridos); ++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
		}
	//	for (iter)
	}
	
	return true; //TODO lo puse para que compile
	
}


Nodo* NodoBStar::siguiente(Clave* clave) {
	
	//Este metodo es llamado solamente si en este nodo no se encontro
	//la clave buscada.
	
	Nodo* nodo;
	Clave* claveResultante = this->getClaves()->findClave(clave);
	
	if (claveResultante == NULL) {
		//Cargar un nuevo nodo en memoria a partir del hijo izquierdo
		//de este nodo.
		//archivo->cargarNodo(nodo, this->refNodo);
	} else {
		//Cargar un nuevo nodo en memoria a partir del hijo derecho
		//de claveResultante.
		//archivo->cargarNodo(nodo, claveResultante->getHijoDer());
	}
	
	return nodo;
	
}
