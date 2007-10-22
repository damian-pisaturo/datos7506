#ifndef REGISTROV_H_
#define REGISTROV_H_

class RegistroV
{
private:
	int tamanio;
	char* datos;
	int longitudClave;
	
public:
	RegistroV(char * contenido,int longitud);
	virtual ~RegistroV();
	char* getClave();
	int getLongitudClave();
};

#endif /*REGISTROV_H_*/
