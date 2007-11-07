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
				//Obtencion del numero de nodo
				//dentro del archivo.
				pipe.parametro(3, &numBloque);
	
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				
				buffer = new char[tamBloque*sizeof(char) + 1];
				
				//Lectura del nodo dentro del archivo.
				resultado = ((ArchivoIndice*)archivo)->leerBloque(buffer, numBloque);
										
				//Envio del resultado de la operacion de lectura.
				pipe.escribir(resultado);
							
				if (resultado == ResFisica::OK)
					//Envio de datos a traves del pipe.
					pipe.escribir(buffer, tamBloque);	
											
			}break;
			
			//Escritura de un nodo de arbol
			case OperacionesCapas::FISICA_ESCRIBIR_NODO:
			{		
				cout << "Te escribo la raiz, Ernesto" << endl;
				
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				buffer = new char[tamBloque*sizeof(char) + 1];
								
				//Obtencion del nodo escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				//Escritura del nodo a disco.
				//Se obtiene la posicion donde fue escrito.				
				numBloque = ((ArchivoIndice*)archivo)->escribirBloque(buffer);
				
				//Se envia la nueva posicion del nodo.
				pipe.escribir(numBloque);		
				
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
				
			}break;
				
			case OperacionesCapas::FISICA_ESCRIBIR_BUCKET:
			{
				cout << "Bajame ese renglon que subiste, Sabesqueson" << endl;
				buffer = new char[tamBloque*sizeof(char)];
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
				unsigned short tamanio = 0;
				unsigned int* buckets = NULL;
				
				((ArchivoIndiceHash*)archivo)->leerTabla(&tamanio, buckets);
				
				pipe.escribir(tamanio);
				pipe.escribir((char*)buckets, tamanio*sizeof(unsigned int));
				
				cout<<"tamanio lado nico capo:"<<tamanio<<endl;
				if (buckets == NULL)
					cout<<"si, es NULL idiota!"<<endl;
				
				if (buckets)
					delete[] buckets;
	
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
							
				if (resultado == ResFisica::OK)
					//Envio de datos a traves del pipe.
					pipe.escribir(buffer, tamBloque);	
					
			}break;
			
			case OperacionesCapas::FISICA_ESCRIBIR_DATO:
			{
				archivo = new ArchivoIndice(nombreArchivo, tamBloque);
				buffer = new char[tamBloque*sizeof(char) + 1];
											
				int tipoOrg = 0;
				
				//Obtencion del tipo de organizacion del archivo.
				pipe.parametro(3, &tipoOrg);
				
				//Obtencion del bloque a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
				
				if (tipoOrg == TipoOrganizacion::REG_VARIABLES){
					archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
				
					//Escritura del bloque a disco.
					//Se obtiene la posicion donde fue escrito.
					numBloque = ((ArchivoDatosBloques*)archivo)->escribirBloque(buffer);
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
				int tipoOrg = 0;
				buffer = new char[tamBloque*sizeof(char)];
				pipe.parametro(3, &numBloque);				
	
				//Obtencion del tipo de organizacion del archivo.
				pipe.parametro(3, &tipoOrg);				
				
				//Obtencion del bloque a escribir a traves del pipe.
				pipe.leer(tamBloque, buffer);
							
				if (tipoOrg == TipoOrganizacion::REG_VARIABLES){
					archivo = new ArchivoDatosBloques(nombreArchivo, tamBloque);
				
					//Modificacion del bloque en disco.
					resultado = ((ArchivoDatosBloques*)archivo)->escribirBloque(buffer, numBloque);
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
				pipe.parametro(3, &tipoOrg);
								
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
				
			default:
				
				resultado = ResFisica::OPERACION_INVALIDA;
				
			break;		
			
			}
			
			//Liberacion de recursos de memoria del buffer.
			if (buffer)
				delete[] buffer;
			
			//Cierre del archivo.			
			if (archivo)
				delete archivo;
			
		}else resultado = ResFisica::CANT_ARGUMENTOS_INVALIDA;
		
		return resultado;		
	}	

/*

#include "../CapaIndices/Indices/IndiceHash.h"

int main(int estaRePedanticEsto, char** hinchaPelotas)
{	 
	RegisterInfo* regInfo = new RegisterInfo();
	Indice *manuColoquio = new IndiceHash(regInfo->getParameterList(),512,"ernesto");
	
	int entero1 = 25;
	char* cadena = new char[5];
	strcpy(cadena, "HOLA");
	unsigned short longCadena = 4;
	int entero2 = 55;

	unsigned short tamReg = 14; // 4 + 2 + 4 + 4
	char *registro = new char [tamReg + 2];

	// Llena el registro.
	memcpy(registro,&tamReg,Tamanios::TAMANIO_LONGITUD);
	memcpy(registro + 2,&entero1,sizeof(int));
	memcpy(registro + 6,&longCadena,Tamanios::TAMANIO_LONGITUD);
	memcpy(registro + 8,cadena,longCadena);
	memcpy(registro + 12,&entero2,sizeof(int));	
	
	manuColoquio->insertar(new ClaveEntera(55),registro);
	
	delete manuColoquio;
	delete[] cadena;
	delete[] registro;
	
	return 0;
}


*/
