///////////////////////////////////////////////////////////////////////////
//	Archivo   : ComuDatos.h
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

#ifndef COMUDATOS_H_
#define COMUDATOS_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <iostream>
#include <vector>
using namespace std;

typedef vector<string> VectorParametros;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ComuDatos
//			(Permite la comunicación entre procesos de capas).
///////////////////////////////////////////////////////////////////////////
class ComuDatos
{	
	private:
	///////////////////////////////////////////////////////////////////////
	//	Atributos
	///////////////////////////////////////////////////////////////////////
		int id_procesoHijo;
		int fd_pipeP; 
	 	int fd_pipeH; 
		string nombreProceso;
		VectorParametros parametrosProceso;
	 	unsigned char cantParametros;
	 	
	 	static const unsigned char CORRIMIENTOARGUMENTO = 3;
	 	
	public:
	///////////////////////////////////////////////////////////////////////
	//	Constantes de error y definiciones
	///////////////////////////////////////////////////////////////////////
		static const unsigned int TOPE_ENVIAR_STRING = 4096;
		static const unsigned char MAX_NUM 			 = 20;
		static const unsigned char FIN_PARAMETROS    = 1;
		static const unsigned char EXCEDE_TOPE       = 3;
		static const unsigned char OK                = 2;
		static const char ERROR_EJECUCION            = -1;

	///////////////////////////////////////////////////////////////////////
	//	Constructores/Destructor
	///////////////////////////////////////////////////////////////////////
		//! Crea una clase de comunicación vacía.
		ComuDatos();		
	 	//! Crea una clase de comunicación y asigna el nombre del ejecutable a lanzar.
		ComuDatos(const string& nombreEjecutable);
	 	//! Toma los parámetros de ejecución del proceso e interpreta los parámetros pasados.
	 	ComuDatos(char** argv);
	 	
	 	//! Libera los recursos tomados por la instancia (los de comunicaciones incluidos).
		 virtual ~ComuDatos();
	
	///////////////////////////////////////////////////////////////////////
	//	Métodos públicos
	///////////////////////////////////////////////////////////////////////		
	 	//! Lanza el proceso registrado e instancia los recursos de comunicación.
		int lanzar();
	 
	 	//! Asigna el nombre del proceso a ejecutar.
	 	void ejecutable(const string& nombreEjecutable);
	 
	 	//! Agrega un parámetro de valor en la posición indicada. 
	 	//! Si no se han cargado las posiciones anteriores se las asinga con valor vacío.
	 	void agregarParametro(const int valorParametro, const unsigned char posParametro);
	 	void agregarParametro(const string valorParametro, const unsigned char posParametro);
	 	void agregarParametro(const unsigned char valorParametro, const unsigned char posParametro);
	 	void agregarParametro(const unsigned int valorParametro, const unsigned char posParametro);
	 	
	 	//! Obtiene el valor de la posición indicada 
		char parametro(const unsigned char posParametro, string &parametro);
		char parametro(const unsigned char posParametro, int &parametro);
		char parametro(const unsigned char posParametro, unsigned int &parametro);
		char parametro(const unsigned char posParametro, unsigned char &parametro);
					 
	 	//! Retorna el descriptor de archivo (tipo open, read, write) del canal de comunicación
		//! para lectura.
		int fd_leer();	 	//! Lee un short del canal de comunicación de datos correspondiente.

		//! Retorna el descriptor de archivo (tipo open, read, write) del canal de comunicación
		//! para escritura.
		int fd_escribir();
	 
	 	//! Escribe un string en el canal de comunicación de datos correspondiente.
	 	char escribir(const char* enviarDato, const unsigned int size);
	 	//! Escribe un int en el canal de comunicación de datos correspondiente.
	 	char escribir(const int enviarDato);
	 	//! Escribe un unsigned int en el canal de comunicación de datos correspondiente.
	 	char escribir(const unsigned int enviarDato);
	 	//! Escribe un short en el canal de comunicación de datos correspondiente.
	 	char escribir(const short enviarDato);
		//! Escribe un short en el canal de comunicación de datos correspondiente.
		char escribir(const unsigned short enviarDato);
	 	//! Escribe un char en el canal de comunicación de datos correspondiente.
	 	char escribir(const char enviarDato);
	 	//! Escribe un string en el canal de comunicación de datos correspondiente.
	 	char escribir(const string& enviarDato);
	 			
	 	//! Lee un string de longitud máxima "cantidad" del canal de comunicación de datos correspondiente.
	 	char leer(const unsigned int size, char* s);
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
	 	char leer(string &cadena);
	 		 			 
		//! Libera los recursos reservados para las comunicaciones.
	 	void liberarRecursos();	
	 	
	private:
	///////////////////////////////////////////////////////////////////////
	//	Método privado
	///////////////////////////////////////////////////////////////////////		
		unsigned char contarArgv(char** argv);
		
 };/*Fin clase ComuDatos*/

#endif /*COMUDATOS_H_*/

