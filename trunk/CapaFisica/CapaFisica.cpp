////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Funcion principal de la Capa Fisica.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "CapaFisica.h"


///////////////////////////////////////////////////////////////////////////
// Funcion principal
//------------------------------------------------------------------------
// Nombre: main()
//			(Funcion main() para la Capa Fisica que interpreta las
//			llamadas de la Capa de Indices).
///////////////////////////////////////////////////////////////////////////

	int main(int argc, char**argv)
	{	
		char resultado = ResFisica::OK;
		
		if (argc > 3){
			
			ArchivoBase* archivo = NULL;
			char* datos = NULL;			
			
			ComuDatos pipe(argv);
			
			string nombreArchivo;
			string buffer;
			unsigned char  accion    = 0;
			unsigned short tamBloque = 0;
			unsigned short numBloque = 0;
			
			//Obtener parametros comunes a todas las acciones
			//de la capa fisica.
			pipe.parametro(0, &accion);       //Accion a procesar.
			pipe.parametro(1, nombreArchivo); //Nombre del archivo a emplear.
			pipe.parametro(2, &tamBloque);    //Tamanio del bloque/registro del archivo.
				
			switch(accion){
			
			//Lectura de un nodo de un arbol
			case OperacionesCapas::FISICA_LEER_NODO:
			{
				//Obtencion del numero de bloque
				//dentro del archivo.
				pipe.parametro(3, &numBloque);
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				
				datos = new char[tamBloque + 1];
				
				//Lectura del nodo dentro del archivo.
				((ArchivoIndice*)archivo)->leerBloque(datos, tamBloque);
				*(datos + tamBloque + 1) = 0;
				buffer = datos;
				
				//Envio de datos a traves del pipe.
				pipe.escribir(buffer);
				
				delete[] datos;							
			}break;
			
			//Escritura de un nodo de arbol
			case OperacionesCapas::FISICA_ESCRIBIR_NODO:
			{
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
								
				//Obtencion del bucket a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del bucket a disco.
				//Se obtiene la posicion donde fue escrito.
				numBloque = ((ArchivoIndice*)archivo)->escribirBloque(buffer.c_str());
				
				//Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);												
			}break;
				
			case OperacionesCapas::FISICA_MODIFICAR_NODO:
			{
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				
				//Obtencion del nodo a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del nodo a disco en la posicion pasada por parametro.
				resultado = ((ArchivoIndice*)archivo)->escribirBloque(buffer.c_str(), numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
			}break;
			
			case OperacionesCapas::FISICA_ELIMINAR_NODO:			
			{
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				
				//Escritura del nodo a disco en la posicion pasada por parametro.				
				resultado = ((ArchivoIndice*)archivo)->eliminarBloque(numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
			}break;
						
			case OperacionesCapas::FISICA_LEER_BUCKET:
			{				
				//Obtencion del tamaño del bucket y su numero
				//dentro del archivo.				
				pipe.parametro(3, &numBloque);
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				datos = new char[tamBloque + 1];
				
				//Lectura del bucket dentro del archivo.
				resultado = ((ArchivoIndiceHash*)archivo)->leerBloque(datos, numBloque);
				*(datos + tamBloque + 1) = 0;
				buffer = datos;
				
				//Envio de datos a traves del pipe.
				pipe.escribir(buffer);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
				delete[] datos;
			}break;
				
			case OperacionesCapas::FISICA_ESCRIBIR_BUCKET:
			{
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				//Obtencion del bucket a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del bucket a disco.
				//Se obtiene la posicion donde fue escrito.
				numBloque = ((ArchivoIndiceHash*)archivo)->escribirBloque(buffer.c_str());
				
				//Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);												
			}break;
							
			case OperacionesCapas::FISICA_MODIFICAR_BUCKET:
			{
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				//Obtencion del bucket a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del bucket a disco en la posicion pasada por parametro.
				resultado = ((ArchivoIndiceHash*)archivo)->escribirBloque(buffer.c_str(), numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
			}break;
							
			case OperacionesCapas::FISICA_ELIMINAR_BUCKET:
			{
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				//Eliminacion del bucket				
				resultado = ((ArchivoIndiceHash*)archivo)->eliminarBloque(numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
			}break;
			
			case OperacionesCapas::FISICA_LEER_TABLA_HASH:
			{	
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				unsigned short tamanio = 0;
				unsigned int* buckets = NULL;
				
				((ArchivoIndiceHash*)archivo)->leerTabla(&tamanio, buckets);
			
				buffer = (char*) buckets;
				
				pipe.escribir(tamanio);
				pipe.escribir(buffer);	
			}break;
				
			case OperacionesCapas::FISICA_ESCRIBIR_TABLA_HASH:
			{
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				unsigned short tamanio = 0;
				unsigned int* buckets = NULL;
				
				pipe.leer(&tamanio);
				pipe.leer(sizeof(unsigned int)*tamanio, buffer);
				
				buckets = (unsigned int*) buffer.c_str();
				
				((ArchivoIndiceHash*)archivo)->escribirTabla(tamanio, buckets);
			}break;
			
			case OperacionesCapas::FISICA_LEER_DATO:
			{
				
			}break;
			
			case OperacionesCapas::FISICA_ESCRIBIR_DATO:
			{
				
			}break;
							
							
			case OperacionesCapas::FISICA_MODIFICAR_DATO:
			{								
												
			}break;
				
			case OperacionesCapas::FISICA_ELIMINAR_DATO:
			{								
												
			}break;
				
			default:
			{
			}break;
					
			
			
			}
		}else resultado = ResFisica::CANT_ARGUMENTOS_INVALIDA;
		
		return resultado;		
	}
	
			
		
