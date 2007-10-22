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
	
	/*
	 * Lee un bucket del archivo desde el offset y carga sus datos en la clase.
	 */
	void leer(int offset);
	
	/*
	 * Escribe en disco los datos del bucket.
	 */ 
	void escribir(int offset);
	 
	/*
	 * Busca un registro por su clave dentro del bucket. Si existe lo carga en el 
	 * atributo registro y devuelve true; de lo contrario devuelve false.
	 */ 
	bool buscarRegistro(char* clave, int tamanioClave);
	
private:
	
	/*
	 * Busca un registro de clave de longitud variable en el bucket.
	 */
	bool buscarRegClaveV(char* clave, int tamanioClave);
	
	/*
	 * Busca un registro de clave de longitud fija en el bucket.
	 */
	bool buscarRegClaveF(char* clave, int tamanioClave);
};

#endif /*BUCKET_H_*/
