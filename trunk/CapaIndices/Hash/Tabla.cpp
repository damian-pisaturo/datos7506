///////////////////////////////////////////////////////////////////////////
//	Archivo   : Tabla.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Tabla.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Tabla.h"
 
///////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////
Tabla::Tabla()
{
	tamanio = 1;
}

Tabla::Tabla(string nombreArchivo, IndiceHashManager* arch, unsigned int tamBucket)
{
	archivo = arch;
	archivo->leerTabla(&this->tamanio,this->nroBucket);
	// Si la tabla no existe, en tamanio devuelve 0.
	// En ese caso, se crea una tabla.
	if (tamanio == 0)
		this->crear(nombreArchivo,tamBucket);
}

Tabla::~Tabla()
{
	archivo->escribirTabla(this->tamanio,this->nroBucket);
	if(nroBucket)
		delete[] nroBucket;
}

///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////

/*
 * Este método crea una tabla para un nuevo indice.
 * Pide a la capa física la creación de un archivo de datos con 1 solo bucket, 
 * y un archivo con la tabla.
 **/
void Tabla::crear(string nombreArchivo, unsigned int tamanioBloque)
{
	this->tamanio = 1;
	
	// Setea en la posición 0 de la tabla al bucket 0.
	this->setNroBucket(0,0);
	
	// Crea un archivo de datos con un bucket vacío y lo escribe a disco.
	Bucket * bucket = new Bucket(0, 1, tamanioBloque);
	archivo->escribirBloque(bucket);
	
	// Escribe la tabla a disco.
	archivo->escribirTabla(this->tamanio,this->nroBucket);
}

/*
 * Este método reorganiza la tabla luego de una inserción que haya provocado un overflow.
 * Para eso si el tamaño de dispersión del bloque es igual al tamaño de la tabla,
 * duplica esta última y sino solo actualiza la referencia de "posicion"
 **/
void Tabla::reorganizarTabla(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket){
	
	if((tamDispActualizado/2)== tamanio){
		duplicarTabla();
		nroBucket[posicion] = nuevoNroBucket;
	}
	else
		actualizarReferencias(tamDispActualizado,posicion,nuevoNroBucket);
}

/*
 * Este método se fija si ante la eliminación del ultimo registro en un bucket
 * es posible reducir la tabla a la mitad de sus entradas.
 * De ser posible, lo hace.
 **/
void Tabla::considerarReduccion()
{
	bool reducir = true;
	
	unsigned int i=0;
	while ((i < (getTamanio()/2)) && reducir)
	{
		if (getNroBucket(i) != getNroBucket(getTamanio()/2 + i))
			reducir = false;
		i++;
	}
	
	if (reducir)
		this->reducirTabla();
}

/*
 * Este método es llamado cuando el tamaño de dispersión de un bucket en el que hubo overflow
 * es igual al tamaño de la tabla, para buscar al otro bucket de la tabla que tiene igual número
 * de dispersión.
 **/
unsigned int Tabla::buscarBucketIgualDispersion(int posicion,unsigned short tamDispersion)
{
	// Se divide el tamaño de dispersión.
	tamDispersion = tamDispersion/2;
	
	// Se calcula la posicion del bucket que tiene igual dispersión. (Su buddy Bucket).
	int posicionNueva = posicion - tamDispersion;
	if (posicionNueva < 0)
		posicionNueva = posicion + tamDispersion;
	
	// Se obtiene su número de bucket.
	unsigned int nroBucket = getNroBucket(posicionNueva);
	
	return(nroBucket);
}

/* 
 * Devuelve el número de bucket dentro del archivo de datos que se encuentra 
 * referenciado por la posición "posicionTabla" dentro de la tabla de dispersión.
 **/
unsigned int Tabla::getNroBucket(int posicionTabla)
{
	return nroBucket[posicionTabla];
}

/*
 * Setea el número de bucket dentro del archivo al que se hace referencia en
 * la entrada "posicion" de la tabla.
 **/
void Tabla::setNroBucket(int posicion, unsigned int nro)
{
	nroBucket[posicion] = nro;
}

/*
 * Devuelve la cantidad de entradas de la tabla.
 **/
unsigned int Tabla::getTamanio()
{
	return tamanio;
}

///////////////////////////////////////////////////////////////////////
// Metodos privados
///////////////////////////////////////////////////////////////////////

/*
 * Este método reduce la tabla de dispersión a la mitad de sus entradas.
 * Es precondición que la tabla esté espejada para obtener una reducción
 * efectiva; de otra forma se obtendrá una tabla truncada.
 **/
void Tabla::reducirTabla()
{
	tamanio = tamanio/2;
	unsigned int* tabla = new unsigned int [getTamanio()];
	for (unsigned int i=0; i< (getTamanio());i++)
		tabla[i] = getNroBucket(i);
	
	delete nroBucket;
	nroBucket = tabla;
}


/*
 * Este metodo se encarga de duplicar la tabla de dispersión. Para esto, lo que
 * hace es espejar las entradas de la misma.
 **/
void Tabla::duplicarTabla(){
	// Se crea una lista de números de buckets auxiliar del doble del tamaño de la lista actual.
	unsigned int * nroBuckets = new unsigned int[tamanio*2];
	
	// Se copia 2 veces el contenido de la lista actual.
	memcpy(nroBuckets,nroBucket,tamanio);
	memcpy(&nroBuckets[tamanio],nroBucket,tamanio);
	
	// Se actualiza la tabla.
	delete nroBucket;
	nroBucket = nroBuckets;
	
	// Se actualiza el tamaño de la tabla.
	tamanio *= 2;
}

/*
 * Este método actualiza las referencias de la tabla luego de una inserción con overflow,
 * donde el tamaño de dispersión del bucket donde se debe hacer la misma es menor que el tamaño
 * de la tabla de dispersión.
 **/
void Tabla::actualizarReferencias(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket){
	
	// Se va saltando de a "tamDispActualizado" posiciones a partir de "posicion" en la tabla
	// haciendo que se apunte a nuevoNroBucket.

	int i;
	// Se recorre desde posicion para arriba.
	for(i = posicion;i >= 0;i -= tamDispActualizado)
		nroBucket[i] = nuevoNroBucket;
	
	// Se recorre desde posicion para abajo.
	for(i = posicion; (unsigned)i<tamanio;i += tamDispActualizado)
		nroBucket[i] = nuevoNroBucket;
}









