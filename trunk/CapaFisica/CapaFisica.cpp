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
			
			ComuDatos pipe(argv);
			
			string nombreArchivo;
			char* buffer  = NULL;
			int accion    = 0;
			int tamBloque = 0;
			int numBloque = 0;
	
			//Obtener parametros comunes a todas las acciones
			//de la capa fisica.
			pipe.parametro(0, &accion);       //Accion a procesar.
			pipe.parametro(1, nombreArchivo); //Nombre del archivo a emplear.
			pipe.parametro(2, &tamBloque);  //Tamanio del bloque/registro del archivo.
			
			switch(accion){
			
			//Lectura de un nodo de un arbol
			case OperacionesCapas::FISICA_LEER_NODO:
			{
				//Obtencion del numero de bloque
				//dentro del archivo.
				pipe.parametro(3, &numBloque);
	
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				
				buffer = new char[tamBloque*sizeof(char) + 1];
				
				//Lectura del nodo dentro del archivo.
				resultado = ((ArchivoIndice*)archivo)->leerBloque(buffer, numBloque);
										
				//Envio del resultado de la operacion de lectura.
				pipe.escribir(resultado);
							
				//if (resultado == ResFisica::OK)
					//Envio de datos a traves del pipe.
					pipe.escribir(buffer, tamBloque);	
				
				delete[] buffer;							
			}break;
			
			//Escritura de un nodo de arbol
			case OperacionesCapas::FISICA_ESCRIBIR_NODO:
			{				
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				buffer = new char[tamBloque*sizeof(char) + 1];
								
				//Obtencion del bucket a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del bucket a disco.
				//Se obtiene la posicion donde fue escrito.				
				numBloque = ((ArchivoIndice*)archivo)->escribirBloque(buffer);
				
				cout << "Lo escribi en " << numBloque << endl;
				//Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);		
				
				delete[] buffer;
				//delete archivo;
			}break;
				
			case OperacionesCapas::FISICA_MODIFICAR_NODO:
			{
				buffer = new char[tamBloque*sizeof(char)];
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				
				//Obtencion del nodo a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del nodo a disco en la posicion pasada por parametro.
				resultado = ((ArchivoIndice*)archivo)->escribirBloque(buffer, numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
				delete[] buffer;
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
				
				buffer = new char[tamBloque*sizeof(char) + 1];
				
				//Lectura del bucket dentro del archivo.
				resultado = ((ArchivoIndiceHash*)archivo)->leerBloque(buffer, numBloque);
						
				//Envio de datos a traves del pipe.
				pipe.escribir(buffer, tamBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
				delete[] buffer;
			}break;
				
			case OperacionesCapas::FISICA_ESCRIBIR_BUCKET:
			{
				buffer = new char[tamBloque*sizeof(char)];
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				//Obtencion del bucket a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del bucket a disco.
				//Se obtiene la posicion donde fue escrito.
				numBloque = ((ArchivoIndiceHash*)archivo)->escribirBloque(buffer);
				
				//Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);				
				
				delete[] buffer;
			}break;
							
			case OperacionesCapas::FISICA_MODIFICAR_BUCKET:
			{
				buffer = new char[tamBloque*sizeof(char)];
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				//Obtencion del bucket a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del bucket a disco en la posicion pasada por parametro.
				resultado = ((ArchivoIndiceHash*)archivo)->escribirBloque(buffer, numBloque);
				
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
				
				pipe.escribir(tamanio);
				pipe.escribir((char*)buckets, tamanio*sizeof(unsigned int));	
			}break;
				
			case OperacionesCapas::FISICA_ESCRIBIR_TABLA_HASH:
			{
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				unsigned short tamanio = 0;
				unsigned int* buckets = NULL;
				
				pipe.leer(&tamanio);
				
				buckets = new unsigned int[tamanio];
				pipe.leer(sizeof(unsigned int)*tamanio, (char*)buckets);
				
				((ArchivoIndiceHash*)archivo)->escribirTabla(tamanio, buckets);
				
				delete[] buckets;
			}break;
			
			case OperacionesCapas::FISICA_ESCRIBIR_NODO_DOBLE:
			{
				//TODO
			}break;
			
			case OperacionesCapas::FISICA_LEER_NODO_DOBLE:
			{
				//TODO			
			}break;
			
			case OperacionesCapas::FISICA_MODIFICAR_NODO_DOBLE:
			{
				//TODO	
			}break;
			
			case OperacionesCapas::FISICA_ELIMINAR_NODO_DOBLE:
			{
				//TODO	
			}break;
			
			case OperacionesCapas::FISICA_LEER_DATO:
			{
				//TODO
			}break;
			
			case OperacionesCapas::FISICA_ESCRIBIR_DATO:
			{
				//TODO
			}break;
							
							
			case OperacionesCapas::FISICA_MODIFICAR_DATO:
			{								
				//TODO						
			}break;
				
			case OperacionesCapas::FISICA_ELIMINAR_DATO:
			{								
				//TODO						
			}break;
				
			default:
			{
				//TODO
			}break;
					
			
			
			}
			
		}else resultado = ResFisica::CANT_ARGUMENTOS_INVALIDA;
		
		return resultado;	
		
	}


	
			
		
