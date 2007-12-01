///////////////////////////////////////////////////////////////////////////
//	Archivo   : BloqueListaManager.cpp
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase BloqueListaManager.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "BloqueListaManager.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: BloqueListaManager
//			(Permite la escritura, lectura, modificacion y eliminacion de
//			bloques de listas de claves primarias indexados en indices 
//			secundarios, accediendo remotamente a la Capa Fisica).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		BloqueListaManager::BloqueListaManager(unsigned int tamanioBloqueLista, string nombreArchivo):
			BloqueManager(tamanioBloqueLista, nombreArchivo + ".list")
		{ }
		
		BloqueListaManager::~BloqueListaManager()
		{ }
		
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

		int BloqueListaManager::leerBloqueDatos(unsigned int numLista, void* listaLeida)
		{
			char resultado = 0;

			// Instancia del pipe		
			ComuDatos* pipe = this->instanciarPipe();

			// Parametros de inicializacion de la Capa Fisica para
			// leer un nodo de disco.
			pipe->agregarParametro(OperacionesCapas::FISICA_LEER_LISTA, 0); // Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); // Nombre de archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); // Tamaño del bloque en disco.
			pipe->agregarParametro(numLista, 3); // Numero de nodo a leer dentro del archivo.

			// Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
			
			//Se chequea la validez del archivo
			pipe->leer(&resultado);
			
			if (resultado == ResultadosFisica::OK)
				//Se obtiene la lista solicitada.
				pipe->leer(this->getTamanioBloque(), (char*)listaLeida);
			}

			if (pipe)
				delete pipe;

			return resultado;
		}

		int BloqueListaManager::escribirBloqueDatos(const void* listaNueva)
		{
			char resultado = 0;
			int numLista   = 0; // Numero de bloque donde fue almacenada la lista en disco.
			
			//Instancia del pipe
			ComuDatos* pipe = this->instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica para
			//escribir un nodo en disco.		
			pipe->agregarParametro(OperacionesCapas::FISICA_ESCRIBIR_LISTA, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
		
			//Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
				
				if (resultado == ResultadosFisica::OK){				
					//Grabar el bloque con la lista en el archivo.
					pipe->escribir((char*)listaNueva, this->getTamanioBloque());
		
					//Obtener nueva posicion del bloque en el archivo. 
					pipe->leer(&numLista);
				}else
					numLista = resultado;
			}else
				numLista = resultado;

			if (pipe)
				delete pipe;
			
			return numLista;
		}

		
		int BloqueListaManager::escribirBloqueDatos(unsigned short numLista, const void* listaModif)
		{
			char resultado = 0;

			//Instancia del pipe
			ComuDatos* pipe = instanciarPipe();

			//Parametros de inicializacion de la Capa Fisica.
			pipe->agregarParametro(OperacionesCapas::FISICA_MODIFICAR_LISTA, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del nodo en disco.
			pipe->agregarParametro(numLista, 3); //Numero de lista a sobre-escribir.
				
			//Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
				
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
				
				if (resultado == ResultadosFisica::OK){
					// Grabar la lista a disco.
					pipe->escribir((char*)listaModif, this->getTamanioBloque());
		
					//Solicitar resultado de la comunicacion con la 
					//capa fisica.
					pipe->leer(&resultado);
				}
			}

			if (pipe)
				delete pipe;
			
			return resultado;
		}

		int BloqueListaManager::eliminarBloqueDatos(unsigned int numLista)
		{
			char resultado = 0;

			//Instancia del pipe
			ComuDatos* pipe = this->instanciarPipe();

			//Parametros para inicializar el pipe.
			pipe->agregarParametro(OperacionesCapas::FISICA_ELIMINAR_LISTA, 0); //Codigo de operacion.
			pipe->agregarParametro(this->getNombreArchivo(), 1); //Nombre del archivo.
			pipe->agregarParametro(this->getTamanioBloque(), 2); //Tamaño del bloque en disco.
			pipe->agregarParametro(numLista, 3); //Posicion del bloque a eliminar.

			//Se lanza el proceso de la capa fisica. 
			resultado = pipe->lanzar();
			
			if (resultado == ComuDatos::OK){
			
				//Se chequea la validez del archivo
				pipe->leer(&resultado);
				
				if (resultado == ResultadosFisica::OK)	
					//Solicitar resultado de la comunicacion con la 
					//capa fisica.
					pipe->leer(&resultado);
			
			}

			if (pipe)
				delete pipe;
			
			return resultado;
		}		

		int BloqueListaManager::buscarEspacioLibre(void* bloque, unsigned int espLibre)
		{
			return 0;
		}
		
		int BloqueListaManager::siguienteBloque(void* bloque)
		{
			return 0;
		}

