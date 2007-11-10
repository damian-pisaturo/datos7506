///////////////////////////////////////////////////////////////////////////
//	Archivo   : Hash.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Hash.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Hash.h"

///////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////
Hash::Hash(IndiceHashManager *arch, ListaNodos * lista, unsigned int tamBucket)
{
	archivo = arch;
	listaParam = lista;
	
	tabla = new Tabla(archivo,tamBucket);
}

Hash::~Hash()
{
	delete tabla;
}

///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////

/*
 * Este método se utiliza para hacer una operación de alta en el archivo.
 * Si el registro se puede insertar devuelve HASH_OK; si ya existe un registro
 * con la misma clave, devuelve DUPLICATED.
 * En caso de que el registro no entre en el bucket correspondiente, se 
 * toman las medidas necesarias para hacer extensible la función de hash.
 * Si el registro es variable "registro" contendrá su longitud en los 
 * primeros bytes.
 **/
int Hash::insertarRegistro(char* registro, Clave &clave)
{	

	// Se aplica la función de hash para saber en que bucket se debe insertar.	
	int posicion = aplicarHash(clave) % tabla->getTamanio();
	
	
	// Se obtiene el bucket donde hay que insertar el registro.
	int numBucket = tabla->getNroBucket(posicion);
	
	// Levanta un bucket ya existente del disco y carga sus datos.
	Bucket * bucket = new Bucket(archivo, numBucket);
	
	// Se busca si el registro ya existe.
	unsigned short aux;
	
	if (bucket->Bloque::buscarRegistro(listaParam, clave, &aux))
	{
		delete bucket;
		// ya existe un registro con esa clave.
		return DUPLICATED; 
	}
	
	// Se obtiene la longitud del registro independientemente de si es variable o fija.
	unsigned short longReg = bucket->getTamanioRegistros(listaParam, registro);
	
	// si el registro es variable se agrega a su longitud los bytes que contienen la misma.
	if (esRegistroVariable())
		longReg += Tamanios::TAMANIO_LONGITUD;
	
	if (bucket->verificarEspacioDisponible(longReg, bucket->getEspacioLibre()))
	{
		// Da de alta el registro en el bloque y persiste en disco.
		bucket->altaRegistro(listaParam,registro);
		bucket->incrementarCantRegistros();
		bucket->actualizarEspLibre();
		archivo->escribirBloque(numBucket, bucket);
		
		delete bucket;
	
		return HASH_OK;
	}
	else
	// Hay OVERFLOW
	// Si el registro no entra en el bucket hay que crear un nuevo bucket.
	{
		// Se duplica el tamaño de dispersión del bucket que se divide.
		bucket->setTamDispersion(bucket->getTamDispersion()*2);
		
		// Se crea un nuevo bucket vacío. Se devuelve en numBucketNuevo el número del mismo. 
		unsigned int numBucketNuevo = 0;
		
		// Se crea un bucket vacío en memoria y se lo graba en disco.
		Bucket * nuevoBucket = new Bucket(numBucketNuevo,bucket->getTamDispersion(),archivo->getTamanioBloque());
		numBucketNuevo = archivo->escribirBloque(nuevoBucket);
		
		// Duplica la tabla o actualiza sus referencias dependiendo del tamaño de dispersión 
		// (ya duplicado) del bucket donde se produjo overflow.
		tabla->reorganizarTabla(bucket->getTamDispersion(), posicion, nuevoBucket->getNroBloque()); 
		
		redistribuirElementos(bucket, nuevoBucket);
		
		// Guarda en disco la redistribución de los elementos.
		archivo->escribirBloque(bucket->getNroBloque(), bucket);
		archivo->escribirBloque(nuevoBucket->getNroBloque(), nuevoBucket);		
		
		delete bucket;
		delete nuevoBucket;
		
		// Intenta nuevamente la inserción del registro.
		return insertarRegistro(registro, clave);
		
	}
}

/* Este método se utiliza para hacer una operación de baja en el archivo.
 * Si el registro se puede eliminar devuelve HASH_OK, si no existe el registro
 * a eliminar, se devuelve NO_ENCONTRADO.
 * En caso de que el bucket quede vacío, se considera la posibilidad de 
 * disminuir el tamaño de la tabla de hash.
 **/
int Hash::eliminarRegistro(Clave &clave)
{
	// Se aplica la función de hash para ver en que bucket se debe buscar el
	// registro a eliminar.
	int posicion = aplicarHash(clave) % tabla->getTamanio();
	unsigned int nroBucket = tabla->getNroBucket(posicion);
	
	// Se levanta dicho bucket.
	Bucket * bucket = new Bucket(archivo, nroBucket);	
	unsigned short cantRegs = bucket->getCantRegs();
	
	if (cantRegs > 1)
	{
		// Si hay más de 1 registro en el bucket simplemente se elimina el registro pedido.
		int result = bucket->bajaRegistro(listaParam,clave);
		// Se actualiza la baja en el archivo.
		archivo->escribirBloque(nroBucket, bucket);
		delete bucket;
		return result;
	}
	
	// Si hay un solo registro se debe verificar si se puede borrar el bucket del archivo.
	// Primero se verifica que el registro que se encuentra en el bucket efectivamente sea
	// el que se quiere eliminar.
	unsigned short aux;
	if (bucket->buscarRegistro(listaParam, clave, &aux))
	{	
		// Si el tamaño de dispersión del bucket coincide con el tamaño de la tabla, se elimina el bucket.
		if (bucket->getTamDispersion() == tabla->getTamanio())
		{				
			//bucket->bajaRegistro(listaParam, clave);
			//archivo->escribirBloque(nroBucket, bucket);
			
			// Se llama a marcar al bucket como vacío en el archivo.
			archivo->eliminarBloque(nroBucket);
			
			// Se renueva la referencia de la tabla que antes apuntaba al bucket que se eliminó.
			unsigned int nroBucket2 = tabla->buscarBucketIgualDispersion(posicion, bucket->getTamDispersion());
		
			
			tabla->setNroBucket(posicion,nroBucket2);
		
			// Se actualiza el tamaño de dispersión del bloque y se lo guarda en archivo.
			dividirDispersion(nroBucket2);
			
			// En caso de que deba realizarse una reducción a la mitad del tamaño de la tabla se lo hace.
			tabla->considerarReduccion();
			delete bucket;
			return HASH_OK;
		}
		else
		{
			// Si el tamaño de dispersión es != al tamaño de la tabla, sólo se elimina el registro.
			int result = bucket->bajaRegistro(listaParam,clave);
			// Se actualiza la baja en disco.
			archivo->escribirBloque(nroBucket, bucket);
			delete bucket;
			return result;
		}	
	}
	else
	{
		delete bucket;
		return NO_ENCONTRADO;
	}
}

bool Hash::modificarRegistro(Clave &claveVieja, Clave &claveNueva, char* registroNuevo)
{
	int result = this->eliminarRegistro(claveVieja);
	if (result == HASH_OK) {
		this->insertarRegistro(registroNuevo, claveNueva);
		return true;
	}
	return false;
}

/*
 * Recupera un registro a partir de una clave 
 **/

void Hash::recuperarRegistro(Clave &clave, char *registro){
	// Se aplica la función de hash para ver en que bucket se debe buscar el
	// registro a recuperar.
	int posicion = aplicarHash(clave) % tabla->getTamanio();
	unsigned int nroBucket = tabla->getNroBucket(posicion);
		
	// Se levanta dicho bucket.
	Bucket * bucket = new Bucket(archivo, nroBucket);	
	unsigned short offsetToReg;
	
	// Si no encuentro el registro buscado devuelvo false. Pero si lo encuentro,
	// tengo el offset al mismo en offsetToReg.
	if (!(bucket->buscarRegistro(listaParam, clave,&offsetToReg)))
		return;
	
	unsigned short longReg;	
	const char *datos = bucket->getDatos();
	
	// Hay que hacer una copia que no sea const de datos.
	char* copiaDatos = new char[archivo->getTamanioBloque() + 1];
	memcpy(copiaDatos,datos,archivo->getTamanioBloque());
	*(copiaDatos + 1)= 0;
	
		// Obtiene la longitud.
	longReg = bucket->getTamanioRegistros(listaParam,&copiaDatos[offsetToReg]);

	
	list<nodoLista>::const_iterator it;
	it = listaParam->begin();
	
	if (it->tipo == TipoDatos::TIPO_VARIABLE){
		registro = new char[Tamanios::TAMANIO_LONGITUD + longReg];
		memcpy(registro,&datos[offsetToReg],Tamanios::TAMANIO_LONGITUD + longReg);
	}
	else{
		registro = new char[longReg];
		memcpy(registro,&datos[offsetToReg],longReg);
	}
}

///////////////////////////////////////////////////////////////////////
// Metodos privados
///////////////////////////////////////////////////////////////////////

/*
 * Este método aplica una función de dispersión a la clave.
 **/ 
int Hash::aplicarHash(Clave &clave)
{
	int aux = 0;
	void ** claveHash = clave.getValorParaHash();
	char* claveSerializada = serializarClave(claveHash);
	
	int valorHash = hashInterno(claveSerializada);
	
	int ultimoBit;
	
	// Doy vuelta los bits, y tomo los 5 menos significativos.
	for (int i = 0; i< 5; i++)
	{
		aux = aux << 1;
		ultimoBit = valorHash & 1;
		aux = aux | ultimoBit;
		valorHash = valorHash >> 1;
	}
	
	delete[] claveSerializada;
	delete[] claveHash;
	
	return aux;
}

/*
 * Método utilizado internamente cuando se aplica la función de dispersión a una clave.
 **/
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

/*
 * Este método se encarga de redistribuir los registros contenidos en bucket 
 * entre este mismo y bucketNuevo.
 **/
void Hash::redistribuirElementos(Bucket* &bucket, Bucket* &nuevoBucket)
{
	unsigned short longReg = 0;
	unsigned short offsetReg = bucket->getOffsetADatos();
	char * datos = bucket->getDatos();
	
	Clave *clave = NULL;
	int posicion = 0;
	unsigned int nroBucket = 0;
	
	// Se crea un bucket auxiliar para poner los registros que deberían quedar en bucket.
	Bucket * bucketAux = new Bucket(bucket->getNroBloque(),bucket->getTamDispersion(), bucket->getTamanioBloque());
	
	ListaNodos::const_iterator it = listaParam->begin();
	nodoLista regAtribute = *it;
	
	// Se obtiene el tipo de atributo del registro.
	int tipo = regAtribute.tipo;

	// Se recorren los registros reubicándolos.
	for(int i = 0; i< bucket->getCantRegs(); i++)
	{		
		// Obtengo el tamaño del registro.	
		longReg = bucket->getTamanioRegistros(listaParam,&datos[offsetReg]);
		
		// Obtengo la clave primaria.
		clave = bucket->getClavePrimaria(listaParam, &datos[offsetReg]);
		
		posicion = aplicarHash(*clave) % tabla->getTamanio(); 
		nroBucket = tabla->getNroBucket(posicion);
		
		// Se decide en cual de los 2 buckets se inserta el registro.
		if (nroBucket == nuevoBucket->getNroBloque())
		{
			nuevoBucket->altaRegistro(listaParam, &datos[offsetReg]);
			nuevoBucket->incrementarCantRegistros();
		}
		else
		{
			bucketAux->altaRegistro(listaParam, &datos[offsetReg]);
			bucketAux->incrementarCantRegistros();
		}
		
		offsetReg += longReg;
		
		if(tipo == TipoDatos::TIPO_VARIABLE)
			offsetReg += Tamanios::TAMANIO_LONGITUD;
	}
	
	bucketAux->actualizarEspLibre();
	nuevoBucket->actualizarEspLibre();
	
	delete bucket;
	
	bucket = bucketAux;	
}

/*
 * Este método se engarga de dividir el tamaño de dispersión del bucket nroBucket
 * y actualizarlo en el archivo.
 **/
void Hash::dividirDispersion(unsigned int nroBucket)
{
	// Se levanta el bucket de disco.
	Bucket* bucket = new Bucket(archivo, nroBucket);
	
	// Se divide el tamaño de dispersión.
	bucket->setTamDispersion(bucket->getTamDispersion()/2);
	archivo->escribirBloque(nroBucket, bucket);
	
	delete bucket;
}

/* Si el indice de hash está trabajando con registros de tipo variable retorna true;
 * de lo contrario retorna false.
 **/ 
bool Hash::esRegistroVariable()
{
	list<nodoLista>::const_iterator it;
	it = listaParam->begin();
	
	if (it->tipo == TipoDatos::TIPO_VARIABLE)
		return true;
	
	return false;
}

char* Hash::serializarClave(void** claveVoid)
{
	ListaNodos::const_iterator it;
	it = listaParam->begin();
	
	int tipo;
	
	unsigned char cantClaves = it->cantClaves;
	
	// En este vector se pondrá la longitud de cada clave.
	int* vectorTamanios = new int[cantClaves];
	int tamanio = 0;
	int i = 0;  // recorre las posiciones del vectorTamanios.
	
	for( ++it ;it != listaParam->end() ; ++it){
		tipo = it->tipo;
		if (it->pk == "true"){
			
			switch(tipo){
			
			case TipoDatos::TIPO_BOOL:
				vectorTamanios[i] = sizeof(bool);
				tamanio += sizeof(bool);
				break;
			case TipoDatos::TIPO_CHAR:
				vectorTamanios[i] = sizeof(char);
				tamanio += sizeof(char);
				break;
			case TipoDatos::TIPO_SHORT:
				vectorTamanios[i] = sizeof(short int);
				tamanio += sizeof(short int);
				break;
			case TipoDatos::TIPO_ENTERO:
				vectorTamanios[i] = sizeof(int);
				tamanio += sizeof(int);
				break;
			case TipoDatos::TIPO_FLOAT:
				vectorTamanios[i] = sizeof(float);
				tamanio += sizeof(float);
				break;
			case TipoDatos::TIPO_FECHA:
				vectorTamanios[i] = Tamanios::TAMANIO_FECHA;
				tamanio += Tamanios::TAMANIO_FECHA;
				break;
			case TipoDatos::TIPO_STRING:
				vectorTamanios[i] = strlen((char*)claveVoid[i]);
				tamanio += vectorTamanios[i];
				break;
			}
			i++;
		}
	}
		
	char* clave = new char[tamanio + 1];
	unsigned char posicion = 0;
	
	for(unsigned char i=0; i<cantClaves; i++){
		memcpy(&clave[posicion], claveVoid[i],vectorTamanios[i]);
		posicion += vectorTamanios[i];
	}
	
	*(clave + tamanio) = 0;
	
	delete []vectorTamanios;
	
	return clave;
		
}

