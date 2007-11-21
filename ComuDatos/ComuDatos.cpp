#include "ComuDatos.h"


#define CORRIMIENTOARGUMENTO 3
	 
int contarArgv(char **argv)
{
	int contador;
	for(contador = 0; argv[contador] != NULL; contador++);
	return contador;
}

ComuDatos::ComuDatos()
{
	this->id_procesoHijo = 0;
	this->fd_pipeP       = 0; 
	this->fd_pipeH       = 0; 
	this->cantParametros = 0;
}

ComuDatos::ComuDatos(string nombreEjecutable)
{
	this->id_procesoHijo = 0;
	this->fd_pipeP       = 0; 
	this->fd_pipeH       = 0; 
	this->cantParametros = 0;	
	this->nombreProceso  = nombreEjecutable;
}

ComuDatos::ComuDatos(char** argv)
{
	int contador = contarArgv(argv);
	
	if (contador >= CORRIMIENTOARGUMENTO){
		this->fd_pipeP = open(argv[2], O_WRONLY);
		if (this->fd_pipeP<0) return;
		
		this->fd_pipeH = open(argv[1], O_RDONLY);
		if (this->fd_pipeH < 0){
			close(this->fd_pipeP);
			perror("argv[]");
			return;	
		}	
		this->parametrosProceso.resize(contador-CORRIMIENTOARGUMENTO);
		for (int i = 0; i<(contador-CORRIMIENTOARGUMENTO); i++)
			this->parametrosProceso.at(i) = argv[i+CORRIMIENTOARGUMENTO];
	}
	
}

//! parametros tiene que ser NULL o tener un puntero a NULL en el último lugar del vector.
int ComuDatos::lanzar()
{
	int resultado = OK;
	unsigned int paramSize = this->parametrosProceso.size();
	char** argumentos = new char*[paramSize + CORRIMIENTOARGUMENTO + 1];
	
	if (this->nombreProceso.length() == 0)
		resultado = ERROR_EJECUCION; 
	else{		
		string pipeLee = this->nombreProceso + "_ComuDatosH";
		string pipeEscribe = this->nombreProceso + "_ComuDatosP";
		
		mkfifo(pipeLee.c_str(), 0666);
		mkfifo(pipeEscribe.c_str(), 0666);
		
		// Parametro 1 lectura y 2 escritura hijo.
		argumentos[1] = new char[sizeof(char)*(pipeEscribe.length() + 1)];
		strcpy(argumentos[1], pipeEscribe.c_str());
		argumentos[2] = new char[sizeof(char)*(pipeLee.length() + 1)];
		strcpy(argumentos[2], pipeLee.c_str());
		
		argumentos[0] = new char[sizeof(char)*(this->nombreProceso.length()+1)];
		strcpy(argumentos[0], this->nombreProceso.c_str());
		
		for(unsigned int i = 3; i<(paramSize + CORRIMIENTOARGUMENTO); i++)
		{
			argumentos[i] = new char[sizeof(char)*(this->parametrosProceso.at(i-CORRIMIENTOARGUMENTO).length()+1)];
			strcpy(argumentos[i], this->parametrosProceso.at(i-CORRIMIENTOARGUMENTO).c_str());
		}
		
		argumentos[paramSize + CORRIMIENTOARGUMENTO] = NULL;
	
		this->id_procesoHijo = fork();
	
		if (this->id_procesoHijo > 0)
		{
			if (execv(argumentos[0], argumentos) == -1)
				resultado = ERROR_EJECUCION;
		}
		
		this->fd_pipeH = open(pipeLee.c_str(), O_RDONLY);
		this->fd_pipeP = open(pipeEscribe.c_str(), O_WRONLY);
		
		for(unsigned int i = 0; i < (paramSize + CORRIMIENTOARGUMENTO); i++){
			delete[] argumentos[i];
		}
	}
	
	if (argumentos)
		delete[] argumentos;
	
	return resultado;
}

void ComuDatos::ejecutable(string nombreEjecutable)
{
		this->nombreProceso = nombreEjecutable;
}

void ComuDatos::agregarParametro(int valorParametro, unsigned int posParametro)
{	
	char num[20];

	sprintf(num, "%d", valorParametro);

    if (posParametro >= this->parametrosProceso.size())
    {
    	this->parametrosProceso.resize(posParametro+1);
    }
    
    this->parametrosProceso.at(posParametro) = num;
    
}

void ComuDatos::agregarParametro(string valorParametro, unsigned int posParametro)
{
	if (posParametro+1 >= this->parametrosProceso.size())
	{
		this->parametrosProceso.resize(posParametro+1);	
	}
	this->parametrosProceso[posParametro] = valorParametro;
}

char ComuDatos::parametro(unsigned int posParametro, string &parametro)
{	
	char resultado = OK;
	
	if (posParametro < this->parametrosProceso.size())
		parametro = this->parametrosProceso[posParametro];
	else
		resultado = FIN_PARAMETROS;	
	
	return resultado;
}

char ComuDatos::parametro(unsigned int posParametro, int* parametro)
{	
	char resultado = OK;
	string aux;
	
	if (posParametro < this->parametrosProceso.size()){
		aux = this->parametrosProceso[posParametro];
		*parametro = atoi(aux.c_str());
	}else
		resultado = FIN_PARAMETROS;	
	
	return resultado;
}

char ComuDatos::parametro(unsigned int posParametro, unsigned char &parametro)
{	
	char resultado = OK;
	
	if (posParametro < this->parametrosProceso.size())
		parametro = (this->parametrosProceso[posParametro])[0];
	else
		resultado = FIN_PARAMETROS;	
	
	return resultado;
}


int ComuDatos::fd_leer()
{
	return this->fd_pipeH;
}

int ComuDatos::fd_escribir()
{
	return this->fd_pipeP;
}
 
char ComuDatos::escribir(const char* enviarDato, unsigned int size)
{
	char resultado = OK;
	
	if (size <= TOPE_ENVIAR_STRING) {
		write(this->fd_pipeP, enviarDato, size*sizeof(char));
	} else resultado = EXCEDE_TOPE;
	
	return resultado;
}

char ComuDatos::escribir(int enviarDato)
{
	char resultado = OK;
	
	if (sizeof(enviarDato) <= TOPE_ENVIAR_STRING)
	{
		write(this->fd_pipeP, &enviarDato, sizeof(int));
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}

char ComuDatos::escribir(short enviarDato)
{
	char resultado = OK;
	
	if (sizeof(enviarDato) <= TOPE_ENVIAR_STRING)
	{
		write(this->fd_pipeP, &enviarDato, sizeof(short));
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}

char ComuDatos::escribir(unsigned short enviarDato)
{
	char resultado = OK;
	
	if (sizeof(enviarDato) <= TOPE_ENVIAR_STRING)
	{
		write(this->fd_pipeP, &enviarDato, sizeof(unsigned short));
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}

char ComuDatos::escribir(unsigned int enviarDato)
{
	char resultado = OK;
	
	if (sizeof(enviarDato) <= TOPE_ENVIAR_STRING)
	{
		write(this->fd_pipeP, &enviarDato, sizeof(unsigned int));
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}

char ComuDatos::escribir(char enviarDato)
{
	char resultado = OK;
	
	if (sizeof(char) <= TOPE_ENVIAR_STRING)
	{
		write(this->fd_pipeP, &enviarDato, sizeof(char));
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}
char ComuDatos::escribir(const string &enviarDato)
{
	char resultado = OK;
	
	if (enviarDato.size() <= TOPE_ENVIAR_STRING)
	{
		write(this->fd_pipeP, enviarDato.c_str(), enviarDato.size()*sizeof(char));
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}

char ComuDatos::leer(unsigned int cantidad, char* s)
{
	char resultado = OK;

	if (cantidad <= TOPE_ENVIAR_STRING){		
		char leo[TOPE_ENVIAR_STRING];
		read(this->fd_pipeH, leo, cantidad);
		memcpy(s, leo, cantidad);		
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}

char ComuDatos::leer(int* i)
{
	int leo;
	read(this->fd_pipeH, &leo, sizeof(int));
	*i = leo;
	
	return OK;
}

char ComuDatos::leer(unsigned int* ui)
{
	unsigned int leo;
	read(this->fd_pipeH, &leo, sizeof(unsigned int));
	*ui = leo;
	
	return OK;
}

char ComuDatos::leer(short* s)
{		
	short leo;
	read(this->fd_pipeH, &leo, sizeof(short));
	*s = leo;
	
	return OK;	
}

char ComuDatos::leer(unsigned short* us)
{
	unsigned short leo;
	read(this->fd_pipeH, &leo, sizeof(unsigned short));
	*us = leo;
	
	return OK;
}

char ComuDatos::leer(char* c)
{
	char leo;
	read(this->fd_pipeH, &leo, sizeof(char));
	*c = leo;
	
	return OK;
}

char ComuDatos::leer(unsigned int cantidad, string &cadena)
{
	char resultado = OK;

	if (cantidad <= TOPE_ENVIAR_STRING){		
		char leo[TOPE_ENVIAR_STRING + 1];
		read(this->fd_pipeH, leo, cantidad);
		leo[cantidad] = 0;
		cadena = leo;
	}else resultado = EXCEDE_TOPE;
	
	return resultado;
}

void ComuDatos::liberarRecursos()
{
	if (this->nombreProceso.length() > 0)
	{
		string nombre = this->nombreProceso + "_ComuDatosH";		
		close(this->fd_pipeH);
		unlink(nombre.c_str());
		
		nombre = this->nombreProceso + "_ComuDatosP";
		close(this->fd_pipeP);
		unlink(nombre.c_str());
	}
}

ComuDatos::~ComuDatos()
{
	liberarRecursos();
}
