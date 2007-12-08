///////////////////////////////////////////////////////////////////////////
//	Archivo   : ComuDatos.cpp
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Librería para comunicaciones de procesos para Organización 
//		de Datos, cátedra Servetto.
///////////////////////////////////////////////////////////////////////////
//	Autor: Aleee
//  Fecha: 15 de Agosto de 2007
///////////////////////////////////////////////////////////////////////////

#include "ComuDatos.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ComuDatos
//			(Permite la comunicación entre procesos de capas).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	//	Constructores/Destructor
	///////////////////////////////////////////////////////////////////////

	ComuDatos::ComuDatos()
	{
		this->id_procesoHijo = 0;
		this->fd_pipeP       = 0; 
		this->fd_pipeH       = 0; 
		this->cantParametros = 0;
	}
	
	ComuDatos::ComuDatos(const string& nombreEjecutable)
	{
		this->id_procesoHijo = 0;
		this->fd_pipeP       = 0; 
		this->fd_pipeH       = 0; 
		this->cantParametros = 0;	
		this->nombreProceso  = nombreEjecutable;
	}
	
	ComuDatos::ComuDatos(char** argv)
	{
		unsigned char contador = this->contarArgv(argv);
		
		if (contador >= CORRIMIENTOARGUMENTO){
			
			this->fd_pipeP = open(argv[2], O_WRONLY);
			if (this->fd_pipeP < 0) {
				cout << "El open(fd_pipeP) lanza ERROR!" << endl;
				return;
			}
			
			this->fd_pipeH = open(argv[1], O_RDONLY);
			if (this->fd_pipeH < 0){
				close(this->fd_pipeP);
				cout << "El open(fd_pipeH) lanza ERROR!" << endl;
				perror("argv[]");
				return;	
			}	
			
			this->parametrosProceso.resize(contador-CORRIMIENTOARGUMENTO);
			
			for (int i = 0; i < (contador-CORRIMIENTOARGUMENTO); i++)
				this->parametrosProceso.at(i) = argv[i+CORRIMIENTOARGUMENTO];
		}
	}
	
	ComuDatos::~ComuDatos()
	{
		this->liberarRecursos();
	}

	///////////////////////////////////////////////////////////////////////
	//	Métodos públicos
	///////////////////////////////////////////////////////////////////////		
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
			
			wait(NULL); // Si existe un proceso hijo aun en ejecucion, se espera a su finalizacion.
			this->id_procesoHijo = fork();
		
			if (this->id_procesoHijo == 0)
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

	void ComuDatos::ejecutable(const string& nombreEjecutable)
	{
		this->nombreProceso = nombreEjecutable;
	}

	void ComuDatos::agregarParametro(const int valorParametro, const unsigned char posParametro)
	{	
		char num[MAX_NUM];
	
		sprintf(num, "%d", valorParametro);
	
	    if (posParametro >= this->parametrosProceso.size())
	    	this->parametrosProceso.resize(posParametro+1);
	    
	    this->parametrosProceso.at(posParametro) = num;
	    
	}
	
	void ComuDatos::agregarParametro(const unsigned int valorParametro, const unsigned char posParametro)
	{	
		char num[MAX_NUM];
	
		sprintf(num, "%d", valorParametro);
	
	    if (posParametro >= this->parametrosProceso.size())
	    	this->parametrosProceso.resize(posParametro+1);
	    
	    this->parametrosProceso.at(posParametro) = num;	    
	}
	
	void ComuDatos::agregarParametro(const string valorParametro, const unsigned char posParametro)
	{
		unsigned char cantParametros = this->parametrosProceso.size();
		
		if ( (posParametro + 1) >= cantParametros)
			this->parametrosProceso.resize(posParametro+1);
		
		this->parametrosProceso[posParametro] = valorParametro;
	}
	
	void ComuDatos::agregarParametro(const unsigned char valorParametro, const unsigned char posParametro)
	{
		unsigned char cantParametros = this->parametrosProceso.size();
		
		if ( (posParametro + 1) >= cantParametros)
			this->parametrosProceso.resize(posParametro+1);	
		
		this->parametrosProceso[posParametro] = valorParametro;
	}
	
	
	char ComuDatos::parametro(const unsigned char posParametro, string &parametro)
	{	
		char resultado = OK;
		
		if (posParametro < this->parametrosProceso.size())
			parametro = this->parametrosProceso[posParametro];
		else
			resultado = FIN_PARAMETROS;
		
		return resultado;
	}
	
	char ComuDatos::parametro(const unsigned char posParametro, int &parametro)
	{	
		char resultado = OK;
		string aux;
		
		if (posParametro < this->parametrosProceso.size()){
			aux = this->parametrosProceso[posParametro];
			parametro = atoi(aux.c_str());
		}else
			resultado = FIN_PARAMETROS;	
		
		return resultado;
	}
	
	char ComuDatos::parametro(const unsigned char posParametro, unsigned int &parametro)
	{	
		char resultado = OK;
		string aux;
		
		if (posParametro < this->parametrosProceso.size()){
			aux = this->parametrosProceso[posParametro];
			parametro = atoi(aux.c_str());
		}else
			resultado = FIN_PARAMETROS;	
		
		return resultado;
	}
	
	char ComuDatos::parametro(const unsigned char posParametro, unsigned char &parametro)
	{	
		char resultado = OK;
		
		if (posParametro < this->parametrosProceso.size())
			parametro = (this->parametrosProceso[posParametro])[0];
		else
			resultado = FIN_PARAMETROS;	
		
		return resultado;
	}
	
	char ComuDatos::escribir(const char* enviarDato, const unsigned int size)
	{
		char resultado = OK;
		
		if (size <= TOPE_ENVIAR_STRING)
			write(this->fd_pipeP, enviarDato, size*sizeof(char));
		else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::escribir(const int enviarDato)
	{
		char resultado = OK;
		
		if (sizeof(enviarDato) <= TOPE_ENVIAR_STRING)
			write(this->fd_pipeP, &enviarDato, sizeof(int));
		else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::escribir(const short enviarDato)
	{
		char resultado = OK;
		
		if (sizeof(short) <= TOPE_ENVIAR_STRING)
			write(this->fd_pipeP, &enviarDato, sizeof(short));
		else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::escribir(const unsigned short enviarDato)
	{
		char resultado = OK;
		
		if (sizeof(unsigned short) <= TOPE_ENVIAR_STRING)
			write(this->fd_pipeP, &enviarDato, sizeof(unsigned short));
		else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::escribir(const unsigned int enviarDato)
	{
		char resultado = OK;
		
		if (sizeof(unsigned int) <= TOPE_ENVIAR_STRING)
			write(this->fd_pipeP, &enviarDato, sizeof(unsigned int));
		else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::escribir(const char enviarDato)
	{
		char resultado = OK;
		
		if (sizeof(char) <= TOPE_ENVIAR_STRING)
			write(this->fd_pipeP, &enviarDato, sizeof(char));
		else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::escribir(const string& enviarDato)
	{
		char resultado = OK;
		unsigned int sizeCadena = enviarDato.size();
		write(this->fd_pipeP, &sizeCadena, sizeof(unsigned char));
		
		if (sizeCadena <= TOPE_ENVIAR_STRING)	
			write(this->fd_pipeP, enviarDato.c_str(), sizeCadena);
		else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::leer(const unsigned int size, char* s)
	{
		char resultado = OK;
	
		if (size <= TOPE_ENVIAR_STRING){		
			read(this->fd_pipeH, s, size);
		//	*(s + size) = 0;		
		}else resultado = EXCEDE_TOPE;
		
		return resultado;
	}
	
	char ComuDatos::leer(int* i)
	{
		int leo = 0;
		read(this->fd_pipeH, &leo, sizeof(int));
		*i = leo;
		
		return OK;
	}
	
	char ComuDatos::leer(unsigned int* ui)
	{
		unsigned int leo = 0;
		read(this->fd_pipeH, &leo, sizeof(unsigned int));
		*ui = leo;
		
		return OK;
	}
	
	char ComuDatos::leer(short* s)
	{		
		short leo = 0;
		read(this->fd_pipeH, &leo, sizeof(short));
		*s = leo;
		
		return OK;	
	}
	
	char ComuDatos::leer(unsigned short* us)
	{
		unsigned short leo = 0;
		read(this->fd_pipeH, &leo, sizeof(unsigned short));
		*us = leo;
		
		return OK;
	}
	
	char ComuDatos::leer(char* c)
	{
		char leo = 0;
		read(this->fd_pipeH, &leo, sizeof(char));
		*c = leo;
		
		return OK;
	}
	
	char ComuDatos::leer(string &cadena)
	{
		char resultado = OK;
		unsigned int sizeCadena = 0;
		char* leo = NULL;
		
		read(this->fd_pipeH, &sizeCadena, sizeof(unsigned char));
		
		if (sizeCadena <= TOPE_ENVIAR_STRING){		
			leo = new char[sizeCadena + 1];
			read(this->fd_pipeH, leo, sizeCadena);
			
			leo[sizeCadena] = 0;
			cadena = leo;
			
			delete leo;
		}else resultado = EXCEDE_TOPE;
		
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
	
	void ComuDatos::liberarRecursos()
	{
		if (this->nombreProceso.length() > 0)
		{
			// Se cierran los archivos
			close(this->fd_pipeH);
			close(this->fd_pipeP);
			
			// Se desvinculan los archivos
			string nombre = this->nombreProceso + "_ComuDatosH";		
			unlink(nombre.c_str());
			
			nombre = this->nombreProceso + "_ComuDatosP";
			unlink(nombre.c_str());
		}
	}
	
	///////////////////////////////////////////////////////////////////////
	//	Método privado
	///////////////////////////////////////////////////////////////////////
	unsigned char ComuDatos::contarArgv(char **argv)
	{
		unsigned char contador;
		for(contador = 0; argv[contador] != NULL; contador++);
		return contador;
	}
