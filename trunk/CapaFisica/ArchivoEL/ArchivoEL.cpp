////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion clases ArchivoEL, ArchivoELFijo y
//		ArchivoELVariable. 
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
//////////////////////////////////////////////////////////////////////////

#include "ArchivoEL.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoEL 
//		   (Clase que define el comportamiento del manejo de
//			los archivos de control de espacio libre).
///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////	
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoEL::ArchivoEL(string nombre, unsigned short tamBloque) :
			ArchivoBase(nombre, tamBloque)
		{ }
		
		ArchivoEL::~ArchivoEL()
		{ }
		
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		char ArchivoEL::agregarRegistro(const void* registro)
		{					
			this->posicionarseFin();
			
			return (this->escribir(registro));	
		}
		
		char ArchivoEL::modificarRegistro(const void* registro, unsigned short numRegistro)
		{
			char resultado = ResultadosFisica::OK;
			
			resultado = this->posicionarse(numRegistro);
			
			if (resultado == ResultadosFisica::OK)
				resultado = this->escribir(registro);
			
			return resultado;
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoELFijo
//		   (Clase que permite manejar archivos de control de espacio
//			libre cuya definicion fisica consiste en un atributo booleano
//			indicando si el bloque se encuentra o no libre para su uso).
///////////////////////////////////////////////////////////////////////////
		
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoELFijo::ArchivoELFijo(string nombre) :
			ArchivoEL(nombre, sizeof(bool))
		{
			//Si el archivo esta vacio, agregar los dos primeros
			//registros booleanos.
			if (this->size() == Tamanios::TAMANIO_IDENTIFICADOR){	
				bool valor = true;
				this->agregarRegistro(&valor);
				this->agregarRegistro(&valor);					
			}
	}
		
		ArchivoELFijo::~ArchivoELFijo() { }
	
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		short ArchivoELFijo::buscarBloqueLibre()
		{
			bool libre = false;
			short numBloque = 0;

			this->posicionarse(0);
			this->leer(&libre);
			
			while((!libre) && (!this->fin())){
				this->posicionarse(++numBloque);
				this->leer(&libre);
			}
			
			if (!libre)
				numBloque = ResultadosFisica::BLOQUES_OCUPADOS;

			return numBloque;		
		}
		
		short ArchivoELFijo::buscarBloqueDobleLibre()
		{
			char cantLibres = 0;
			short numBloque = -1;
			bool libre      = false;			

			//Posicionarse al comienzo del archivo de espacio libre.
			this->posicionarse(0);
			
			//Recorrer el archivo hasta encontrar los dos primeros valores
			//booleanos en true, o hasta llegar al final del mismo.
			while( (cantLibres < 2) && (!this->fin()) ){				
				numBloque++;
				cantLibres = 0;
				this->leer(&libre);
				
				if (libre){ 
					cantLibres++;
					this->leer(&libre);
					if (libre) cantLibres++;
				}				
			}
			
			//Si no existen dos bloques consecutivos libres, se devuelve
			//BLOQUES_OCUPADOS.
			if (cantLibres < 2)
				numBloque = ResultadosFisica::BLOQUES_OCUPADOS;

			return numBloque;		
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoELVariable
//		   (Clase que permite manejar archivos de control de espacio
//			libre cuya definicion fisica consiste en dos atributos
//			unsigned short: uno para el numero de bloque y otro indicando
//			su espacio libre).
///////////////////////////////////////////////////////////////////////////
		
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		ArchivoELVariable::ArchivoELVariable(string nombre) :
			ArchivoEL(nombre, sizeof(unsigned short))
		{ }
		
		ArchivoELVariable::~ArchivoELVariable() 
		{ }
	
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		short ArchivoELVariable::buscarEspacioLibre(unsigned short espacioRequerido, unsigned short numBloque)
		{
			unsigned short espacioLibre = 0;
			short bloqueActual = -1;
			
			this->posicionarse(numBloque);
			
			while((espacioLibre <= espacioRequerido) && 
					(!this->fin())){
				++bloqueActual;
				this->leer(&espacioLibre);
			}
			
			if (espacioLibre < espacioRequerido)
				bloqueActual = ResultadosFisica::BLOQUES_OCUPADOS;
			
			return bloqueActual;		
		}
