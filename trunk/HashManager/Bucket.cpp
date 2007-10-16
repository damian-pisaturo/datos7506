#include "Bucket.h"

Bucket::Bucket(int tamanioBucket)
{
	tamanio = tamanioBucket;
	datos = new char[tamanio];
}

Bucket::~Bucket()
{
	delete[] datos;
}

void Bucket::leer(int offset)
{
	// TODO: pedir a la capa fisica que levante tamanio desde offset
}

void Bucket::escribir(int offset)
{
	// TODO: pedir a la capa fisica q escriba datos a partir de offset en el archivo.
}
