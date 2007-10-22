#include "Bloque.h"


Bloque::Bloque(int num,int tam)
{
	numero = num;
	tamanio = tam;
	//contendrá offset a espacio libre, cantidad de registros, y datos.-
	datos = new char[tamanio];
}

Bloque::~Bloque()
{
	delete [] datos;
}

void Bloque::organizarBloque(int offsetToReg,int longReg){
	
}
/*
 * Inserta un nuevo registro dentro del bloque
 * Retorna OK (0) si la inserción fue exitosa, o FAIL (1) en caso contrario
 **/
int Bloque::altaRegistro(char *registro){
	int offsetEspLibre;
	unsigned short longReg;
	char *auxEspLibre = new char [5]; //Reservo 4 bytes para obtener el espacio libre
	char *auxLongReg = new char[3];
	
	memcpy(auxLongReg,registro,2); //Obtengo la longitud del registro
	auxLongReg[3]= '\0';
	longReg = atoi(auxLongReg);
 	
	//Obtengo el offset al espacio libre
	memcpy(auxEspLibre,datos,4);
	//TODO: Ver lo del '\0'
	auxEspLibre[5]= '\0';
	offsetEspLibre = atoi(auxEspLibre);
	
	//Si el registro tiene espacio dentro del bloque realizo la inserción
	if(verificarEspacioDisponible(longReg,offsetEspLibre)){
		
		//Actualizo el offset al espacio libre, suponiendo que longReg incluye la longitud completa del registro
		//inclusive los 2 bytes que indican la longitud.
		offsetEspLibre = offsetEspLibre + longReg; 
		insertarRegistro(registro,offsetEspLibre);
		delete []auxEspLibre;
		delete []auxLongReg;
		return OK;	
	}
	delete []auxEspLibre;
	delete []auxLongReg;
	return FAIL;
}
	
/*
 * 
 **/
int Bloque::bajaRegistro(list <string>listaParam,void *clavePrimaria){
	bool registroBorrado = false;
	unsigned short longCampo;
	unsigned short longReg;
	int offsetToPk = 0;
	int offsetToRegDelete;
	//Se usa para levantar la longitud del atributo variable del registro
	char *longAtributo = new char [3];
	list <string>::iterator it;
	size_t posOfDelim;
	string regAtribute;
	string tipo;
	string pk;
	
	//Itero la lista de atributos del registro
	   for(it = listaParam.begin(); it != listaParam.end(); ++it){
		   regAtribute = *it;
		   posOfDelim = regAtribute.find(";");
		   //Obtengo el tipo de atributo del registro
		   tipo = regAtribute.substr(0,posOfDelim);
		   //Obtengo el indicador de clave primaria
		   pk = regAtribute.substr(posOfDelim+1);
		  
		   //Levanto el registro i
		   //TODO: Agregar más tipos si hace falta
		   
		   if(tipo == "string"){}
		   else if(tipo == "int"){
			   
		   }
		   else if(tipo == "shortInt"){}
		   else if(tipo == "double"){}
		   else if (tipo == "char"){}
		   
		   if(pk == "true"){
			   if(tipo == "string"){
				   
			   }
			   else if(tipo == "int")
			   //Obtengo el offset del registro dentro del bloque
			   
			   //Reorganizo el bloque pisando el registro a borrar
			   organizarBloque(offsetToRegDelete,longReg);
			   return OK;
		   }
		   /*else{
			   if(tipo == "string"){
				   //obtener la longitud del registro y sumarla al offset
				   longAtributo[0] = 
			   }
			   else if (tipo == "int")
				   offsetToPk += 4;
			   else if(tipo == "unsigned short")
				   offsetToPk += 2;
			   //TODO: agregar los tipos y tamaños que faltan
		  }*/
	   } 

	return OK;
}

int Bloque::modificarRegistro(list <string>listaParam,int longReg){
	
}

bool Bloque::verificarEspacioDisponible(int longReg,int offsetEspLibre){
	int espacioDisponible;
	
	espacioDisponible = tamanio - offsetEspLibre;
	if(espacioDisponible < longReg)
		return false;
	else
		return true;
}

void Bloque::insertarRegistro(char *registro, int nuevoOffsetEspLibre){}

//int Bloque::buscarRegistro(int offsetToPk,int longReg){};
