///////////////////////////////////////////////////////////////////////////
//	Archivo   : BloqueDatosManager.cpp
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase BloqueDatosManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "BloqueDatosManager.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BloqueDatosManager
//			(Permite la escritura, lectura, modificacion y eliminacion de
//			bloques de datos indexados, accediendo remotamente a la 
// 			Capa Fisica).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		BloqueDatosManager::BloqueDatosManager(unsigned int tamanioBloque, string nombreArchivo, unsigned char tipoOrg):
			Manager(tamanioBloque, nombreArchivo + ".data")
		{
			this->tipoOrg = tipoOrg;
		}
		
		BloqueDatosManager::~BloqueDatosManager()
		{ }
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		int BloqueDatosManager::leerBloqueDatos(unsigned int numBloque, void* bloqueLeido)
		{
			char resultado = 0;

			//Instancia del pipe		
			ComuDatos* pipe = this->instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica para
			//leer un nodo de disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_DATO, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre de archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
			pipe->agregarParametro(numBloque, 3); //Numero de nodo a leer dentro del archivo
			pipe->agregarParametro(this->getTipoOrganizacion(), 4); //Tipo de organizacion del archivo.

			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();

			//Se obtiene el resultado del proceso de lectura del bloque.
			pipe->leer(&resultado);

			if (resultado == ResultadosFisica::OK)
				pipe->leer(this->getTamanioBloque(), (char*)bloqueLeido);

			if (pipe)
				delete pipe;

			return resultado;
		}

		int BloqueDatosManager::escribirBloqueDatos(const void* bloqueNuevo, unsigned int espLibre)
		{
			int numBloque = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = this->instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica para
			//escribir un nodo en disco.		
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_DATO, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
			pipe->agregarParametro(this->getTipoOrganizacion(), 3); //Tipo de organizacion del archivo de datos.
			
			if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES)
				pipe->agregarParametro(espLibre, 4); //Espacio libre dentro del bloque.

			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();

			//Grabar el bloque en el archivo.
			pipe->escribir((char*)bloqueNuevo, this->getTamanioBloque());

			//Obtener nueva posicion del bloque en el archivo. 
			pipe->leer(&numBloque);

			if (pipe)
				delete pipe;
			
			return numBloque;
		}
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque modificado en el disco
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		int BloqueDatosManager::escribirBloqueDatos(unsigned short numBloque, const void* bloqueModif, unsigned int espLibre)
		{
			int resultado = 0;

			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica.
			pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_DATO, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
			pipe->agregarParametro(numBloque, 3); //Numero de nodo a sobre-escribir.
			pipe->agregarParametro(this->getTipoOrganizacion(), 4); //Tipo de organizacion del archivo de datos.
			
			if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES)
				pipe->agregarParametro(espLibre, 5); //Espacio libre dentro del bloque.
			
			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
	
			//Grabar el buffer en el archivo.
			pipe->escribir((char*)bloqueModif, this->getTamanioBloque());

			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);

			if (pipe)
				delete pipe;

			return resultado;
		}
		

		int BloqueDatosManager::eliminarBloqueDatos(unsigned int numBloque)
		{
			char resultado = 0;

			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros para inicializar el pipe.
			pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_DATO, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
			pipe->agregarParametro(numBloque, 3); //Posicion del bloque a eliminar.
			pipe->agregarParametro(this->getTipoOrganizacion(), 4); //Tipo de organizacion del archivo de datos.

			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();

			//Solicitar resultado de la comunicacion con la 
			//capa fisica.
			pipe->leer(&resultado);

			if (pipe)
				delete pipe;

			return resultado;
		}

		int BloqueDatosManager::buscarEspacioLibre(void* bloque, unsigned int espLibre)
		{
			int numBloque = 0;			

			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros para inicializar el pipe.
			pipe->agregarParametro(OperacionesCapas::FISICA_BUSCAR_ESPACIO_LIBRE, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
			pipe->agregarParametro(espLibre, 3); //Espacio libre requerido dentro del bloque.			

			//Se lanza el proceso de la capa fisica. 
			pipe->lanzar();
			
			//Se obtiene el numero de bloque que contiene el espacio libre solicitado.
			//Si ningun bloque cumple con el requisito, 'numBloque' valdra BLOQUES_OCUPADOS.
			pipe->leer(&numBloque);
			
			if (numBloque != ResultadosFisica::BLOQUES_OCUPADOS)
				pipe->leer(this->getTamanioBloque(), (char*)bloque);

			if (pipe)
				delete pipe;
			
			return numBloque;
		}
