#ifndef HASH_H_
#define HASH_H_

#include <string>
#include "Tabla.h"
#include "Bucket.h"

#define OK 		0
#define FAIL 	1

class Hash
{
private:
	Tabla tabla;
	Bucket* bucket;
	int tamanioClave;  // -1 si es de longitud variable.
	
public:
	Hash();
	virtual ~Hash();
	int aplicarHash(char* clave);
	int insertarRegistro(char* clave);
	int eliminarRegistro(char* clave);
	
private:
	int hash(char* clave);

};

#endif /*HASH_H_*/
