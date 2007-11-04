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
		{
			//Si el archivo esta vacio, agregar el primer registro
			//booleano.
			//cout << "Size:" << this->size() << endl;
			if (this->size() == 0){	
				//cout << "Inserto el primer bool" << endl;
				bool valor = true;
				this->agregarRegistro(&valor);
			}			
		}
		
		ArchivoEL::~ArchivoEL(){ }
		
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
			char resultado = ResFisica::OK;
			
			resultado = this->posicionarse(numRegistro);
			if (resultado == ResFisica::OK)
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
		{ }
		
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
				numBloque = ResFisica::BLOQUES_OCUPADOS;
			
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
		
		ArchivoELVariable::~ArchivoELVariable() { }
	
	///////////////////////////////////////////////////////////////////////
	//	Metodos publicos
	///////////////////////////////////////////////////////////////////////
		short ArchivoELVariable::buscarEspacioLibre(unsigned short espacioRequerido)
		{
			unsigned short espacioLibre = 0;
			short numBloque = -1;
					
			while((espacioLibre <= espacioRequerido) && 
					(!this->fin())){
				this->posicionarse(++numBloque);
				this->leer(&espacioLibre);
			}
			
			if (this->fin())
				numBloque = ResFisica::BLOQUES_OCUPADOS;
			
			return numBloque;		
		}
