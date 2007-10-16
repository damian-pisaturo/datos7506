#ifndef BUCKET_H_
#define BUCKET_H_

class Bucket
{
private:
	int tamDispersion;
	int cantRegs;
	char* datos;
	int tamanio; 
	
public:
	Bucket(int tamanioBucket);
	virtual ~Bucket();
	void leer(int offset);
	void escribir(int offset);
};

#endif /*BUCKET_H_*/
