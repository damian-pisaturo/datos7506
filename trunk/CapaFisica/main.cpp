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

#include "ArchivoRegistros.h"
#include "../ComuDatos/ComuDatos.h"
#include <iostream>

using namespace std;

int main(int argc, char**argv){

	if (argc > 2){
		
		ComuDatos pipe(argv);
		
		string nombreArchivo;
		unsigned char accion;
		
		accion = pipe.parametro(0);
		nombreArchivo = pipe.parametro(1);
			
		switch(accion){
		
			case OperacionesCapas::FISICA_LEER_NODO:{
								
				
								
			}break;
					
			
			case OperacionesCapas::FISICA_ESCRIBIR_NODO:{
				
				
				
			}break;
			
			case OperacionesCapas::FISICA_MODIFICAR_NODO:{
				
						
					
						
			}break;
					
			
			case OperacionesCapas::FISICA_ELIMINAR_NODO:{
								
								
								
			}break;
					
			case OperacionesCapas::FISICA_LEER_BUCKET:{
											
											
											
			}break;
			
			case OperacionesCapas::FISICA_ESCRIBIR_BUCKET:{
											
											
											
			}break;
						
			case OperacionesCapas::FISICA_MODIFICAR_BUCKET:{
											
											
											
			}break;
						
			case OperacionesCapas::FISICA_ELIMINAR_BUCKET:{
											
											
											
			}break;
						
						
			case OperacionesCapas::FISICA_LEER_DATO:{
											
											
											
			}break;
						
						
			case OperacionesCapas::FISICA_ESCRIBIR_DATO:{
											
											
											
			}break;
						
						
			case OperacionesCapas::FISICA_MIDIFICAR_DATO:{
											
											
											
			}break;
						
						
			case OperacionesCapas::FISICA_ELIMINAR_DATO:{
											
											
											
			}break;
			
			case default:{
				
			}
		
		}
	}else{
	
	}

		
	
}