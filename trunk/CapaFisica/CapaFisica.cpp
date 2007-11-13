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
		char resultado = ResultadosFisica::OK;
		
		if (argc > 3){

			//Archivo de escritura/lectura.
			ArchivoBase* archivo = NULL;			
			
			//Instancia del pipe de comunicacion entre capas.
			ComuDatos pipe(argv);
			
			string nombreArchivo;
			char* buffer  = NULL;
			int accion    = 0;
			int tamBloque = 0;
			int numBloque = 0;
			int espLibre  = 0;
	
			//Obtener parametros comunes a todas las acciones
			//de la capa fisica.
			pipe.parametro(0, &accion);       //Accion a procesar.
			pipe.parametro(1, nombreArchivo); //Nombre del archivo a emplear.
			pipe.parametro(2, &tamBloque);  //Tamanio del bloque/registro del archivo.
			
			switch(accion){
			
			//Lectura de un nodo de un arbol
			case OperacionesCapas::FISICA_LEER_NODO:
			{
				//Obtencion del numero de nodo
				//dentro del archivo.
				pipe.parametro(3, &numBloque);
	
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);
				
				buffer = new char[tamBloque*sizeof(char) + 1];
				
				//Lectura del nodo dentro del archivo.
				resultado = ((ArchivoIndiceArbol*)archivo)->leerBloque(buffer, numBloque);
										
				//Envio del resultado de la operacion de lectura.
				pipe.escribir(resultado);
							
				if (resultado == ResultadosFisica::OK)
					//Envio de datos a traves del pipe.
					pipe.escribir(buffer, tamBloque);	
											
			}break;
			
			//Escritura de un nodo de arbol
			case OperacionesCapas::FISICA_ESCRIBIR_NODO:
			{	
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);
				buffer = new char[tamBloque*sizeof(char)];
								
				//Obtencion del nodo escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del nodo a disco.
				//Se obtiene la posicion donde fue escrito.				
				numBloque = ((ArchivoIndiceArbol*)archivo)->escribirBloque(buffer);
				
				//Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);		
				
			}break;
				
			case OperacionesCapas::FISICA_MODIFICAR_NODO:
			{
				buffer = new char[tamBloque*sizeof(char)];
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);
				
			//Obtencion del nodo a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del nodo a disco en la posicion pasada por parametro.
				resultado = ((ArchivoIndice*)archivo)->escribirBloque(buffer, numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
			}break;
			
			case OperacionesCapas::FISICA_ELIMINAR_NODO:			
			{
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);
				
				//Escritura del nodo a disco en la posicion pasada por parametro.				
				resultado = ((ArchivoIndiceArbol*)archivo)->eliminarBloque(numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
			}break;
						
			case OperacionesCapas::FISICA_LEER_BUCKET:
			{				
				//Obtencion su numero de bucket
				//dentro del archivo.				
				pipe.parametro(3, &numBloque);
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				buffer = new char[tamBloque*sizeof(char)];
				
				//Lectura del bucket dentro del archivo.
				cout << "Leyendo desde la fisica el bucket " << numBloque << endl;
				resultado = ((ArchivoIndiceHash*)archivo)->leerBloque(buffer, numBloque);
				
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
				if (resultado == ResultadosFisica::OK)
					//Envio de datos a traves del pipe.
					pipe.escribir(buffer, tamBloque);
				
			}break;
				
			case OperacionesCapas::FISICA_ESCRIBIR_BUCKET:
			{				
				buffer  = new char[tamBloque*sizeof(char)];
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				
				// Obtencion del bucket a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				// Escritura del bucket a disco.
				// Se obtiene la posicion donde fue escrito.
				numBloque = ((ArchivoIndiceHash*)archivo)->escribirBloque(buffer);
				
				// Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);				
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
				unsigned int tamanio  = 0;
				unsigned int* buckets = NULL;
				
				((ArchivoIndiceHash*)archivo)->leerTabla(&tamanio, buckets);
								
				pipe.escribir(tamanio);
				
				if (tamanio > 0)
					pipe.escribir((char*)buckets, tamanio*sizeof(unsigned int));					
						
				if (buckets)
					delete[] buckets;
	
			}break;
				
			case OperacionesCapas::FISICA_ESCRIBIR_TABLA_HASH:
			{
				archivo = new ArchivoIndiceHash(nombreArchivo, tamBloque);
				unsigned int tamanio = 0;
				unsigned int* buckets = NULL;
								
				pipe.leer(&tamanio);
				
				buckets = new unsigned int[tamanio];
				pipe.leer(sizeof(unsigned int)*tamanio, (char*)buckets);
				
				((ArchivoIndiceHash*)archivo)->escribirTabla(tamanio, buckets);
				
				if (buckets)
					delete[] buckets;
			}break;
			
			case OperacionesCapas::FISICA_ESCRIBIR_NODO_DOBLE:
			{
				buffer = new char[2*tamBloque*sizeof(char)];
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);
				
				// Obtencion del primer nodo a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				// Obtencion del segundo nodo a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer + tamBloque);
				
				// Escritura del bucket a disco.
				// Se obtiene la posicion donde fue escrito.
				numBloque = ((ArchivoIndiceArbol*)archivo)->escribirBloqueDoble(buffer);
				
				// Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);	
				
			}break;
			
			case OperacionesCapas::FISICA_LEER_NODO_DOBLE:
			{
				//Obtencion del primer numero de nodo a leer			
				pipe.parametro(3, &numBloque);
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);
				
				buffer = new char[2*tamBloque*sizeof(char)];
				
				//Lectura del bucket dentro del archivo.
				resultado = ((ArchivoIndiceArbol*)archivo)->leerBloqueDoble(buffer, numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
				if (resultado == ResultadosFisica::OK){
					//Envio del primer nodo a traves del pipe.
					pipe.escribir(buffer, tamBloque);
					
					//Envio del segundi nodo a traves del pipe.
					pipe.escribir(buffer + tamBloque, tamBloque);
				}	
			}break;
			
			case OperacionesCapas::FISICA_MODIFICAR_NODO_DOBLE:
			{
				buffer = new char[2*tamBloque*sizeof(char)];
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);				
				
				// Obtencion del primer nodo a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				// Obtencion del segundo nodo a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer + tamBloque);
				
				//Escritura del nodo a disco en la posicion pasada por parametro.
				resultado = ((ArchivoIndiceArbol*)archivo)->escribirBloqueDoble(buffer, numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
			}break;
			
			case OperacionesCapas::FISICA_ELIMINAR_NODO_DOBLE:
			{
				//Obtencion del numero del primer nodo a eliminar
				pipe.parametro(3, &numBloque);
				
				archivo = new ArchivoIndiceArbol(nombreArchivo, tamBloque);
				
				//Eliminacion de ambos nodos				
				resultado = ((ArchivoIndiceArbol*)archivo)->eliminarBloqueDoble(numBloque);
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
			}break;
			
			case OperacionesCapas::FISICA_LEER_DATO:
			{
				int tipoOrg = 0;
				
				//Obtencion del numero de bloque
				//dentro del archivo.
				pipe.parametro(3, &numBloque);
				
				//Obtencion del tipo de organizacion del archivo.
				pipe.parametro(4, &tipoOrg);
				
				buffer = new char[tamBloque*sizeof(char) + 1];
				
				if (tipoOrg == TipoOrganizacion::REG_VARIABLES){
					archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
				
					//Lectura del bloque dentro del archivo.
					resultado = ((ArchivoDatosBloques*)archivo)->leerBloque(buffer, numBloque);
				}else if (TipoOrganizacion::REG_FIJOS){
					archivo = new ArchivoDatosRegistros(nombreArchivo, tamBloque);
					
					//Lectura del bloque dentro del archivo
					resultado = ((ArchivoDatosRegistros*)archivo)->leerRegistro(buffer, numBloque);
				}
				
				//Envio del resultado de la operacion de lectura.
				pipe.escribir(resultado);
							
				if (resultado == ResultadosFisica::OK)
					//Envio de datos a traves del pipe.
					pipe.escribir(buffer, tamBloque);	
					
			}break;
			
			case OperacionesCapas::FISICA_ESCRIBIR_DATO:
			{
				int espLibre = 0;
				int tipoOrg  = 0;
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				buffer  = new char[tamBloque*sizeof(char)];			
				
				//Obtencion del tipo de organizacion del archivo.
				pipe.parametro(3, &tipoOrg);		

				//Obtencion del bloque a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				if (tipoOrg == TipoOrganizacion::REG_VARIABLES){
					//Obtencion del espacio libre dentro del bloque.
					pipe.parametro(4, &espLibre);
								
					archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
					
					//Escritura del bloque a disco.
					//Se obtiene la posicion donde fue escrito.
					numBloque = ((ArchivoDatosBloques*)archivo)->escribirBloque(buffer, espLibre);
				}else if (TipoOrganizacion::REG_FIJOS){
					archivo = new ArchivoDatosRegistros(nombreArchivo, tamBloque);
					
					//Escritura del registro a disco.
					//Se obtiene la posicion donde fue escrito.
					numBloque = ((ArchivoDatosRegistros*)archivo)->escribirRegistro(buffer);
				}
				
				numBloque = ((ArchivoIndice*)archivo)->escribirBloque(buffer);
				
				//Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);
				
			}break;
							
							
			case OperacionesCapas::FISICA_MODIFICAR_DATO:
			{								
				int tipoOrg  = 0;
				int espLibre = 0;
				buffer = new char[tamBloque*sizeof(char)];
				pipe.parametro(3, &numBloque);				
	
				//Obtencion del tipo de organizacion del archivo.
				pipe.parametro(4, &tipoOrg);			

				//Obtencion del bloque a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
							
				if (tipoOrg == TipoOrganizacion::REG_VARIABLES){
					//Obtencion del espacio libre dentro del bloque.
					pipe.parametro(5, &espLibre);

					archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
				
					//Modificacion del bloque en disco.
					resultado = ((ArchivoDatosBloques*)archivo)->escribirBloque(buffer, numBloque, espLibre);
				}else if (TipoOrganizacion::REG_FIJOS){
					archivo = new ArchivoDatosRegistros(nombreArchivo, tamBloque);
					
					//Modificacion del registro en disco.
					resultado = ((ArchivoDatosRegistros*)archivo)->escribirRegistro(buffer, numBloque);
				}
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);						
			}break;
				
			case OperacionesCapas::FISICA_ELIMINAR_DATO:
			{								
				int tipoOrg = 0;
				pipe.parametro(3, &numBloque);
				
				//Obtencion del tipo de organizacion del archivo.
				pipe.parametro(4, &tipoOrg);
								
				if (tipoOrg == TipoOrganizacion::REG_VARIABLES){
					archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
				
					//Eliminacion del bloque.
					resultado = ((ArchivoDatosBloques*)archivo)->eliminarBloque(numBloque);
				}else if (TipoOrganizacion::REG_FIJOS){
					archivo = new ArchivoDatosRegistros(nombreArchivo, tamBloque);
					
					//Eliminacion del registro.
					resultado = ((ArchivoDatosRegistros*)archivo)->eliminarRegistro(numBloque);
				}
				
				//Envio del resultado de la operacion a traves del pipe.
				pipe.escribir(resultado);
				
			}break;
			
			case OperacionesCapas::FISICA_BUSCAR_ESPACIO_LIBRE:
			{
				int numBloque = 0;
				
				//Obtencion del espacio libre a buscar.
				pipe.parametro(3, &espLibre);
				buffer = new char[tamBloque*sizeof(char)];
				
				archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
				
				//Obtencion del numero del primer bloque que contiene el
				//espacio libre requerido
				numBloque = ((ArchivoDatosBloques*)archivo)->buscarBloque(buffer, espLibre);
				
				if (numBloque != ResultadosFisica::BLOQUES_OCUPADOS)
					//Se envia el bloque si existe al menos uno que cumpla el
					//criterio de busqueda (espacio libre suficiente).
					pipe.escribir(buffer, tamBloque);	
				
				pipe.escribir(numBloque);				
			
			}break;
				
			default:
				
				resultado = ResultadosFisica::OPERACION_INVALIDA;
				
			break;		
			
			}
			
			//Liberacion de recursos de memoria del buffer.
			if (buffer)
				delete[] buffer;
			
			//Cierre del archivo.			
			if (archivo)
				delete archivo;
			
		}else resultado = ResultadosFisica::CANT_ARGUMENTOS_INVALIDA;
	
		return resultado;		
	}
