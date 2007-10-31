#ifndef TABLA_H_
#define TABLA_H_
#include <cstring>

class Tabla
{
private: 
	unsigned int tamanio;			// Contiene la cantidad de entradas de la tabla.
	unsigned int * nroBucket;		// Contiene una lista de buckets.
	
	void reducirTabla();
	
public:
	Tabla();
	Tabla(char* nombreArchivo);
	virtual ~Tabla();
	void crear(char* nombreArchivo);
	unsigned int getTamanio();
	unsigned int getNroBucket(int posicion);
	void setNroBucket(int posicion, unsigned int nro);
	void considerarReduccion();
	

	void reorganizarTabla(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket);
	void duplicarTabla();
	void actualizarReferencias(unsigned short tamDispActualizado, int posicion, unsigned int nuevoNroBucket);
	unsigned int buscarBucketIgualDispersion(int posicion,unsigned short tamDispersion);
};

#endif /*TABLA_H_*/
