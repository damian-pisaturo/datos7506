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
	indiceHash->leerBloque(nroBucket, this);
	
	this->setTamanioBloque(indiceHash->getTamanioBloque());
	this->setNroBloque(nroBucket);
	this->setOffsetADatos(Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + 
				   Tamanios::TAMANIO_DISPERSION);
}

/*
 * Constructor utilizado para crear un bucket nuevo en memoria y no acceder a disco 
 **/
Bucket::Bucket(unsigned int nroBucket,unsigned short tamDisp,unsigned int tamanioBloque)
:Bloque(nroBucket,tamanioBloque){
	
	tamDispersion = tamDisp;
	cantRegs = 0;
	char* datos = getDatos();
	
	//Los primeros 6 bytes se usan para guardar esplibre, cantRegs y tamDispersion.
	unsigned short eLibre = Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS + 
	  						Tamanios::TAMANIO_DISPERSION; 
	this->setEspacioLibre(eLibre);
	
	memcpy(datos,&eLibre,Tamanios::TAMANIO_ESPACIO_LIBRE);

	// El primer registro se encontrará donde esta el espacio libre al momento de la creación del bucket.
	this->setOffsetADatos(eLibre);

	// Actualiza el tamaño de dispersión dentro de la tira de bytes.
	memcpy(&datos[Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS],&tamDisp,
		   Tamanios::TAMANIO_DISPERSION);
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





