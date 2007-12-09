#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include "../../CapaFisica/ArchivoDatos/ArchivoDatos.h"
#include "../../Common/DefManager/DefinitionsManager.h"
#include "../../Common/Bloque/Bloque.h"
#include "../../Common/ResultadosMetadata.h"

class FileManager {
	
	private:
		ArchivoDatos* archivo;
		unsigned short tamBloque;
		unsigned char tipoOrg;
		int siguienteBloque;
		Bloque* bloque;
		ListaInfoRegistro* listaInfoReg;
		bool pedirBloque;
	
	public:
		/*
		 * Recibe el nombre del archivo (incluida la extensión)
		 */
		FileManager(const string &nombreArchivo, unsigned short tamBloque,
					unsigned char tipoOrg, ListaInfoRegistro* listaTiposAtributos);
		
		virtual ~FileManager();
		
		/* Devuelve en 'registro' el registro cuya posicion en el archivo es 
		 * 'numReg'.
		 * En 'tamReg' devuelve el tamaño del mismo.
		 * En 'quedanRegistros' indican si quedan registros en el archivo de datos.
		 */
		int leerRegistro(char* &registro, unsigned short numReg,
						 unsigned short &tamReg, bool &quedanRegistros);
		
		/* Escribe en el archivo de datos el registro pasado por parametro
		 * en la posicion del primer registro libre que encuentre, utilizando
		 * el archivo de control de espacio libre. Si todos los registro
		 * se encuentran ocupados, appendea al final del archivo.
		 * Devuelve la posicion en donde fue escrito finalmente.
		 */
		int escribirRegistro(char* registro, unsigned short tamRegistro);
		
		/* Devuelve el siguiente registro con datos validos en el archivo de
		 * datos. Si pudo obtener un registro, lo devuelve en 'registro' y
		 * retorna ResultadosMetadata::OK.
		 * Si no pudo, porque no existen más registros, 
		 * retorna ResultadosMetadata::FIN_REGISTROS.
		 */
		int siguienteRegistro(char* &registro, unsigned short &tamRegistro);
		
		/*
		 * Método que posiciona el puntero interno del archivo el comienzo
		 * del mismo.
		 */
		void comienzo();
		
		unsigned short getTamanioBloqueDatos() const { return this->tamBloque; }
		
		unsigned char getTipoOrganizacion() const { return this->tipoOrg; }
		
		/*
		 * Recibe la extensión de los archivos a borrar.
		 * Ejemplo: tmp, tmp.nfo
		 */
		static void eliminarArchivosTemporales(const string &extension);
		
		/*
		 * Recibe el nombre y la extensión del archivo a borrar.
		 * Ejemplo: tmp, tmp.nfo
		 */
		static void eliminarArchivoTemporal(const string &nombre, const string &extension);
		
		/*
		 * Recibe el nombre del archivo incluyendo la extensión.
		 * Ejemplo: persona.tmp, persona.tmp.nfo
		 */
		static void eliminarArchivoTemporal(const string &nombreArchivo);
		
		/*
		 * Recibe la lista con los nombres de los archivos a renombrar (sin la extensión),
		 * luego la extensión vieja y por último la extensión nueva.
		 * Las extensiones no deben incluir el punto inicial.
		 * Ejemplo: tmp, tmp.nfo
		 */
		static void renombrarArchivosTemporales(ListaStrings &listaNombres,
												const string &extVieja,
												const string &extNueva);
		
	private:
		int getNextBloque(char* &registro, unsigned short &tamRegistro);
		
};

#endif /*FILEMANAGER_H_*/
