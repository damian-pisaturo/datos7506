///////////////////////////////////////////////////////////////////////////
//	Archivo   : Tabla.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase Tabla.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#ifndef TABLA_H_
#define TABLA_H_
#include <cstring>
#include "Bucket.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Tabla (Implementa una tabla para manejar la dispersion extensible)
//////////////////////////////////////////////////////////////////////////
class Tabla
{
private: 
	//////////////////////////////////////////////////////////////////////
	// Atributos
	//////////////////////////////////////////////////////////////////////
	unsigned int tamanio;			// Contiene la cantidad de entradas de la tabla.
	unsigned int * nroBucket;		// Contiene una lista de buckets.
	IndiceHashManager * archivo;
	
public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	Tabla();
	Tabla(char* nombreArchivo, IndiceHashManager* archivo);
	virtual ~Tabla();
	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
	
	/*
	 * Este método crea una tabla para un nuevo indice.
	 * Pide a la capa física la creación de un archivo de datos con 1 solo bucket, 
	 * y un archivo con la tabla.
	 **/
	void crear(char* nombreArchivo, unsigned int tamanioBloque);
	
	/*
	 * Este método reorganiza la tabla luego de una inserción que haya provocado un overflow.
	 * Para eso si el tamaño de dispersión del bloque es igual al tamaño de la tabla,
	 * duplica esta última y sino solo actualiza la referencia de "posicion"
	 **/
	void reorganizarTabla(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket);

	/*
	 * Este método se fija si ante la eliminación del ultimo registro en un bucket
	 * es posible reducir la tabla a la mitad de sus entradas.
	 * De ser posible, lo hace.
	 **/
	void considerarReduccion();
	
	/*
	 * Este método es llamado cuando el tamaño de dispersión de un bucket en el que hubo overflow
	 * es igual al tamaño de la tabla, para buscar al otro bucket de la tabla que tiene igual número
	 * de dispersión.
	 **/
	unsigned int buscarBucketIgualDispersion(int posicion,unsigned short tamDispersion);

	/* Devuelve el número de bucket dentro del archivo de datos que se encuentra 
	 * referenciado por la posición "posicionTabla" dentro de la tabla de dispersión.
	 **/
	unsigned int getNroBucket(int posicion);
	
	/*
	 * Setea el número de bucket dentro del archivo al que se hace referencia en
	 * la entrada "posicion" de la tabla.
	 **/
	void setNroBucket(int posicion, unsigned int nro);
	
	/*
	 * Devuelve la cantidad de entradas de la tabla.
	 **/
	unsigned int getTamanio();

private:
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////	
	
	/*
	 * Este método reduce la tabla de dispersión a la mitad de sus entradas.
	 * Es precondición que la tabla esté espejada para obtener una reducción
	 * efectiva; de otra forma se obtendrá una tabla truncada.
	 **/
	void reducirTabla();
	
	/*
	 * Este metodo se encarga de duplicar la tabla de dispersión. Para esto, lo que
	 * hace es espejar las entradas de la misma.
	 **/
	void duplicarTabla();
	
	/*
	 * Este método actualiza las referencias de la tabla luego de una inserción con overflow,
	 * donde el tamaño de dispersión del bucket donde se debe hacer la misma es menor que el tamaño
	 * de la tabla de dispersión.
	 **/
	void actualizarReferencias(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket);
};

#endif /*TABLA_H_*/
