#include "Hash.h"

Hash::Hash()
{
}

Hash::~Hash()
{
}

/*
 * Método privado que aplica una función de hash a la clave.
 */ 
int Hash::hashInterno(char* clave)
{
	int acumulador = 0;
	int largo = strlen(clave);
	
	// Necesito que largo sea par. Para eso, si es impar le sumo 1, con lo cual
	// se usa el "\0" como parte de la clave.
	if (largo % 2 == 1)
		largo++;
	for(int j=0; j<largo; j+=2)
		acumulador = (acumulador + 100*clave[j] + clave[j+1]) % 19937 ;
		// Se usa el número 19937 por ser primo, lo cual genera una mejor distribución
		// de las claves. Además evita un overflow del acumulador.
	
	return acumulador;
}

int Hash::aplicarHash(char *clave)
{
	int aux = 0;
	int valorHash = hashInterno(clave);
	int ultimoBit;
	
	// Doy vuelta los bits, y tomo los 5 menos significativos.
	for (int i = 0; i< 5; i++)
	{
		aux = aux << 1;
		ultimoBit = valorHash & 1;
		aux = aux | ultimoBit;
		valorHash = valorHash >> 1;
	}
	return aux;
}

/*
 * Este método se utiliza para hacer una operación de alta en el archivo.
 * Si el registro se puede insertar devuelve OK; si ya existe un registro
 * con la misma clave, devuelve DUPLICATED, y si no hay lugar en el bucket
 * correspondiente para insertarlo, devuelve FAIL.
 */
int Hash::insertarRegistro(char* registro, char* clave)
{	
	int nroBloque = aplicarHash(clave) % tabla->getTamanio();
	Bucket * bucket;
	bucket->leer(tabla->getDireccionBucket(nroBloque));
	
	unsigned short aux;
	if (bucket->buscarRegistro(listaParam, clave, aux)==)
		return DUPLICATED; // ya existe un registro con esa clave.
}

int main()
{
	return 0;
}
