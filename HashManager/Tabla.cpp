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

int Tabla::getTamanio()
{
	return tamanio;
}

int Tabla::getDireccionBucket(int i)
{
	return direccionesBuckets[i];
}

