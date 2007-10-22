#include "RegistroV.h"

#include <string>

RegistroV::RegistroV(char* contenido, int longitud)
{
	tamanio = longitud;
	datos = new char[longitud];
	memcpy(datos, contenido, longitud); // copio los datos del registro en "datos".
	char aux[2];
	memcpy(aux,datos,1); // en el primer byte del registro se encuentra la longitud de la clave
	aux[1] = '\0';
	longitudClave = atoi(aux);
}

RegistroV::~RegistroV()
{
	delete[] datos;
}
 
char* RegistroV::getClave()
{
	char* aux = new char[longitudClave+1]; // TODO: ver q esten bien los offsets.
	memcpy(aux,&datos[1],longitudClave); // supongo que en datos[0] esta la longitud de la clave.
	aux[longitudClave] = '\0';
	
	return aux;
}

int RegistroV::getLongitudClave()
{
	return longitudClave;
}

