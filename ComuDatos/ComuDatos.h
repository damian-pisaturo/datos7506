/* 
 *	Autor: Aleee
 *  Fecha: 15 de Agosto de 2007
 *	
 *	Librería para comunicaciones de procesos para Organización de Datos, cátedra Servetto.
 */
 #include <string>
 #include <vector>
 
 #define TOPE_ENVIAR_STRING 512
 #define FIN_PARAMETROS "\1"
 
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
		//! Crea una clase de comunicación vacía.
		ComuDatos();
	 	//! Crea una clase de comunicación y asigna el nombre del ejecutable a lanzar.
		ComuDatos(string nombreEjecutable);
	 	//! Toma los parámetros de ejecución del proceso e interpreta los parámetros pasados.
	 	ComuDatos(char** argv);
		
	 	//! Lanza el proceso registrado e instancia los recursos de comunicación.
		void lanzar();
	 
	 	//! Asigna el nombre del proceso a ejecutar.
	 	void ejecutable(string nombreEjecutable);
	 
	 	//! Agrega un parámetro de valor en la posición indicada. 
	 	//! Si no se han cargado las posiciones anteriores se las asinga con valor vacío.
	 		void agregarParametro(unsigned char valorParametro, unsigned int posParametro);
	 	void agregarParametro(string valorParametro, unsigned int posParametro);
	 
	 	//! Obtiene el valor de la posición indicada 
		string parametro(unsigned int posParametro);
		unsigned char parametro(unsigned int posParametro);
			 
	 	//! Retorna el descriptor de archivo (tipo open, read, write) del canal de comunicación
		//! para lectura.
		int fd_leer();
		//! Retorna el descriptor de archivo (tipo open, read, write) del canal de comunicación
		//! para escritura.
		int fd_escribir();
	 
	 	//! Escribe un string en el canal de comunicación de datos correspondiente.
	 	void escribir(string enviarDato);
	 	//! Escribe un INT en el canal de comunicación de datos correspondiente.
	 	void escribir(int enviarDato);
	 			
	 	//! Lee un string de longitud máxima "cantidad" del canal de comunicación de datos correspondiente.
	 	string leerString(int cantidad);
	 	//! Lee un int del canal de comunicación de datos correspondiente.
	 	int leerInt();
	 		 			 
		//! Libera los recursos reservados para las comunicaciones.
	 	void liberarRecursos();
	 
	 	//! Libera los recursos tomados por la instancia (los de comunicaciones incluidos).
	 	~ComuDatos();
 };
