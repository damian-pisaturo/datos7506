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
Bucket::Bucket(unsigned short numBucket, unsigned short tamDispersion,ArchivoIndice *indiceHash): Bloque(numBucket,indiceHash->getTamanioBloque())
{
	this->tamDispersion = tamDispersion;
	this->cantRegs      = 0;
	//Los primeros 6 bytes se usan para guardar esplibre, cantRegs y tamDispersion.
	this->setEspacioLibre(Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + Tamanios::TAMANIO_DISPERSION); 
}
  
/**
 * Este constructor inicia un bloque preexistente levantandolo desde el archivo.
 */
Bucket::Bucket(ArchivoIndice* indiceHash, unsigned int nroBucket):Bloque(nroBucket,indiceHash->getTamanioBloque())
{
	indiceHash->leerBloque(nroBucket, this); 
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

unsigned short Bucket::getOffsetToRegs()
{
	unsigned short aux = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + Tamanios::TAMANIO_DISPERSION;
	return aux;
}

void Bucket::setCantRegs(unsigned short cRegs)
{
	cantRegs = cRegs;
}








