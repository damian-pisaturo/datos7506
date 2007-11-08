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
  
/**
 * Este constructor inicia un bloque preexistente levantandolo desde el archivo.
 */
Bucket::Bucket(IndiceManager* indiceHash, unsigned int nroBucket):
	Bloque()
{
	cout << "constructor bucket q lee de archivo"<< endl;
	indiceHash->leerBloque(nroBucket, this);
	this->setTamanioBloque(indiceHash->getTamanioBloque());
	this->setNroBloque(nroBucket);
}

/*
 * Constructor utilizado para crear un bucket nuevo en memoria y no acceder a disco 
 **/
Bucket::Bucket(unsigned int nroBucket,unsigned short tamDisp,unsigned int tamanioBloque):
	Bloque(nroBucket,tamanioBloque){
	cout << "constructor bucket q crea en memoria."<< endl;
	tamDispersion = tamDisp;
	cantRegs = 0;
	//Los primeros 6 bytes se usan para guardar esplibre, cantRegs y tamDispersion.
	this->setEspacioLibre(Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + Tamanios::TAMANIO_DISPERSION);
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
	char * datos = getDatos();
	// Actualizo el tamaño de dispersión tambien en "datos".
	memcpy(&datos[Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS],&tDisp,Tamanios::TAMANIO_DISPERSION);
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

void Bucket::incrementarCantRegistros()
{
	cantRegs++;
}

void Bucket::decrementarCantRegistros()
{
	cantRegs--;
}

/*
 * Actualiza la variable de espacio libre.
 **/
void Bucket::actualizarEspLibre()
{
	unsigned short espLibre;
	memcpy(&espLibre,getDatos(),Tamanios::TAMANIO_ESPACIO_LIBRE);
	setEspacioLibre(espLibre);
}




