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
		   unsigned short tamanio, bool bstar)
{
	this->bstar = bstar;
	this->refNodo = refNodo;
	this->nivel   = nivel;
	this->tamanio = tamanio;
	this->posicionEnArchivo = 1;
	
	if (bstar && (nivel == 0))
		this->setEspacioLibre(tamanio - this->getTamanioHeader() + Tamanios::TAMANIO_REFERENCIA);
	else
		this->setEspacioLibre(tamanio - this->getTamanioHeader());
	
	/*Agrega la clave a la lista de claves del nodo*/
	this->claves = new SetClaves();
	this->claves->insert(clave);
	this->actualizarEspacioLibre(clave, true);
}

Nodo::Nodo(unsigned int refNodo, unsigned char nivel, unsigned short tamanio, bool bstar)
{
	this->bstar = bstar;
	this->refNodo = refNodo;
    this->nivel = nivel;
    this->tamanio = tamanio;
    this->posicionEnArchivo = 1;

    if (bstar && (nivel == 0))
		this->setEspacioLibre(tamanio - this->getTamanioHeader() + Tamanios::TAMANIO_REFERENCIA);
	else
		this->setEspacioLibre(tamanio - this->getTamanioHeader());
    
    /*Agrega la clave a la lista de claves del nodo*/
	this->claves = new SetClaves();
}


Nodo::~Nodo()
{
	
	if(claves)
		delete this->claves;
}

///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////              


Clave* Nodo::buscar(Clave* claveBuscada) const
{
	return this->getClaves()->findClave(claveBuscada);
}

Clave* Nodo::buscarSiguiente(Clave* claveBuscada) const
{
	return this->getClaves()->findClaveSiguiente(claveBuscada);
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
		tamanio += (*iter)->getTamanioEnDisco(bstar);
	}
	
	return tamanio;
	
} 


unsigned short Nodo::bytesACeder(unsigned short bytesRequeridos, unsigned char &clavesPropuestas,
								 bool izquierda) const {
	
	unsigned short sumaBytesRequeridos = 0;
	unsigned short tamanioEnDiscoSetClaves = this->getTamanioEnDiscoSetClaves();
	unsigned short tamanioMinimo = this->getTamanioMinimo();
	clavesPropuestas = 0;
	
	
	if (izquierda){
		for (SetClaves::iterator iter = this->getClaves()->begin();
			(iter != this->getClaves()->end()) && (sumaBytesRequeridos < bytesRequeridos) &&
			((tamanioEnDiscoSetClaves - sumaBytesRequeridos) > tamanioMinimo);
			++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
			++clavesPropuestas;
		}
	}
	else{ //if (derecha)
		for (SetClaves::reverse_iterator iter = this->getClaves()->rbegin();
			(iter != this->getClaves()->rend()) && (sumaBytesRequeridos < bytesRequeridos) &&
			((tamanioEnDiscoSetClaves - sumaBytesRequeridos) > tamanioMinimo);
			++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
			++clavesPropuestas;
		}	
	}
	
	if ( (bytesRequeridos > sumaBytesRequeridos) || ((tamanioEnDiscoSetClaves - sumaBytesRequeridos) < tamanioMinimo) ) return 0;
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
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
		}
	}
	else{ //if (derecha)
		for (SetClaves::reverse_iterator iter = this->getClaves()->rbegin();
			(iter != this->getClaves()->rend()) && (i < clavesPropuestas);
			++iter, ++i){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
		}	
	}
	
	return sumaBytesRequeridos;

}

bool Nodo::puedeRecibir(unsigned short bytesEntrantes, unsigned short bytesSalientes) const {
	
	unsigned short tamanioEspacioClavesResultante = this->getTamanioEnDiscoSetClaves() + bytesEntrantes - bytesSalientes;
	
	if (bytesEntrantes >= bytesSalientes) {
		return ( tamanioEspacioClavesResultante <=  this->getTamanioEspacioClaves() ); 
	}
	else{
		//Si este nodo es el nodo raíz, no se verifica underflow
		if (this->getPosicionEnArchivo() == 0) return true;
		return ( tamanioEspacioClavesResultante >= this->getTamanioMinimo() );
	}
	
}


//Devuelve un conjunto con las claves a ceder
SetClaves* Nodo::cederBytes(unsigned short bytesRequeridos, bool izquierda) {

	unsigned short sumaBytesRequeridos = 0;
	
	SetClaves* set = new SetClaves();
	
	if (izquierda){
		
		SetClaves::iterator iter;
		for (iter = this->getClaves()->begin();
			(iter != this->getClaves()->end()) && (sumaBytesRequeridos < bytesRequeridos);
			++iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
			set->insert(*iter);
		}
		
		this->getClaves()->erase(this->getClaves()->begin(), iter);
		
	}
	else{ //if (derecha)
		
		SetClaves::iterator iter;
		for (iter = (--(this->getClaves()->end()));
			(iter != this->getClaves()->begin()) && (sumaBytesRequeridos < bytesRequeridos);
			--iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
			set->insert(*iter);
		}

		this->getClaves()->erase(++iter, this->getClaves()->end());

	}
	
	this->actualizarEspacioLibre();
	return set;
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
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
			set->insert(*iter);
			++i;
		}
		
		if ( (getTamanioEnDiscoSetClaves() - sumaBytesRequeridos) >= this->getTamanioMinimo() ) {
			this->getClaves()->erase(this->getClaves()->begin(), iter);
			return set;
		}
		
	}
	else{ //if (derecha)
		
		SetClaves::iterator iter;
		for (iter = (--(this->getClaves()->end()));
			(iter != this->getClaves()->begin()) && (i < cantClaves);
			--iter){
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
			set->insert(*iter);
			++i;
		}
		
		if ( (iter == this->getClaves()->begin()) && (i < cantClaves) ) {
			sumaBytesRequeridos += (*iter)->getTamanioEnDisco(bstar);
			set->insert(*iter);
		}
		
		if ( (getTamanioEnDiscoSetClaves() - sumaBytesRequeridos) >= this->getTamanioMinimo() ) {
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
	this->actualizarEspacioLibre(set, true);
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
			sumaSobrantes += (*iter)->getTamanioEnDisco(bstar);
		}
	}
	else{ //if (derecha)
		for(SetClaves::reverse_iterator iter = this->getClaves()->rbegin(); (tamanioEnDiscoSetClaves - sumaSobrantes) > tamanioEspacioClaves; ++iter){
			++cantClaves;
			sumaSobrantes += (*iter)->getTamanioEnDisco(bstar);
		}
	}
	
	return sumaSobrantes;
	
}

unsigned short Nodo::obtenerBytesSobreMinimo(bool izquierda) const {
	
	unsigned short tamanioEnDiscoSetClaves = this->getTamanioEnDiscoSetClaves();
	unsigned short suma = 0, sumaAnterior = 0;
	unsigned short tamanioMinimo = this->getTamanioMinimo();
	
	if (izquierda){
		for(SetClaves::iterator iter = this->getClaves()->begin(); ( iter != this->getClaves()->end() ) && ( (tamanioEnDiscoSetClaves - suma) > tamanioMinimo ); ++iter){
			sumaAnterior = suma;
			suma += (*iter)->getTamanioEnDisco(bstar);
		}
	}
	else{ //if (derecha)
		for(SetClaves::reverse_iterator iter = this->getClaves()->rbegin(); ( iter != this->getClaves()->rend() ) && ( (tamanioEnDiscoSetClaves - suma) > tamanioMinimo ); ++iter){
			sumaAnterior = suma;
			suma += (*iter)->getTamanioEnDisco(bstar);
		}
	}
	
	return sumaAnterior;
	
}

bool Nodo::esPadre(const Nodo* hijo, Clave* &claveProxNodo, Clave* claveNodoHijo) const {
	
	Clave* primeraClaveNodoHijo = hijo->obtenerPrimeraClave();
	
	if (primeraClaveNodoHijo)
		claveProxNodo = this->buscar(primeraClaveNodoHijo);
	else
		claveProxNodo = this->buscar(claveNodoHijo);
	
	if ( (claveProxNodo) && (claveProxNodo->getHijoDer() == hijo->getPosicionEnArchivo()) )
		return true;
	
	if ((!claveProxNodo) && (this->getHijoIzq() == hijo->getPosicionEnArchivo()))
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
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(bstar);
	unsigned short tamanioClavePadreARecibir = tamanioClavePadre;

	if (this->getNivel() == 0) {
		tamanioClavePadreARecibir -= Tamanios::TAMANIO_REFERENCIA;
		bytesUltimaClaveSobrante += Tamanios::TAMANIO_REFERENCIA;
	}
	
	if ( tamanioClavePadreARecibir > this->getEspacioLibre() ) return false;
	
	
	if ( (this->puedeRecibir(primerosBytesSobrantes + tamanioClavePadreARecibir, 0))
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
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(bstar);
	unsigned short tamanioClavePadreARecibir = tamanioClavePadre;

	if (this->getNivel() == 0) {
		tamanioClavePadreARecibir -= Tamanios::TAMANIO_REFERENCIA;
		bytesUltimaClaveSobrante += Tamanios::TAMANIO_REFERENCIA;
	}
	
	if ( tamanioClavePadreARecibir > this->getEspacioLibre() ) return false;
	
	if ( (this->puedeRecibir(primerosBytesSobrantes + tamanioClavePadreARecibir, 0))
		&& ( nodoPadre->puedeRecibir(bytesUltimaClaveSobrante, tamanioClavePadre) ) )
		return true;
	
	return false;

}


bool Nodo::puedePasarClaveHaciaIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre, Clave* clavePadre) const {

	//Bytes requeridos por el hermano izquierdo
	unsigned short bytesRequeridos = nodoHnoIzq->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPorMi = 0;
	unsigned short bytesHaciaElPadre = 0;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(bstar);
	unsigned char clavesPropuestas = 0;
	unsigned short tamanioClavePadreARecibir = tamanioClavePadre;
	Clave* primeraClave = this->obtenerPrimeraClave();
	if (!primeraClave) return false;
	unsigned short tamanioClaveHaciaElPadre = primeraClave->getTamanioEnDisco(bstar);
	
	if (this->getNivel() == 0) {
		tamanioClavePadreARecibir -= Tamanios::TAMANIO_REFERENCIA;
		tamanioClaveHaciaElPadre += Tamanios::TAMANIO_REFERENCIA;
	}

	if ( ( tamanioClavePadreARecibir >= bytesRequeridos ) &&
		 (nodoPadre->puedeRecibir(tamanioClaveHaciaElPadre, tamanioClavePadre)) &&
		 (bytesACeder(this->obtenerPrimeraClave()->getTamanioEnDisco(bstar), clavesPropuestas) > 0) )
		return true;
	
	bytesRequeridos -= tamanioClavePadreARecibir;
	bytesPropuestosPorMi = this->bytesACeder(bytesRequeridos, clavesPropuestas);

	if ( (bytesPropuestosPorMi > 0)
		&& (nodoHnoIzq->puedeRecibir(bytesPropuestosPorMi + tamanioClavePadreARecibir, 0))
		&& ( (bytesHaciaElPadre = this->bytesACeder(clavesPropuestas + 1)) > 0 ) ) {
			bytesHaciaElPadre -= bytesPropuestosPorMi;
			//Si la clave pasa de un nodo hoja a un nodo interno, el tamaño se incrementa
			//debido a que ahora tendrá una referencia al hijo derecho.
			if (this->getNivel() == 0) bytesHaciaElPadre += Tamanios::TAMANIO_REFERENCIA;
			if ( nodoPadre->puedeRecibir(bytesHaciaElPadre, tamanioClavePadre) )
				return true;
	}
	
	return false;
}


bool Nodo::puedePasarClaveHaciaDer(Nodo* nodoHnoDer, Nodo* nodoPadre, Clave* clavePadre) const {
	
	//Bytes requeridos por el hermano derecho
	unsigned short bytesRequeridos = nodoHnoDer->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPorMi = 0;
	unsigned short bytesHaciaElPadre = 0;
	unsigned short tamanioClavePadre = clavePadre->getTamanioEnDisco(bstar);
	unsigned char clavesPropuestas = 0;
	unsigned short tamanioClavePadreARecibir = tamanioClavePadre;
	Clave* primeraClave = this->obtenerPrimeraClave();
	if (!primeraClave) return false;
	unsigned short tamanioClaveHaciaElPadre = primeraClave->getTamanioEnDisco(bstar);

	if (this->getNivel() == 0) {
		tamanioClavePadreARecibir -= Tamanios::TAMANIO_REFERENCIA;
		tamanioClaveHaciaElPadre += Tamanios::TAMANIO_REFERENCIA;
	}
	
	if ( ( tamanioClavePadreARecibir >= bytesRequeridos ) &&
		 (nodoPadre->puedeRecibir(tamanioClaveHaciaElPadre, tamanioClavePadre)) &&
		 (bytesACeder(this->obtenerPrimeraClave()->getTamanioEnDisco(bstar), clavesPropuestas, false) > 0) )
		return true;
		
	bytesRequeridos -= tamanioClavePadreARecibir;
	bytesPropuestosPorMi = this->bytesACeder(bytesRequeridos, clavesPropuestas, false);
	
	if ( (bytesPropuestosPorMi > 0)
		&& (nodoHnoDer->puedeRecibir(bytesPropuestosPorMi + tamanioClavePadreARecibir, 0))
		&& ( (bytesHaciaElPadre = this->bytesACeder(clavesPropuestas + 1, false)) > 0 ) ){
			bytesHaciaElPadre -= bytesPropuestosPorMi;
			//Si la clave pasa de un nodo hoja a un nodo interno, el tamaño se incrementa
			//debido a que ahora tendrá una referencia al hijo derecho.
			if (this->getNivel() == 0) bytesHaciaElPadre += Tamanios::TAMANIO_REFERENCIA;
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
	if (this->getClaves()->empty()) return NULL;
	SetClaves::iterator iter = this->getClaves()->end();
	Clave* clave = *(--iter);
	this->getClaves()->erase(iter);
	this->actualizarEspacioLibre(clave, false);
	return clave;
}

Clave* Nodo::extraerPrimeraClave() {
	if (this->getClaves()->empty()) return NULL;
	SetClaves::iterator iter = this->getClaves()->begin();
	Clave* clave = *iter;
	this->getClaves()->erase(iter);
	this->actualizarEspacioLibre(clave, false);
	return clave;
}

void Nodo::extraerClave(Clave* clave) {
	SetClaves::iterator iter = this->getClaves()->find(clave);
	if (iter == this->getClaves()->end()) return;
	this->getClaves()->erase(iter);
	this->actualizarEspacioLibre(clave, false);
}


bool Nodo::tieneOverflow() const {	
	return (this->getTamanioEnDiscoSetClaves() > this->getTamanioEspacioClaves());
}


bool Nodo::tieneUnderflow() const {
	return (this->getTamanioEnDiscoSetClaves() < this->getTamanioMinimo());
}


SetClaves* Nodo::splitB(unsigned short minClaves) {
	SetClaves* set = this->getClaves()->splitBPlus(minClaves, this->getTamanioEspacioClaves(), bstar, this->getNivel() == 0);
	this->actualizarEspacioLibre();
	return set;
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
	if (this->getEspacioLibre() >= clave->getTamanioEnDisco(bstar)){
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
		
		//Se actualiza el espacio libre del nodo
		this->actualizarEspacioLibre(clave, false);
		
		//Se libera la memoria utilizada por la clave
		delete *iter;
		//Se elimina el puntero a la clave que estaba dentro del conjunto
		set->erase(iter);
		
		if (this->getTamanioEnDiscoSetClaves() < this->getTamanioMinimo()) 
			*codigo = Codigo::UNDERFLOW;
		else
	 		*codigo = Codigo::MODIFICADO;
	
	} else
		*codigo = Codigo::NO_MODIFICADO;
	
}


void Nodo::setClaves(SetClaves* set)
{	
	if (this->claves)
		delete this->claves;
	this->claves = set;
	this->actualizarEspacioLibre();
}
		

//Método que carga en este nodo todos los mismos valores que posee el nodo pasado por parámetro
Nodo& Nodo::operator = (const Nodo &nodo) {
	
	this->setBstar(nodo.isBstar());
	this->setHijoIzq(nodo.getHijoIzq());
	this->setNivel(nodo.getNivel());
	this->setTamanio(nodo.getTamanio());
	
	SetClaves* setClavesCopia = new SetClaves();
	*setClavesCopia = *(nodo.getClaves());
	this->setClaves(setClavesCopia);
	
	this->setPosicionEnArchivo(nodo.getPosicionEnArchivo());
	this->setEspacioLibre(nodo.getEspacioLibre());
	
	return *this;
	
}


void Nodo::actualizarEspacioLibre(Clave* clave, bool insercion)
{	
	if (insercion)
		this->setEspacioLibre(this->getEspacioLibre() - clave->getTamanioEnDisco(bstar));
	else
	   	this->setEspacioLibre(this->getEspacioLibre() + clave->getTamanioEnDisco(bstar));
}

void Nodo::actualizarEspacioLibre(SetClaves* claves, bool insercion)
{
	unsigned int suma = 0;
	
	for (SetClaves::iterator iter = claves->begin(); iter != claves->end(); ++iter){
		suma += (*iter)->getTamanioEnDisco(bstar);			
	}
	
	if (insercion)
		this->setEspacioLibre(this->getEspacioLibre() - suma);
	else
		this->setEspacioLibre(this->getEspacioLibre() + suma);
}


void Nodo::actualizarEspacioLibre() {

	unsigned int espacioLibre = this->getTamanioEspacioClaves();
	
	for (SetClaves::iterator iter = this->getClaves()->begin(); iter != this->getClaves()->end(); ++iter) {
		espacioLibre -= (*iter)->getTamanioEnDisco(bstar);
	}
	
	this->setEspacioLibre(espacioLibre);
	
}

