#ifndef TABLA_H_
#define TABLA_H_

class Tabla
{
private:
	int tamanio;
	int* direccionesBuckets;
	
public:
	Tabla();
	virtual ~Tabla();
	void crear(char* nombreArchivo);
};

#endif /*TABLA_H_*/
