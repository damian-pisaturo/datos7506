///////////////////////////////////////////////////////////////////////////
//	Archivo   : Archivo.cpp
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementeacion de las clase Archivo.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "Archivo.h"

///////////////////////////////////////////////////////////////////////
// Constructores/Destructor
///////////////////////////////////////////////////////////////////////
	Archivo::Archivo() : fstream()
	{ 
		this->archivoValido = true;
	}
	
	Archivo::Archivo(const string& nomArchivo): fstream()
	{
		int id = this->generarID(nomArchivo);
		
		//Abre el archivo en modo lectura - escritura binario  
		this->open(nomArchivo.c_str(), ios::in |ios::out |ios::binary);
		
		//Determina si tuvo exito la apertura del archivo
		if (!this->is_open()){	
			
		    //Limpia los flags de control de estado del archivo.
		    this->clear();
			  
		    //Si el archivo no existe, se crea    
		    this->open(nomArchivo.c_str(), ios::out | ios::binary);      
		    this->close();
			    
		    //Reabre el archivo para lectura - escritura binario
		    this->open(nomArchivo.c_str(), ios::in|ios::out|ios::binary);
		    
		    //Escribe el numero magico identificador.
		    this->write((const char*)&id, Tamanios::TAMANIO_IDENTIFICADOR);		    
		    this->archivoValido = true;
		    
		}else{			
			int idPresente = 0;
			this->read((char*)&idPresente, Tamanios::TAMANIO_IDENTIFICADOR);				
			this->archivoValido = (id == idPresente);
		}
	}
	
	Archivo::Archivo(const string& nomArchivo, int magicNumber): fstream()
	{		
		//Abre el archivo en modo lectura - escritura binario  
		this->open(nomArchivo.c_str(), ios::in |ios::out |ios::binary);
		
		//Determina si tuvo exito la apertura del archivo
		if (!this->is_open()){	
			
		    //Limpia los flags de control de estado del archivo.
		    this->clear();
			  
		    //Si el archivo no existe, se crea    
		    this->open(nomArchivo.c_str(), ios::out | ios::binary);      
		    this->close();
			    
		    //Reabre el archivo para lectura - escritura binario
		    this->open(nomArchivo.c_str(), ios::in|ios::out|ios::binary);
		    
		    //Escribe el numero magico identificador.
		    this->write((const char*)&magicNumber, Tamanios::TAMANIO_IDENTIFICADOR);		    
		    this->archivoValido = true;
		    
		}else{			
			int id = 0;
			this->read((char*)&id, Tamanios::TAMANIO_IDENTIFICADOR);				
			this->archivoValido = (id == magicNumber);
		}
	}

	Archivo::~Archivo()
	{ 
		if (this->is_open())
			this->close();
	}

///////////////////////////////////////////////////////////////////////
// Metodos publicos
///////////////////////////////////////////////////////////////////////
	
	/* Operador <<
	 */
	Archivo& Archivo::operator<< (const int& val)
	{
		this->write((const char*)&val, sizeof(int));

		return *this;
	}
	
	Archivo& Archivo::operator<< (const unsigned int& val)
	{
		this->write((const char*)&val, sizeof(unsigned int));
		
		return *this;
	}
	
	Archivo& Archivo::operator<< (const short& val)
	{
		this->write((const char*)&val, sizeof(short));
		
		return *this;
	}
	
	Archivo& Archivo::operator<< (const unsigned short& val)
	{
		this->write((const char*)&val, sizeof(unsigned short));
		
		return *this;
	}
	
	Archivo& Archivo::operator<< (const char& val)
	{
		this->write((const char*)&val, sizeof(char));
		
		return *this;
	}
	
	Archivo& Archivo::operator<< (const unsigned char& val)
	{
		this->write((const char*)&val, sizeof(unsigned char));
		
		return *this;
	}

	
	Archivo& Archivo::operator<< (const float& val)
	{
		this->write((const char*)&val, sizeof(float));
		
		return *this;
	}
	
	Archivo& Archivo::operator<< (const bool& val)
	{
		this->write((const char*)&val, sizeof(bool));
		
		return *this;
	}
	
	Archivo& Archivo::operator<< (const string& val)
	{
		const char stringSize = val.size();
		this->write((const char*)&stringSize, sizeof(char));
		this->write(val.c_str(), stringSize*sizeof(char));
		
		return *this;		
	}
	
	
	/* Operador >>
	 */
	Archivo& Archivo::operator>> (int& val)
	{
		this->read((char*)&val, sizeof(int));
	
		return *this;
	}
	
	Archivo& Archivo::operator>> (unsigned int& val)
	{
		this->read((char*)&val, sizeof(unsigned int));
		
		return *this;
	}
	
	Archivo& Archivo::operator>> (short& val)
	{
		this->read((char*)&val, sizeof(short));
		
		return *this;
	}
	
	Archivo& Archivo::operator>> (unsigned short& val)
	{
		this->read((char*)&val, sizeof(unsigned short));
		
		return *this;
	}
	
	Archivo& Archivo::operator>> (char& val)
	{
		this->read(&val, sizeof(char));
		
		return *this;
	}
	
	Archivo& Archivo::operator>> (unsigned char& val)
	{
		this->read((char*)&val, sizeof(unsigned char));
		
		return *this;
	}
	
	Archivo& Archivo::operator>> (float& val)
	{
		this->read((char*)&val, sizeof(float));
		
		return *this;
	}
	
	
	Archivo& Archivo::operator>> (bool& val)
	{
		this->read((char*)&val, sizeof(bool));
		
		return *this;
	}
	
	Archivo& Archivo::operator>> (string& val)
	{
		char stringSize = 0;
		this->read(&stringSize, sizeof(char));
		
		char* s = new char[stringSize*sizeof(char)];
		this->read(s, stringSize);
		
		val.assign(s, stringSize);
		delete[] s;
		
		return *this;
	}
	
	void Archivo::end()
	{
		this->seekg(0, ios_base::end);
	}
	
	void Archivo::rewind()
	{
		this->seekg(sizeof(int), ios_base::beg);
	}
	
	size_t Archivo::size()
	{
		this->end();	
		return this->tellg();
	}

///////////////////////////////////////////////////////////////////////
// Metodo privado
///////////////////////////////////////////////////////////////////////

	int Archivo::generarID(const string& nomArchivo)
	{
		int id = 0;
		string idString, nombre, extension;
		size_t posPunto = nomArchivo.rfind('.');			
	
		idString  = "_7506";
	
		nombre    = nomArchivo.substr(0, posPunto);
		extension = nomArchivo.substr(posPunto);
		
		if (nombre.length() > 4)
			idString += nombre.substr(0,4);
		else
			idString += nombre;
		
		idString += extension;			
		idString += '_';			

		for (string::const_iterator it = idString.begin(); 
			it != idString.end(); ++it){
			id = *it + (id << 6) + (id << 16) - id;
		}

		return id;
	}
