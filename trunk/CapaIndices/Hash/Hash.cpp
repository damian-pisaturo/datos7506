#include "Hash.h"

Hash::Hash(ArchivoIndiceHash *arch)
{
	archivo = arch;
	//TODO:: Agregar nodo con tam de dsipersión a la lista de parámetros
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
 * Si el registro es variable "registro" contendrá la long del mismo en sus primeros 2 bytes
 */
int Hash::insertarRegistro(char* registro, char* clave)
{	
	// Se aplica la función de hash para saber en que bucket se debe insertar.
	int nroBucket = aplicarHash(clave) % tabla->getTamanio();
	
	// Se obtiene el bucket donde hay que insertar el registro.
	Bucket * bucket = new Bucket(archivo,tabla->getDireccionBucket(nroBucket));
	
	// Se busca si el registro ya existe.
	unsigned short aux;
	if (bucket->buscarRegistro(listaParam, (void*)clave, &aux))
		// ya existe un registro con esa clave.
		return DUPLICATED; 
	
	// Se obtiene la longitud del registro independientemente de si es variable o fija.
	unsigned short longReg = bucket->getTamanioRegistros(listaParam,registro);
	
	if (bucket->verificarEspacioDisponible(longReg,bucket->getEspacioLibre()))
	{
		bucket->altaRegistro(listaParam,registro);
	}
	else
	// Si el registro no entra en el bucket hay que crear un nuevo bucket.
	{
		//Bucket * nuevoBucket = new Bucket(tabla->getNumProximoBucket(),bucket->getTamDispersion(),archivo);
		// TODO redistribuir los elementos del bucket donde hubo overflow.
		
	}
	return 0;
	
}


