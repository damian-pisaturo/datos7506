#include "FileManager.h"

FileManager::FileManager(const string &nombreArchivo, unsigned short tamBloque,
						 unsigned char tipoOrg, ListaInfoRegistro* listaTiposAtributos) {
		
	this->tamBloque = tamBloque;
	this->tipoOrg = tipoOrg;
	this->siguienteBloque = 0;
	this->listaInfoReg = listaTiposAtributos;
	this->pedirBloque = true;
	
	switch (tipoOrg) {
	
		case TipoOrganizacion::REG_FIJOS:
			this->archivo = new ArchivoDatosRegistros(nombreArchivo, tamBloque);
			this->bloque = NULL;
			break;
		case TipoOrganizacion::REG_VARIABLES:
			this->archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
			this->bloque = new Bloque(0, tamBloque, tipoOrg);
			break;
		default:
			this->archivo = NULL;
			this->bloque = NULL;
			
	}
	
}

FileManager::~FileManager() {
	
	if (this->archivo)
		delete this->archivo;
	
	if (this->bloque)
		delete this->bloque;
	
}


/* Devuelve en 'registro' el registro cuya posicion en el archivo es 
 * numReg.
 * En tamReg devuelve el tamaño del mismo.
 */
int FileManager::leerRegistro(char* &registro, unsigned short numReg,
							  unsigned short &tamReg, bool &quedanRegistros) {
	
	int resultado = ResultadosMetadata::OK;
	char* contenidoBloque = new char[this->getTamanioBloqueDatos()];
	tamReg = 0;
	
	if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES) {
		
		// Se obtiene el siguiente bloque hasta encontrar el registro
		int numBloque = 0;
		unsigned short espLibre = this->getTamanioBloqueDatos() - Tamanios::TAMANIO_ESPACIO_LIBRE - Tamanios::TAMANIO_CANTIDAD_REGISTROS;
		bool encontrado = false;
		Bloque bloque(0, this->getTamanioBloqueDatos(), this->getTipoOrganizacion());
		unsigned short cantRegistrosLeidos = 0, cantRegistrosEnBloque = 0;

		while ( (!encontrado) && (resultado == ResultadosMetadata::OK) ) {
			
			resultado = ((ArchivoDatosBloques*)this->archivo)->siguienteBloque(contenidoBloque, numBloque, espLibre);
			
			if (resultado == ResultadosFisica::OK) {
				
				bloque.setDatos(contenidoBloque);
				cantRegistrosEnBloque = bloque.getCantidadRegistros();
				
				if ((cantRegistrosLeidos + cantRegistrosEnBloque) > numReg) {
					// Obtengo el registro
					unsigned short nroRegistro = numReg - cantRegistrosLeidos;
					registro = bloque.getRegistroPorNro(nroRegistro, tamReg);
					encontrado = true;
				} else {
					cantRegistrosLeidos += cantRegistrosEnBloque;
					++numBloque;
				}
				
				if (this->archivo->fin())
					quedanRegistros = false;
				else quedanRegistros = true;
				
				resultado = ResultadosMetadata::OK;
				
			} else if (resultado == ResultadosFisica::FIN_BLOQUES)
				resultado = ResultadosMetadata::FIN_REGISTROS;
				
		}
		
	} else if (this->getTipoOrganizacion() == TipoOrganizacion::REG_FIJOS) {
		// Lee el registro de datos
		resultado = ((ArchivoDatosRegistros*)this->archivo)->leerRegistro(contenidoBloque, numReg);
		
		if (resultado == ResultadosFisica::OK) {
			
			registro = contenidoBloque;
			tamReg = this->getTamanioBloqueDatos();
			resultado = ResultadosMetadata::OK;
			
		} else if (resultado == ResultadosFisica::ERROR_POSICION)
			resultado = ResultadosMetadata::FIN_REGISTROS;
	}
	
	return resultado;
	
}


/* Escribe en el archivo de datos el registro pasado por parametro
 * en la posicion del primer registro libre que encuentre, utilizando
 * el archivo de control de espacio libre. Si todos los registro
 * se encuentran ocupados, appendea al final del archivo.
 * Devuelve la posicion en donde fue escrito finalmente.
 */
int FileManager::escribirRegistro(char* registro, unsigned short tamRegistro) {
	
	int resultado = ResultadosMetadata::OK;
	int nroBloque = -1;
	
	if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES) {
				
		char* contenidoBloque = new char[this->getTamanioBloqueDatos()];
		memset(contenidoBloque, 0, this->getTamanioBloqueDatos());
		
		// Lee de disco el contenido del bloque donde debe insertar.
		nroBloque = ((ArchivoDatosBloques*)this->archivo)->buscarBloque(contenidoBloque, tamRegistro);
		
		// Si hay lugar en el bloque de disco, le asigno su contenido	
		if ( (nroBloque != ResultadosFisica::BLOQUES_OCUPADOS) && (nroBloque != ResultadosFisica::ARCHIVO_VACIO) ){
			this->bloque->setNroBloque(nroBloque);		
			this->bloque->setDatos(contenidoBloque);
			// Inserta el registro.
			this->bloque->altaRegistro(this->listaInfoReg, (char*)registro);
			// Sobreescribe el archivo de datos en la posicion 'nroBloque'
			resultado = ((ArchivoDatosBloques*)this->archivo)->escribirBloque(this->bloque->getDatos(), nroBloque,
																			  this->bloque->getTamanioEspacioLibre());
		}else{
			
			// Inserta el registro.
			this->bloque->altaRegistro(this->listaInfoReg, (char*)registro);
			
			// Agrega un nuevo bloque de datos al archivo
			nroBloque = ((ArchivoDatosBloques*)this->archivo)->escribirBloque(this->bloque->getDatos(),
																			  this->bloque->getTamanioEspacioLibre());
			
			delete[] contenidoBloque;
		}
		
	} else if (this->getTipoOrganizacion() == TipoOrganizacion::REG_FIJOS)
		// Agrega un nuevo registro de datos al archivo
		nroBloque = ((ArchivoDatosRegistros*)this->archivo)->escribirRegistro(registro);
	
	if (nroBloque >= 0)
		resultado = ResultadosMetadata::OK;
	else resultado = nroBloque;
	
	return resultado;
	 
}


/*
 * Método que posiciona el puntero interno del archivo el comienzo
 * del mismo.
 */
void FileManager::comienzo() {
	this->archivo->posicionarse(0);
}


int FileManager::getNextBloque(char* &registro, unsigned short &tamRegistro) {
	
	unsigned short espLibre = this->getTamanioBloqueDatos() - Tamanios::TAMANIO_ESPACIO_LIBRE - Tamanios::TAMANIO_CANTIDAD_REGISTROS;
	char* bloqueDatos = new char[this->getTamanioBloqueDatos()];
	int nroBloque = this->siguienteBloque;
	int resultado = this->archivo->siguienteBloque(bloqueDatos, nroBloque, espLibre);
	
	if (resultado == ResultadosFisica::OK) {
		
		this->bloque->setDatos(bloqueDatos);
		registro = this->bloque->getNextRegister();
		this->siguienteBloque = ++nroBloque; 
		this->pedirBloque = false;
		
	} else {
		delete[] bloqueDatos;
		tamRegistro = 0;
	}
	
	return resultado;
}

/* Devuelve el siguiente registro con datos validos en el archivo de
 * datos. Si pudo obtener un registro, lo devuelve en 'registro' y
 * retorna ResultadosMetadata::OK.
 * Si no pudo, porque no existen más registros, 
 * retorna ResultadosMetadata::FIN_REGISTROS.
 */
int FileManager::siguienteRegistro(char* &registro, unsigned short &tamRegistro) {
	int resultado = ResultadosMetadata::OK;
	
	if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES) {
		
		if (pedirBloque) {
			if(this->getNextBloque(registro, tamRegistro) != ResultadosFisica::OK)
				resultado = ResultadosMetadata::FIN_REGISTROS;
		} else {
			registro = this->bloque->getNextRegister();
			if (!registro) {
				if(this->getNextBloque(registro, tamRegistro) != ResultadosFisica::OK)
					resultado = ResultadosMetadata::FIN_REGISTROS;
			}
			
		}
		
	} else if (this->getTipoOrganizacion() == TipoOrganizacion::REG_FIJOS) {
		
		char *bloqueDatos = new char[this->getTamanioBloqueDatos()];
		int nroBloque = this->siguienteBloque;
		
		if (this->archivo->siguienteBloque(bloqueDatos, nroBloque, 0) == ResultadosFisica::OK) {
			registro = bloqueDatos;
			tamRegistro = this->getTamanioBloqueDatos();
			this->siguienteBloque = ++nroBloque;
		} else {
			delete[] bloqueDatos;
			tamRegistro = 0;
			resultado = ResultadosMetadata::FIN_REGISTROS;
		}
		
	}
	
	return resultado;
	
}


void FileManager::eliminarArchivosTemporales(const string &extension) {
	
	string operacion("rm -fv *." + extension);
	system(operacion.c_str());
	
}


void FileManager::eliminarArchivoTemporal(const string &nombre, const string &extension) {
	
	string operacion("rm -fv " + nombre + "." + extension);
	system(operacion.c_str());
	
}


void FileManager::eliminarArchivoTemporal(const string &nombreArchivo) {
	
	string operacion("rm -fv " + nombreArchivo);
	system(operacion.c_str());
	
}

		
void FileManager::renombrarArchivosTemporales(ListaStrings &listaNombres,
											  const string &extVieja, const string &extNueva) {
	
	string operacion;
	
	for (ListaStrings::iterator it = listaNombres.begin(); it != listaNombres.end(); ++it) {
		
		operacion = "mv " + *it + "." + extVieja + " " + *it + "." + extNueva;
		system(operacion.c_str());	
	}
	
}
