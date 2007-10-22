#ifndef BUCKET_H_
#define BUCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RegistroV.h"

class Bucket
{
private:
	char* datos;
	int tamanio;
	char* registro;
	int espacioLibre;
	int tamDispersion;
	int cantRegs;
	
public:
	Bucket(int tamanioBucket);
	virtual ~Bucket();
	void leer(int offset);
	void escribir(int offset);
	bool buscarRegistro(char* clave, int tamanioClave);
	
private:
	bool buscarRegClaveV(char* clave, int tamanioClave);
	bool buscarRegClaveF(char* clave, int tamanioClave);
};

#endif /*BUCKET_H_*/
