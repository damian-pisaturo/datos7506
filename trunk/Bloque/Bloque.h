#ifndef BLOQUE_H_
#define BLOQUE_H_

class Bloque
{
private:
	int numero;
	int tamanio;
	char *datos;
public:
	Bloque();
	virtual ~Bloque();
};

#endif /*BLOQUE_H_*/
