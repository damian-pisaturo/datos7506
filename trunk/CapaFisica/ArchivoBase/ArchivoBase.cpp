////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de la clase ArchivoBase. 
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "ArchivoBase.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoBase
//			(Abstracta. Implementa primitivas para el manejo de archivos
//           organizados en bloques fijos en disco).
///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////

		ArchivoBase::ArchivoBase(string &nombre, unsigned short tamBloque)
		{
			int id = this->generarID(nombre);
			
			//Abre el archivo en modo lectura - escritura binario  
			this->archivo.open(nombre.c_str(), ios::in |ios::out |ios::binary);
			
			//Determina si tuvo éxito la apertura del archivo
			if (!this->archivo.is_open()){	
				
			    //Limpia los flags de control de estado del archivo.
			    this->archivo.clear();
				  
			    //Si el archivo no existe, se crea    
			    this->archivo.open(nombre.c_str(), ios::out | ios::binary);      
			    this->archivo.close();
				    
			    //Reabre el archivo para lectura - escritura binario
			    this->archivo.open(nombre.c_str(),ios::in|ios::out|ios::binary);
			    
			    //Escribe el numero magico identificador.
			    cout << "Escribiendo ID " << id << " (" << nombre << ")" << endl;
			    this->archivo.write((char*)&id, Tamanios::TAMANIO_IDENTIFICADOR);
			    
			    this->archivoValido  = true;
			}else{
				int idPresente = 0;
				
				this->archivo.read((char*)&idPresente, Tamanios::TAMANIO_IDENTIFICADOR);				
				this->archivoValido = (id == idPresente);
			}
			
			this->tamBloque = tamBloque;
		}
	
		ArchivoBase::~ArchivoBase()
		{
			//Cierra el archivo
			this->tamBloque = 0;
			this->archivo.close();
		}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
		char ArchivoBase::escribir(const void* bloque) 
		{
			char resultado = ResultadosFisica::OK;

			//Verifica que el archivo esté abierto
			if (this->archivo.is_open()) {	
				//Escribe el bloque en el archivo
			    this->archivo.write(static_cast<const char*>(bloque), this->tamBloque);			  
			    
			    //Chequea si se ha producido un error 
			    if (this->archivo.fail()){
			    	this->archivo.clear();
			      resultado = ResultadosFisica::ERROR_ESCRITURA;
			    }else
			    	 this->archivo.flush();
		    }else
			  //El archivo no se encuentra abierto
			  resultado = ResultadosFisica::NO_ABIERTO;
	
			return resultado;
		}	

		char ArchivoBase::leer(void* bloque) 
		{
			char resultado = ResultadosFisica::OK;
			
			//Verifica que el archivo esté abierto
			if (this->archivo.is_open()) {		  	
				//Lee del archivo un bloque 
				this->archivo.read(static_cast<char*>(bloque), this->tamBloque);
				
				//Chequea si se ha producido un error
				if (this->archivo.fail()){
					this->archivo.clear();
					resultado = ResultadosFisica::ERROR_LECTURA;
				}
			}else
			  //El archivo no se encuentra abierto
			  resultado = ResultadosFisica::NO_ABIERTO;
			
			return resultado;
		}

		bool ArchivoBase::fin()
		{	
		  // Para comprobar el fin lee un char del buffer, sin retirarlo y lo
		  // compara con el fin de archivo
		  bool esEof = (this->archivo.peek() == char_traits<char>::eof());
		
		  if (esEof)
		    //Si llegó al fin del archivo limpia los flags
		    this->archivo.clear();
		
		  return esEof;
		 
		}
	
	
		short ArchivoBase::posicion()
		{
			short pos = 0;
			
			//Verifica que el archivo esté abierto 
			if (this->archivo.is_open()){
				// Calcula el número de bloque según la posición del byte actual
			    pos = this->archivo.tellg();
				pos += Tamanios::TAMANIO_IDENTIFICADOR;
				pos /= this->tamBloque;
			}else
				pos = ResultadosFisica::NO_ABIERTO;
			
			  return pos;
		}
		
		char ArchivoBase::posicionarse(unsigned short posicion)
		{
			char resultado = ResultadosFisica::OK;
		
			//Verifica que el archivo esté abierto
			if (this->archivo.is_open()) {
				//Mueve la posición actual según sea el tamano del bloque
				//considerando la corrección por el identificador mágico.
				this->archivo.seekg( (posicion*this->tamBloque) + Tamanios::TAMANIO_IDENTIFICADOR,
						ios_base::beg);

				//Chequea si se ha producido un error			
				if (this->archivo.fail())
					resultado = ResultadosFisica::ERROR_POSICION;
			}else
				resultado = ResultadosFisica::NO_ABIERTO;

			
			return resultado;
		}
	
		void ArchivoBase::posicionarseFin()
		{
			this->archivo.seekg(0, ios_base::end);
		}
		
		long ArchivoBase::size()
		{			
			this->posicionarseFin();
			return this->archivo.tellg();	
		}

	///////////////////////////////////////////////////////////////////////
	// Metodo privado
	///////////////////////////////////////////////////////////////////////
		int ArchivoBase::generarID(string &nomArchivo)
		{			
			int id = 1;
			string idString, nombre, extension;
			char posPunto = nomArchivo.find_last_of('.');
			
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
				id *= (*it);
			}
			
			return id / 97;
		}

	///////////////////////////////////////////////////////////////////////
	// Getters
	///////////////////////////////////////////////////////////////////////

		unsigned short ArchivoBase::getTamanioBloque()
		{
			return this->tamBloque;
		}
				
		bool ArchivoBase::esValido()
		{
			return this->archivoValido;
		}

