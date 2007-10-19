#ifndef TABLA_H_
#define TABLA_H_

class Tabla
{
private:
	int tamanio;
	int* direccionesBuckets;
	
public:
	Tabla();
	Tabla(char* nombreArchivo);
	virtual ~Tabla();
	void crear(char* nombreArchivo);
	int getTamanio();
	int getDireccionBucket(int i);
};

#endif /*TABLA_H_*/
