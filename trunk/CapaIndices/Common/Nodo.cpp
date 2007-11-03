///////////////////////////////////////////////////////////////////////////
//	Archivo   : Nodo.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases Nodo, NodoBp y NodoBStar.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Nodo.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: Nodo (Abstracta)
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Constructores/Destructor
///////////////////////////////////////////////////////////////////////
Nodo::Nodo(unsigned int refNodo, unsigned char nivel, Clave* clave,
											unsigned short tamanio)
{
	this->refNodo = refNodo;
	this->nivel   = nivel;
	this->tamanio = tamanio;
	//this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
	
	/*Agrega la clave a la lista de claves del nodo*/
	this->claves = new SetClaves();
	this->claves->insert(clave);
	
	this->actualizarEspacioLibre(clave,true);
}

Nodo::Nodo(unsigned int refNodo, unsigned char nivel, unsigned short tamanio)
{
	this->refNodo = refNodo;
    this->nivel = nivel;
    this->tamanio = tamanio;
    //this->espacioLibre = archivo->getTamanioNodo()- archivo->getTamanioHeader();
    
    /*Agrega la clave a la lista de claves del nodo*/
	this->claves = new SetClaves();
}

/*
Nodo::Nodo(ArchivoIndice* archivoIndice, int referencia)
{	
	archivoIndice->leer(referencia,this);
	//this->espacioLibre = archivoIndice->getTamanioNodo()-archivoIndice->getTamanioNodo();		
}
*/

Nodo::~Nodo()
{
	if(claves)
		delete this->claves;
}

///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////              

void Nodo::actualizarEspacioLibre(Clave* clave, bool insercion)
{
	if (insercion)
		this->setEspacioLibre(this->getEspacioLibre() - clave->getTamanioEnDisco());
	else
	   	this->setEspacioLibre(this->getEspacioLibre() + clave->getTamanioEnDisco());
}

void Nodo::actualizarEspacioLibre(SetClaves* claves, bool insercion)
{
	unsigned int suma = 0;
	
	for (SetClaves::iterator iter = claves->begin(); iter != claves->end(); ++iter){
		suma += (*iter)->getTamanioEnDisco();			
	}
	
	if (insercion)
		this->setEspacioLibre(this->getEspacioLibre() - suma);
	else
		this->setEspacioLibre(this->getEspacioLibre() + suma);
}


Clave* Nodo::buscar(Clave* claveBuscada) const
{
	return this->getClaves()->findClave(claveBuscada);
}


bool Nodo::reemplazarClave(Clave* claveVieja, Clave* claveNueva, char* codigo)
{		
	Clave* claveBuscada = this->buscar(claveVieja);
	
	if ((!claveBuscada) || (!(*claveBuscada == *claveVieja))) return false;
	
	this->eliminarClave(claveVieja, codigo);
	
	this->insertarClave(claveNueva, codigo);
    
	return true;
}



unsigned short Nodo::getTamanioEnDiscoSetClaves() const {
	
	//Calcula el tamanio que ocupa el conjunto de claves dentro del nodo
	unsigned short tamanio = 0;
	
	SetClaves::iterator iter;
	for (iter = this->getClaves()->begin(); iter != this->getClaves()->end(); ++iter){
		tamanio += (*iter)->getTamanioEnDisco();
	}
	
	return tamanio;
	
} 


unsigned short Nodo::bytesACeder(unsigned short bytesRequeridos, unsigned char &clavesPropuestas,
								 bool izquierda) const {
	
	unsigned short sumaBytesRequeridos = 0;
	clavesPropuestas = 0;
	
	if (izquierda){
		for (SetClaves::iterator iter = this->getClaves()->begin();
			(iter != this->getClaves()->end()) && (sumaBytesRequeridos < bytesRequeridos);
			++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			++clavesPropuestas;
		}
	}
	else{ //if (derecha)
		for (SetClaves::reverse_iterator iter = this->getClaves()->rbegin();
			(iter != this->getClaves()->rend()) && (sumaBytesRequeridos < bytesRequeridos);
			++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			++clavesPropuestas;
		}	
	}
	
	if (bytesRequeridos < sumaBytesRequeridos) return 0;
	else return sumaBytesRequeridos;
	
}

unsigned short Nodo::bytesACeder(unsigned char clavesPropuestas, bool izquierda) const{
	
	unsigned short sumaBytesRequeridos = 0;
	unsigned char i = 0;
	
	if (this->getClaves()->size() < clavesPropuestas) return 0;
	
	if (izquierda){
		for (SetClaves::iterator iter = this->getClaves()->begin();
			(iter != this->getClaves()->end()) && (i < clavesPropuestas);
			++iter, ++i){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
		}
	}
	else{ //if (derecha)
		for (SetClaves::reverse_iterator iter = this->getClaves()->rbegin();
			(iter != this->getClaves()->rend()) && (i < clavesPropuestas);
			++iter, ++i){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
		}	
	}
	
	return sumaBytesRequeridos;

}

bool Nodo::puedeRecibir(unsigned short bytesEntrantes, unsigned short bytesSalientes) const {
	if (bytesEntrantes > bytesSalientes){
		return ( (bytesEntrantes - bytesSalientes) < this->getEspacioLibre() ); 
	}
	else{
		return ( (this->getTamanioEnDiscoSetClaves() + bytesEntrantes - bytesSalientes) > this->getTamanioMinimo() );
	}
	
}


//Si puede ceder devuelve un conjunto con las claves a ceder, sino devuelve NULL.
SetClaves* Nodo::cederBytes(unsigned short bytesRequeridos, bool izquierda) {
	
	unsigned short sumaBytesRequeridos = 0;
	
	SetClaves* set = new SetClaves();
	
	if (izquierda){
		
		SetClaves::iterator iter;
		for (iter = this->getClaves()->begin();
			(iter != this->getClaves()->end()) && (sumaBytesRequeridos < bytesRequeridos);
			++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			set->insert(*iter);
		}
		
		if ( (getTamanioEnDiscoSetClaves() - sumaBytesRequeridos) > this->getTamanioMinimo() ) {
			this->getClaves()->erase(this->getClaves()->begin(), iter);
			return set;
		}
		
	}
	else{ //if (derecha)
		
		SetClaves::iterator iter;
		for (iter = (--(this->getClaves()->end()));
			(iter != this->getClaves()->begin()) && (sumaBytesRequeridos < bytesRequeridos);
			--iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			set->insert(*iter);
		}
		
		if ( (iter == this->getClaves()->begin()) && (sumaBytesRequeridos < bytesRequeridos) ) {
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			set->insert(*iter);
		}
		
		if ( (getTamanioEnDiscoSetClaves() - sumaBytesRequeridos) > this->getTamanioMinimo() ) {
			this->getClaves()->erase(iter, this->getClaves()->end());
			return set;
		}
	}
	
	set->clear();
	delete set;
	
	return NULL;
	
}


//Si puede ceder devuelve un conjunto con las claves a ceder, sino devuelve NULL.
SetClaves* Nodo::cederClaves(unsigned short cantClaves, bool izquierda) {
	
	unsigned short i = 0, sumaBytesRequeridos = 0;
	SetClaves* set = new SetClaves();
	
	if (izquierda){
		
		SetClaves::iterator iter;
		for (iter = this->getClaves()->begin();
			(iter != this->getClaves()->end()) && (i < cantClaves);
			++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			set->insert(*iter);
			++i;
		}
		
		if ( (getTamanioEnDiscoSetClaves() - sumaBytesRequeridos) > this->getTamanioMinimo() ) {
			this->getClaves()->erase(this->getClaves()->begin(), iter);
			return set;
		}
		
	}
	else{ //if (derecha)
		
		SetClaves::iterator iter;
		for (iter = (--(this->getClaves()->end()));
			(iter != this->getClaves()->begin()) && (i < cantClaves);
			--iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			set->insert(*iter);
			++i;
		}
		
		if ( (iter == this->getClaves()->begin()) && (i < cantClaves) ) {
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco();
			set->insert(*iter);
		}
		
		if ( (getTamanioEnDiscoSetClaves() - sumaBytesRequeridos) > this->getTamanioMinimo() ) {
			this->getClaves()->erase(iter, this->getClaves()->end());
			return set;
		}
	}
	
	set->clear();
	delete set;
	
	return NULL;
	
}


void Nodo::recibir(SetClaves* set){
	for(SetClaves::iterator iter = set->begin(); iter != set->end(); ++iter){
		this->getClaves()->insert(*iter);
	}
	set->clear();
}


unsigned short Nodo::obtenerBytesRequeridos() const {
	
	unsigned short bytesOcupados = this->getTamanioEnDiscoSetClaves();
	
	if (bytesOcupados >= this->getTamanioMinimo()) return 0;
	
	return (this->getTamanioMinimo() - bytesOcupados);
	
}


unsigned short Nodo::obtenerBytesSobrantes(unsigned short &cantClaves, bool izquierda) const {
	
	unsigned short tamanioEspacioClaves = this->getTamanioEspacioClaves();
	cantClaves = 0;
	unsigned short sumaSobrantes = 0;
	unsigned short tamanioEnDiscoSetClaves = this->getTamanioEnDiscoSetClaves();
	
	if (izquierda){
		for(SetClaves::iterator iter = this->getClaves()->begin(); (tamanioEnDiscoSetClaves - sumaSobrantes) > tamanioEspacioClaves; ++iter){
			++cantClaves;
			sumaSobrantes += (*iter)->getTamanioEnDisco();
		}
	}
	else{ //if (derecha)
		for(SetClaves::reverse_iterator iter = this->getClaves()->rbegin(); (tamanioEnDiscoSetClaves - sumaSobrantes) > tamanioEspacioClaves; ++iter){
			++cantClaves;
			sumaSobrantes += (*iter)->getTamanioEnDisco();
		}
	}
	
	return sumaSobrantes;
	
}


bool Nodo::esPadre(const Nodo* hijo, Clave* &clave) const {
	
	clave = this->buscar(hijo->obtenerPrimeraClave());
	
	if ( (clave) && (clave->getHijoDer() == hijo->getPosicionEnArchivo()) )
		return true;
	
	return false;
	
}
		

Clave* Nodo::obtenerPrimeraClave() const {
	
	if (this->getClaves()->empty()) return NULL;
	
	return *(this->getClaves()->begin());
	
}
		

Clave* Nodo::obtenerUltimaClave() const {
	
	if (this->getClaves()->empty()) return NULL;
			
	return *(this->getClaves()->rbegin());
	
}


bool Nodo::puedeRecibirClaveDesdeIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre, Clave* clavePadre) const {
	
	unsigned short cantClavesSobrantes;
	unsigned short bytesSobrantes = nodoHnoIzq->obtenerBytesSobrantes(cantClavesSobrantes, false);
	//Variable que almacena el tamanio en bytes de las claves sobrantes sin incluir la ultima clave.
	unsigned short primerosBytesSobrantes = nodoHnoIzq->bytesACeder(cantClavesSobrantes - 1, false);
	unsigned short bytesUltimaClaveSobrante = bytesSobrantes - primerosBytesSobrantes;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	
	if ( tamanioClavePadre > this->getEspacioLibre() ) return false;
	
	if ( (this->puedeRecibir(primerosBytesSobrantes + tamanioClavePadre, 0))
		&& ( nodoPadre->puedeRecibir(bytesUltimaClaveSobrante, tamanioClavePadre) ) )
		return true;
	
	return false;
}


bool Nodo::puedeRecibirClaveDesdeDer(Nodo* nodoHnoDer, Nodo* nodoPadre, Clave* clavePadre) const {
	
	unsigned short cantClavesSobrantes;
	unsigned short bytesSobrantes = nodoHnoDer->obtenerBytesSobrantes(cantClavesSobrantes);
	//Variable que almacena el tamanio en bytes de las claves sobrantes sin incluir la ultima clave.
	unsigned short primerosBytesSobrantes = nodoHnoDer->bytesACeder(cantClavesSobrantes - 1);
	unsigned short bytesUltimaClaveSobrante = bytesSobrantes - primerosBytesSobrantes;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	
	if ( tamanioClavePadre > this->getEspacioLibre() ) return false;
	
	if ( (this->puedeRecibir(primerosBytesSobrantes + tamanioClavePadre, 0))
		&& ( nodoPadre->puedeRecibir(bytesUltimaClaveSobrante, tamanioClavePadre) ) )
		return true;

}


bool Nodo::puedePasarClaveHaciaIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre, Clave* clavePadre) const {
	
	//Bytes requeridos por el hermano izquierdo
	unsigned short bytesRequeridos = nodoHnoIzq->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPadre = 0, bytesPropuestosPorMi = 0;
	unsigned short bytesHaciaElPadre = 0;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	unsigned char clavesPropuestas = 0;
	
	if ( ( tamanioClavePadre >= bytesRequeridos ) &&
		 (nodoPadre->puedeRecibir(this->obtenerPrimeraClave()->getTamanioEnDisco(), tamanioClavePadre)) )
		return true;
	
	bytesRequeridos -= tamanioClavePadre;
	bytesPropuestosPorMi = this->bytesACeder(bytesRequeridos, clavesPropuestas);
	
	if ( (bytesPropuestosPorMi > 0)
		&& (nodoHnoIzq->puedeRecibir(bytesPropuestosPorMi + tamanioClavePadre, 0))
		&& ( (bytesHaciaElPadre = this->bytesACeder(clavesPropuestas + 1)) > 0 ) ){
			bytesHaciaElPadre -= bytesPropuestosPorMi;
			if ( nodoPadre->puedeRecibir(bytesHaciaElPadre, tamanioClavePadre) )
				return true;
		}
	
	return false;
}


bool Nodo::puedePasarClaveHaciaDer(Nodo* nodoHnoDer, Nodo* nodoPadre, Clave* clavePadre) const {
	
	//Bytes requeridos por el hermano derecho
	unsigned short bytesRequeridos = nodoHnoDer->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPadre = 0, bytesPropuestosPorMi = 0;
	unsigned short bytesHaciaElPadre = 0;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco();
	unsigned char clavesPropuestas = 0;
	
	if ( ( tamanioClavePadre >= bytesRequeridos ) &&
		 (nodoPadre->puedeRecibir(this->obtenerUltimaClave()->getTamanioEnDisco(), tamanioClavePadre)) )
		return true;
		
	bytesRequeridos -= tamanioClavePadre;
	bytesPropuestosPorMi = this->bytesACeder(bytesRequeridos, clavesPropuestas, false);
	
	if ( (bytesPropuestosPorMi > 0)
		&& (nodoHnoDer->puedeRecibir(bytesPropuestosPorMi + tamanioClavePadre, 0))
		&& ( (bytesHaciaElPadre = this->bytesACeder(clavesPropuestas + 1, false)) > 0 ) ){
			bytesHaciaElPadre -= bytesPropuestosPorMi;
			if ( nodoPadre->puedeRecibir(bytesHaciaElPadre, tamanioClavePadre) )
				return true;
		}
	
	return false;	
	
}


void Nodo::merge(Nodo* nodoHno, Clave* clavePadre) {
	
	return this->getClaves()->merge(nodoHno->getClaves(), clavePadre, NULL);
	
}


void Nodo::merge(Nodo* nodoHno1, Nodo* nodoHno2, Clave* clavePadre1, Clave* clavePadre2) {
	
	return this->getClaves()->merge(nodoHno1->getClaves(), nodoHno2->getClaves(), clavePadre1, clavePadre2);
	
}


Clave* Nodo::extraerUltimaClave() {
	SetClaves::iterator iter = this->getClaves()->end();
	this->getClaves()->erase(*(--iter));
	return *iter;
}

Clave* Nodo::extraerPrimeraClave() {
	SetClaves::iterator iter = this->getClaves()->begin();
	this->getClaves()->erase(*iter);
	return *iter;
}

void Nodo::extraerClave(Clave* clave) {
	this->getClaves()->erase(clave);
}


bool Nodo::tieneOverflow() const {
	return (this->getTamanioEnDiscoSetClaves() > this->getTamanioEspacioClaves());
}


bool Nodo::tieneUnderflow() const {
	return (this->getTamanioEnDiscoSetClaves() < this->getTamanioMinimo());
}


SetClaves* Nodo::splitB(unsigned short minClaves) {
	return (this->getClaves()->splitBPlus(minClaves));
}


void Nodo::insertarClave(Clave* &clave, char* codigo)
{
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
	}else{ //No hay espacio libre suficiente para insertar la clave...
		 cod = Codigo::OVERFLOW;
	}

	*codigo = cod;	
}


void Nodo::eliminarClave(Clave* clave, char* codigo) {
	
	SetClaves* set = this->getClaves();
	SetClaves::iterator iter = set->find(clave);
	
	if (iter != set->end()) { //Se encontró la clave
		//Se libera la memoria utilizada por la clave
		delete *iter;
		//Se elimina el puntero a la clave que estaba dentro del conjunto
		set->erase(iter);
		//Se actualiza el espacio libre del nodo
		this->actualizarEspacioLibre(clave, false);
		
		if (this->getTamanioEnDiscoSetClaves() < this->getTamanioMinimo()) 
			*codigo = Codigo::UNDERFLOW;
		else
	 		*codigo = Codigo::MODIFICADO;
	 	
	}
	
	*codigo = Codigo::NO_MODIFICADO;
	
}


