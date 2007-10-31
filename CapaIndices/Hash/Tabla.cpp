#include "Tabla.h"
 
Tabla::Tabla()
{
	tamanio = 1;
}

Tabla::Tabla(char* nombreArchivo)
{
	// TODO: lee el archivo e inicializa la tabla con las direcciones de los buckets.
}

Tabla::~Tabla()
{
}

void Tabla::crear(char* nombreArchivo)
{
	// Creo un archivo con un solo bucket, y un archivo para la tabla.
	// TODO: pido a la capa fisica q los cree.
	
}

unsigned int Tabla::getTamanio()
{
	return tamanio;
}


unsigned int Tabla::getNroBucket(int posicionTabla)
{
	return nroBucket[posicionTabla];
}

void Tabla::reorganizarTabla(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket){
	
	if((tamDispActualizado/2)== tamanio){
		duplicarTabla();
		nroBucket[posicion] = nuevoNroBucket;
		
	}
	else
		actualizarReferencias(tamDispActualizado,posicion,nuevoNroBucket);
	
}

void Tabla::duplicarTabla(){
	int * nroBuckets = new int[tamanio*2];
	
	memcpy(nroBuckets,nroBucket,tamanio);
	memcpy(&nroBuckets[tamanio],nroBucket,tamanio);
	tamanio *=2;
	
}

void Tabla::actualizarReferencias(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket){
	int i;
	for(i = posicion;i >= 0;i -= tamDispActualizado)
		nroBucket[i] = nuevoNroBucket;
	
	for(i = posicion; (unsigned)i<tamanio;i += tamDispActualizado)
		nroBucket[i] = nuevoNroBucket;
}
