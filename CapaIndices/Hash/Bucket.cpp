///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bucket.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Bucket.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Bucket.h"

///////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////
/* 
 * Este constructor inicializa un bucket nuevo
 */
// TODO: conseguir el num de bloque.

Bucket::Bucket(unsigned int * numBucket, unsigned short tamDispersion,IndiceManager *indiceHash):
Bloque(*numBucket,indiceHash->getTamanioBloque())
{
	this->tamDispersion = tamDispersion;
	this->cantRegs      = 0;
	//Los primeros 6 bytes se usan para guardar esplibre, cantRegs y tamDispersion.
	this->setEspacioLibre(Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + Tamanios::TAMANIO_DISPERSION); 
	// TODO: llamar a lo q sea q implemente Nico para hacer un bucket nuevo. PALURDOOOOO de 6 patas!!!
}
  
/**
 * Este constructor inicia un bloque preexistente levantandolo desde el archivo.
 */
Bucket::Bucket(IndiceManager* indiceHash, unsigned int nroBucket):
	Bloque(nroBucket,indiceHash->getTamanioBloque())
{
	indiceHash->leerBloque(nroBucket, this); 
}

/*
 * Constructor utilizado para crear un bucket auxiliar y no acceder a disco 
 **/

Bucket::Bucket(unsigned int nroBucket,unsigned short tamDispersion,unsigned int tamanioBloque):
	Bloque(nroBucket,tamanioBloque){
	
}

Bucket::~Bucket()
{
} 

///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////
void Bucket::setTamDispersion(unsigned short tDisp)
{
	tamDispersion = tDisp;
} 

unsigned short Bucket::getTamDispersion()
{
	return tamDispersion;
}

unsigned short Bucket::getOffsetToRegs()
{
	unsigned short aux = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + Tamanios::TAMANIO_DISPERSION;
	return aux;
}

void Bucket::setCantRegs(unsigned short cRegs)
{
	cantRegs = cRegs;
}

unsigned short Bucket::getCantRegs()
{
	return cantRegs;
}

char *Bucket::getClavePrimaria(const list <nodoLista>& listaParam, char* registro){
	return NULL;
}

void Bucket::incrementarCantRegistros()
{
	cantRegs++;
}

void Bucket::decrementarCantRegistros()
{
	cantRegs--;
}

void Bucket::eliminarBucket()
{
	//TODO implementar!!! llamar a la funcion del palurdo q borra el bloque.
}





