#include "ArchivoRegistros.h"
#include "../ComuDatos/ComuDatos.h"
#include <iostream>

using namespace std;

int main(int argc, char**argv){

	if (argc > 3){
		
		ComuDatos pipe(argv);
		
		string nombreArchivo,nombreArchivoEL;
		unsigned char accion;
		
		accion = pipe.parametro(0);
		nombreArchivo = pipe.parametro(1);
		nombreArchivoEL = pipe.parametro(2);
			
		switch(accion){
		
		case OperacionesCapas::FISICA_ESCRIBIR_NODO:{
			
			int lugarDondeGuardar=false;
			
			ArchivoRegistros miArchivoRegistros = new ArchivoRegistros(nombreArchivo);
			
			ArchivoRegistros miArchivoEL = new ArchivoRegistros(nombreArchivoEL,1);
			
			while(lugarDondeguardar != true)
			miArchivoEL.leer(&lugarDondeGuardar);
						
			if (lugarDondeGuardar != false){
			
				miArchivoRegistros.posicionarse(lugarDondeGuardar);
				miarchivoRegistros.escribir((pipe.leerString(miArchivoRegistros.getTamanioRegistro())).c_str());
				
			}
			
			
			
			
		}break;
		
		case default:{
			//NADA!!!
		}
		
		}
	}else{
	//NO HAGO ALGO.
	}

	

	
	
	
}