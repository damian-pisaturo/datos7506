#ifndef COMUDATOS_H_
#define COMUDATOS_H_

/* 
 *	Autor: Aleee
 *  Fecha: 15 de Agosto de 2007
 *	
 *	Librería para comunicaciones de procesos para Organización de Datos, cátedra Servetto.
 */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <vector>
 
using namespace std;

 class ComuDatos {
	 private:
		int id_procesoHijo;
		int fd_pipeP; 
	 	int fd_pipeH; 
		string nombreProceso;
	 	vector<string> parametrosProceso;
	 	int cantParametros;
		
	 public:
		 
		 /*Constantes de error y definiciones
		  */
		static const unsigned int TOPE_ENVIAR_STRING = 4096;
		static const unsigned char FIN_PARAMETROS = 1;
		static const unsigned char EXCEDE_TOPE  = 3;
		static const unsigned char OK = 2;
		static const char ERROR_EJECUCION  = -1;
		 
		//! Crea una clase de comunicación vacía.
		ComuDatos();
	 	//! Crea una clase de comunicación y asigna el nombre del ejecutable a lanzar.
		ComuDatos(string nombreEjecutable);
	 	//! Toma los parámetros de ejecución del proceso e interpreta los parámetros pasados.
	 	ComuDatos(char** argv);
		
	 	//! Lanza el proceso registrado e instancia los recursos de comunicación.
		int lanzar();
	 
	 	//! Asigna el nombre del proceso a ejecutar.
	 	void ejecutable(string nombreEjecutable);
	 
	 	//! Agrega un parámetro de valor en la posición indicada. 
	 	//! Si no se han cargado las posiciones anteriores se las asinga con valor vacío.
	 	void agregarParametro(int valorParametro, unsigned int posParametro);
	 	void agregarParametro(string valorParametro, unsigned int posParametro);
	 	void agregarParametro(unsigned char valorParametro, unsigned int posParametro);
	 	void agregarParametro(unsigned int valorParametro, unsigned int posParametro);
	 	
	 	//! Obtiene el valor de la posición indicada 
		char parametro(unsigned int posParametro, string &parametro);
		char parametro(unsigned int posParametro, int &parametro);
		char parametro(unsigned int posParametro, unsigned char &parametro);
					 
	 	//! Retorna el descriptor de archivo (tipo open, read, write) del canal de comunicación
		//! para lectura.
		int fd_leer();	 	//! Lee un short del canal de comunicación de datos correspondiente.

		//! Retorna el descriptor de archivo (tipo open, read, write) del canal de comunicación
		//! para escritura.
		int fd_escribir();
	 
	 	//! Escribe un string en el canal de comunicación de datos correspondiente.
	 	char escribir(const char* enviarDato, unsigned int size);
	 	//! Escribe un int en el canal de comunicación de datos correspondiente.
	 	char escribir(int enviarDato);
	 	//! Escribe un unsigned int en el canal de comunicación de datos correspondiente.
	 	char escribir(unsigned int enviarDato);
	 	//! Escribe un short en el canal de comunicación de datos correspondiente.
	 	char escribir(short enviarDato);
		//! Escribe un short en el canal de comunicación de datos correspondiente.
		char escribir(unsigned short enviarDato);
	 	//! Escribe un char en el canal de comunicación de datos correspondiente.
	 	char escribir(char enviarDato);
	 	//! Escribe un string en el canal de comunicación de datos correspondiente.
	 	char escribir(const string &enviarDato);
	 			
	 	//! Lee un string de longitud máxima "cantidad" del canal de comunicación de datos correspondiente.
	 	char leer(unsigned int cantidad, char* s);
	 	//! Lee un int del canal de comunicación de datos correspondiente.
	 	char leer(int* i);
	 	//! Lee un unsigned short del canal de comunicación de datos correspondiente.
	 	char leer(unsigned int* ui);
	 	//! Lee un short del canal de comunicación de datos correspondiente.
	 	char leer(short* s);
	 	//! Lee un unsigned short del canal de comunicación de datos correspondiente.
	 	char leer(unsigned short* us);
	 	//! Lee un char del canal de comunicación de datos correspondiente.
	 	char leer(char* c);
	 	//! Lee un string en el canal de comunicación de datos correspondiente.
	 	char leer(unsigned int cantidad, string &cadena);
	 		 			 
		//! Libera los recursos reservados para las comunicaciones.
	 	void liberarRecursos();
	 
	 	//! Libera los recursos tomados por la instancia (los de comunicaciones incluidos).
	 	~ComuDatos();
 };

#endif

