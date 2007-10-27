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
	this->tamanioMinimo = 0;
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
    this->tamanioMinimo = 0;
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

/*
bool Nodo::puedeDonar(ArchivoIndice* archivo)
{
	bool devolver = true;
	
	if(this->espacioLibre >= condicionMinima(archivo))     
   		devolver = false;
	
	return devolver;     
}
*/

/*
unsigned int Nodo::condicionMinima(ArchivoIndice* archivo)
{
	unsigned int tamanioReal = archivo->getTamanioNodo() - archivo->getTamanioHeader();
	
	return  (tamanioReal / 2);   
}
*/

Clave* Nodo::buscar(Clave* claveBuscada) const
{
	return this->getClaves()->findClave(claveBuscada);
}

/*
bool vacio(ArchivoIndice* archivo)
{
	bool vacio = false;
	
	if((this->espacioLibre) == (archivo->getTamanioNodo() - archivo->getTamanioHeader()))
		vacio = true;
	
	return vacio;
}
*/

/*
Clave* Nodo::reemplazarClave(Clave* claveVieja, Clave* claveNueva, char* codigo)
{		
	this->claves->findClave(claveVieja);
	Clave* copia = claveNueva->copiar();
	Clave* reemplazada = (Clave*)this->claves->reemplazar(copia);
	*codigo = Codigo::MODIFICADO;
	
    delete reemplazada;
    
	return copia;
}
*/


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
SetClaves* Nodo::ceder(unsigned short bytesRequeridos, bool izquierda) {
	
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


unsigned short Nodo::obtenerBytesSobrantes(unsigned short &cantClaves) const {
	
	//TODO Implementar
	return 0;
	
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


bool Nodo::puedePasarClaveHaciaIzq(Nodo* nodoHnoIzq, Nodo* nodoPadre) const {
	
	//Bytes requeridos por el hermano izquierdo
	unsigned short bytesRequeridos = nodoHnoIzq->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPadre = 0, bytesPropuestosPorMi = 0;
	unsigned char clavesPropuestas = 0;
	
	if ( (bytesPropuestosPadre = nodoPadre->bytesACeder(bytesRequeridos, clavesPropuestas)) > 0 ) {
		
		//TODO Ver el tema de 'clavesPropuestas', habria que pasarle una variable.
		if ( nodoHnoIzq->puedeRecibir(bytesPropuestosPadre, 0) ) {
			
			if ( (bytesPropuestosPorMi = this->bytesACeder(clavesPropuestas)) > 0 ) {
				
				if ( nodoPadre->puedeRecibir(bytesPropuestosPorMi, bytesPropuestosPadre) ) {
					return true;
				}
				
			}
			
		}
		
	}
	
	return false;
}


bool Nodo::puedePasarClaveHaciaDer(Nodo* nodoHnoDer, Nodo* nodoPadre) const {
	
	unsigned short bytesRequeridos = nodoHnoDer->obtenerBytesRequeridos();
	unsigned short bytesPropuestosPadre = 0, bytesPropuestosPorMi = 0;
	unsigned char clavesPropuestas = 0;
	
	if ( (bytesPropuestosPadre = nodoPadre->bytesACeder(bytesRequeridos, clavesPropuestas, false)) > 0 ) {
		
		//TODO Ver el tema de 'clavesPropuestas', habria que pasarle una variable.
		if ( nodoHnoDer->puedeRecibir(bytesPropuestosPadre, 0) ) {

			if ( (bytesPropuestosPorMi = this->bytesACeder(clavesPropuestas, false)) > 0 ) {
				
				if ( nodoPadre->puedeRecibir(bytesPropuestosPorMi, bytesPropuestosPadre) ) {
					return true;
				}
				
			}
			
		}
		
	}
	
	return false;
	
}
