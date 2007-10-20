#ifndef BUCKET_H_
#define BUCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "RegistroV.h"

class Bucket
{
private:
	int tamDispersion;
	int cantRegs;
	char* datos;
	int tamanio;
	char* registro;
	
public:
	Bucket(int tamanioBucket);
	virtual ~Bucket();
	void leer(int offset);
	void escribir(int offset);
	bool buscarRegistro(char* clave, int tamanioClave);
};

#endif /*BUCKET_H_*/
