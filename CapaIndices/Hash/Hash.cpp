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
	int posicion = aplicarHash(clave) % tabla->getTamanio();
	
	// Se obtiene el bucket donde hay que insertar el registro.
	int numBucket = tabla->getNroBucket(posicion);
	
	// Levanta un bucket ya existente del disco y carga sus datos.
	Bucket * bucket = new Bucket(archivo,numBucket);
	
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
		bucket->incrementarCantRegistros();
	}
	else
	// Hay OVERFLOW
	// Si el registro no entra en el bucket hay que crear un nuevo bucket.
	{
		// Se duplica el tamaño de dispersión del bucket que se divide.
		bucket->setTamDispersion(bucket->getTamDispersion()*2);
		
		// Se crea un nuevo bucket vacío. Se devuelve en numBucketNuevo el número del mismo. 
		unsigned int numBucketNuevo;
		Bucket * nuevoBucket = new Bucket(&numBucketNuevo,bucket->getTamDispersion(),archivo);
		
		// Duplica la tabla o actualiza sus referencias dependiendo del tamaño de dispersión 
		// duplicado del bucket donde se produjo overflow.
		tabla->reorganizarTabla(bucket->getTamDispersion(), posicion, numBucketNuevo); // TODO: implementar.
		
		redistribuirElementos(bucket,nuevoBucket);
		
		int result = insertarRegistro(registro, clave);
		return result;

		
	}
	return OK;
}

void Hash::redistribuirElementos(Bucket* bucket, Bucket* nuevoBucket)
{
	unsigned short longReg;
	unsigned short offsetReg = bucket->getOffsetToRegs();
	char * datos = bucket->getDatos();
	char * clave;
	int posicion;
	unsigned int nroBucket;
	
	// Se crea un bucket auxiliar para poner los registros que deberían quedar en bucket.
	// TODO Cambiar constructor a uno q no toque disco.
	Bucket * bucketAux = new Bucket(bucket->getNroBloque(),bucket->getTamanioBloque(),bucket->getTamDispersion());
	
	// Se recorren los registros reubicándolos.
	for(int i = 0; i< bucket->getCantRegs(); i++)
	{
		// Obtengo el tamaño del registro.
		longReg = bucket->getTamanioRegistros(listaParam,&datos[offsetReg]);
		
		// Obtengo la clave primaria.
		clave = bucket->getClavePrimaria(listaParam, &datos[offsetReg]);
		
		posicion = aplicarHash(clave) % tabla->getTamanio(); 
		nroBucket = tabla->getNroBucket(posicion);
		
		// Se decide en cual de los 2 buckets se inserta el registro.
		if (nroBucket == nuevoBucket->getNroBloque())
			nuevoBucket->altaRegistro(listaParam,&datos[offsetReg]);
		else bucketAux->altaRegistro(listaParam,&datos[offsetReg]);
		
		offsetReg += longReg;
		
		list<nodoLista>::const_iterator it = listaParam.begin();
		nodoLista regAtribute = *it;
		
		// Se obtiene el tipo de atributo del registro.
		int tipo = regAtribute.tipo;

		if(tipo == TipoDatos::TIPO_VARIABLE)
			offsetReg += Tamanios::TAMANIO_LONGITUD;
		
	}
	delete bucket;
	bucket = bucketAux;
}

int Hash::eliminarRegistro(char* clave)
{
	int posicion = aplicarHash(clave) % tabla->getTamanio();
	unsigned int nroBucket = tabla->getNroBucket(posicion);
	
	Bucket * bucket = new Bucket(archivo, nroBucket);	
	unsigned short cantRegs = bucket->getCantRegs();
	
	if (cantRegs > 1)
		// Si hay más de 1 registro en el bucket simplemente se elimina el registro pedido.
		return bucket->bajaRegistro(listaParam,clave);
	
	// Si hay un solo registro se debe verificar si se puede borrar el bucket del archivo.
	// Primero se verifica que el registro que se encuentra en el bucket efectivamente sea
	// el que se quiere eliminar.
	unsigned short aux;
	if (bucket->buscarRegistro(listaParam, (void *) clave,&aux))
	{	
		// Si el tamaño de dispersión del bucket coincide con el tamaño de la tabla, se elimina el bucket.
		if (bucket->getTamDispersion() == tabla->getTamanio())
		{			
			// Se llama a marcar al bucket como vacío en el archivo.
			bucket->eliminarBucket();
			
			// Se renueva la referencia de la tabla que antes apuntaba al bucket que se eliminó.
			unsigned int nroBucket2 = tabla->buscarBucketIgualDispersion(posicion, bucket->getTamDispersion());
			tabla->setNroBucket(posicion,nroBucket2);
			
			// En caso de que deba realizarse una reducción a la mitad del tamaño de la tabla se lo hace.
			tabla->considerarReduccion();
			return OK;
		}
		else
			// Si el tamaño de dispersión es != al tamaño de la tabla, sólo se elimina el registro.
			return bucket->bajaRegistro(listaParam,clave);
	}
	else
		return NO_ENCONTRADO;
}


