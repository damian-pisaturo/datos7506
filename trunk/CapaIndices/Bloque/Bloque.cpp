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
	int offsetToReg = 4;
	int i = 1 ;
	//int j = 0;
	int longReg;
	int longCampo;
	float campoNumerico;
	char *registro;
	int offsetToProxCampo = 0;
	//Se usa para levantar la longitud del atributo variable del registro
	char *longAtributo = new char [3];
	//Se usa para levantar la longitud del registro
	char *longRegistro = new char [3];
	//Cantidad de registros dentro del bloque
	char *cantRegs = new char[3];
	int cantRegistros;
	list <string>::iterator it;
	string regAtribute;
	size_t posOfDelim;
	char *campo;
	string tipo;
	string pk;
	//Obtengo la cantidad de registros dentro del bloque
	cantRegs[0] = datos[2];
	cantRegs[1] = datos[3];
	cantRegs[2] = '\0';
	cantRegistros = atoi(cantRegs);
	
	//Mientras no itero sobre la cantidad total de registros y no borré el registro busco la clave primaria
	while( (i<cantRegistros + 1) && (!registroBorrado) ){
	
		//obtengo la longitud del registro;
		longRegistro[0] = datos [offsetToReg];
		longRegistro[1] = datos [offsetToReg + 1];
		longRegistro[2] = '\0';
		longReg = atoi(longRegistro);
		registro = new char[longReg];
		//Omito la longitud del registro
		offsetToReg += 2;
		registro = getRegistro(longReg,offsetToReg);
		
		//La próxima iteración se levantará la longitud del registro siguiente
		offsetToReg += longReg;
	
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
			   
			   if(tipo == "string"){
			   		//obtengo la longitud del campo variable
				   	longAtributo[0] = registro[offsetToProxCampo];
			   		longAtributo[1] = registro[offsetToProxCampo + 1];
			   		longAtributo[2] = '\0';
			   		longCampo = atoi(longAtributo);
			   		offsetToProxCampo += 2;
			   		//Si es pk, me preocupo por obtener el campo en si
			   		if(pk == "true"){
			   			campo = getRegisterAtribute(registro,offsetToProxCampo,longCampo);
			   			//TODO: casteo la clave a string, y comparo si es el reg buscado
			   			//si es, llamo a organizar bloque
			   			//registroBorrado=true
			   		}
			   		
			   		//Seteo el nuevo offset del próximo campo para la siguiente iteración
			   		offsetToProxCampo += longCampo;
			   }
			   else if(tipo == "int"){
				   
				   if(pk == "true"){
					   campo = new char[5];
					   
					   memcpy(campo,&registro[offsetToProxCampo],4);
					   campo[4] = '\0';		   		
					   campoNumerico = atoi(campo);
					   //TODO: comparo si es el reg buscado
					   //si es, llamo a organizar bloque
					   //registroBorrado=true
					   delete[]campo;
				   	}
				   offsetToProxCampo += 4;
			   }
			   else if(tipo == "shortInt"){
				   if(pk == "true"){
					   campo = new char[3];
					   memcpy(campo,&registro[offsetToProxCampo],2);
   					   campo[2] = '\0';		   		
   					   campoNumerico = atoi(campo);
   					   //TODO: comparo si es el reg buscado
   					   //si es, llamo a organizar bloque
   					   //registroBorrado=true
   					   delete[]campo;
					   
				   }
				   offsetToProxCampo += 2;
				   
			   }
			   else if(tipo == "float"){
				   if(pk == "true"){
					   campo = new char[5];
   					   memcpy(campo,&registro[offsetToProxCampo],4);
  					   campo[4] = '\0';		   		
  					   campoNumerico = atof(campo);
  					   //TODO: comparo si es el reg buscado
  					   //si es, llamo a organizar bloque
  					   //registroBorrado=true
  					   delete[]campo;
				   					   
   				   }
   				   offsetToProxCampo += 4;
				   
			   }
			   else if (tipo == "char"){
				   if(pk == "true"){
					   campo = new char[1];
					   campo[0] = registro[offsetToProxCampo];
					   //TODO: comparo si es el reg buscado
 					   //si es, llamo a organizar bloque
 					   //registroBorrado=true
					   delete[]campo;
					   
				   }
			   }
			   
		   } 
	}
	return OK;
}

int Bloque::modificarRegistro(list <string>listaParam,int longReg){

	return 0;
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

char* Bloque::getRegistro(int longReg,int offsetToReg){
	char *registro = new char [longReg];
	
	memcpy(registro,&datos[offsetToReg],longReg);
	return registro;
	
}

char* Bloque::getRegisterAtribute(string registro,int offsetCampo,int longCampo){
	char *campo = new char[longCampo + 1];
	
	memcpy(campo,&registro[offsetCampo],longCampo);
	campo[longCampo] = '\0';
	return campo;
}
