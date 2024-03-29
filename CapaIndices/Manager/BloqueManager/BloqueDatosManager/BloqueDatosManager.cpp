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
			BloqueManager(tamanioBloque, nombreArchivo + ".data")
		{
			this->tipoOrg            = tipoOrg;
			this->bloqueSiguiente    = 0;
			this->espacioBloqueLibre = this->getTamanioBloque() - Tamanios::TAMANIO_LONGITUD - Tamanios::TAMANIO_ESPACIO_LIBRE;
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
			
			//Se chequea la validez del archivo
			pipe->leer(&resultado);
			
			if (resultado == ResultadosFisica::OK){
				//Se obtiene el resultado del proceso de lectura del bloque.
				pipe->leer(&resultado);
	
				if (resultado == ResultadosFisica::OK) 
					pipe->leer(this->getTamanioBloque(), (char*)bloqueLeido);
			}

			if (pipe)
				delete pipe;

			return resultado;
		}

		int BloqueDatosManager::escribirBloqueDatos(const void* bloqueNuevo)
		{
			char resultado = 0;
			int numBloque  = 0;
			unsigned int espLibre = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = this->instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica para
			//escribir un nodo en disco.		
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_DATO, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
			pipe->agregarParametro(this->getTipoOrganizacion(), 3); //Tipo de organizacion del archivo de datos.
			
			if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES){
				espLibre = this->getTamanioBloque() - *((unsigned short*)bloqueNuevo);
				pipe->agregarParametro(espLibre, 4); //Espacio libre dentro del bloque.
			}				
			
			//Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
			
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
							
				if (resultado == ResultadosFisica::OK){
					
					//Grabar el bloque en el archivo.
					pipe->escribir((char*)bloqueNuevo , this->getTamanioBloque());
		
					//Obtener nueva posicion del bloque en el archivo. 
					pipe->leer(&numBloque);
					
				}else
					numBloque = resultado;
			}else
				numBloque = resultado;

			if (pipe)
				delete pipe;
			
			return numBloque;
		}
		
		/* Utiliza ComuDatos para comunicarse con la Capa Fisica y
		 * sobre-escribir el bloque modificado en el disco
		 * el bloqueNuevo en el archivo especificado por nombreArchivo.
		 */
		int BloqueDatosManager::escribirBloqueDatos(unsigned short numBloque, const void* bloqueModif)
		{
			char resultado = 0;
			unsigned int espLibre = 0;

			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica.
			pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_DATO, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
			pipe->agregarParametro(numBloque, 3); //Numero de nodo a sobre-escribir.
			pipe->agregarParametro(this->getTipoOrganizacion(), 4); //Tipo de organizacion del archivo de datos.
			
			if (this->getTipoOrganizacion() == TipoOrganizacion::REG_VARIABLES){
				espLibre = this->getTamanioBloque() - *((unsigned short*)bloqueModif);
				pipe->agregarParametro(espLibre, 5); //Espacio libre dentro del bloque.
			}	
			
			//Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
				
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
				
				if (resultado == ResultadosFisica::OK){
	
					//Grabar el buffer en el archivo.
					pipe->escribir((char*)bloqueModif, this->getTamanioBloque());
		
					//Solicitar resultado de la comunicacion con la 
					//capa fisica.
					pipe->leer(&resultado);
				}
			}

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
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
			
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
				
				if (resultado == ResultadosFisica::OK){
	
					//Solicitar resultado de la comunicacion con la 
					//capa fisica.
					pipe->leer(&resultado);
				}
			}

			if (pipe)
				delete pipe;

			return resultado;
		}

		int BloqueDatosManager::buscarEspacioLibre(void* bloque, unsigned int espLibre)
		{
			char resultado = 0;
			int numBloque  = 0;			

			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros para inicializar el pipe.
			pipe->agregarParametro(OperacionesCapas::FISICA_BUSCAR_ESPACIO_LIBRE, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
			pipe->agregarParametro(espLibre, 3); //Espacio libre requerido dentro del bloque.			

			//Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
				
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
				
				if (resultado == ResultadosFisica::OK){
				
					//Se obtiene el numero de bloque que contiene el espacio libre solicitado.
					//Si ningun bloque cumple con el requisito, 'numBloque' valdra BLOQUES_OCUPADOS.
					//Si el archivo aun no contiene ningun bloque, valdra ARCHIVO_VACIO.
					pipe->leer(&numBloque);
					
					if ( (numBloque != ResultadosFisica::BLOQUES_OCUPADOS) && (numBloque != ResultadosFisica::ARCHIVO_VACIO) )
						pipe->leer(this->getTamanioBloque(), (char*)bloque);
				}else 
					numBloque = resultado;
			}else 
				numBloque = resultado;

			if (pipe)
				delete pipe;
			
			return numBloque;
		}
		
		int BloqueDatosManager::siguienteBloque(void* bloque)
		{
			char resultado = 0;
			
			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros para inicializar el pipe.
			pipe->agregarParametro(OperacionesCapas::FISICA_SIGUIENTE_BLOQUE, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
			pipe->agregarParametro(this->bloqueSiguiente, 3); //Numero del ultimo bloque de datos validos levantado.
			pipe->agregarParametro(this->espacioBloqueLibre, 4); //Espacio en bytes que ocupa un bloque de datos libre en disco.
			pipe->agregarParametro(this->getTipoOrganizacion(), 5); //Tipo de organizacion del archivo de datos.
			
			//Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
				
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
				
				if (resultado == ResultadosFisica::OK){
					
					//Se chequea si existen más bloques
					//de datos en el archivo.
					pipe->leer(&resultado);
					
					if (resultado == ResultadosFisica::OK){
						// Se obtiene el bloque de datos
						pipe->leer(this->getTamanioBloque(), (char*)bloque);
						
						// Se obtiene el numero de bloque que
						// se acaba de levantar de disco.
						pipe->leer(&this->bloqueSiguiente);
						
						++(this->bloqueSiguiente);
					}
				
				}
			}
			
			if (pipe)
				delete pipe;
			
			return resultado;				
		}
