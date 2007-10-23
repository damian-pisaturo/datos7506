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
	int offsetToReg = 0;
	int i = 1 ;
	int j = 0;
	int longReg;
	int longCampo;
	int campoInt;
	string registro;
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
	string campo;
	string tipo;
	string pk;
	//Obtengo la cantidad de registros dentro del bloque
	cantRegs[0] = datos[2];
	cantRegs[1] = datos[3];
	cantRegs[2] = '\0';
	cantRegistros = atoi(cantRegs);
	
	//Mientras no itero sobre la cantidad total de registros o no borré el registro busco la clave primaria
	while( (i<cantRegistros + 1) || (!registroBorrado) ){
	//obtengo la longitud del registro;
	longRegistro[0] = datos [4 + offsetToReg];
	longRegistro[1] = datos [4 + offsetToReg + 1];
	longRegistro[2] = '\0';
	longReg = atoi(longRegistro);
	//La próxima iteración se levantará la longitud del registro siguiente
	offsetToReg = longReg;
	registro = getRegistro(longReg,offsetToReg);

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
		   		longAtributo[0] = registro[offsetToProxCampo];
		   		longAtributo[1] = registro[offsetToProxCampo + 1];
		   		longAtributo[2] = '\0';
		   		longCampo = atoi(longAtributo);
		   		
		   		campo = getRegisterAtribute(registro,offsetToProxCampo,longCampo);
		   		
		   		//Seteo el nuevo offset del próximo campo para la siguiente iteración
		   		offsetToProxCampo = longCampo;
		   }
		   else if(tipo == "int"){
			   for(j;j<4;j++)
			   	campo[j] = registro[offsetToProxCampo + j];
			  
			   campo[4] = '\0';
			   campoInt = atoi(campo.c_str());
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
			  // organizarBloque(offsetToRegDelete,longReg);
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

string Bloque::getRegistro(int longReg,int offsetToReg){
	string registro;
	int i = 2;
	//offsetToReg apunta al primer byte del registro, donde los dos primeros bytes
	//indican la longitud del mismo
	// Acá supongo que longReg tiene incluido sus dos bytes. 
	for(i;i<longReg;i++)
		// Los 4 corresponden al offset A espacio libre dentro del bloque y cantidad de registros
		// i arranca de 2, asi omito los 2 bytes que indican la longitud del mismo y retorno los datos directamente
		registro[i-2] = datos[4 + offsetToReg + i];
	
	return registro;
	
}

string Bloque::getRegisterAtribute(string registro,int offsetCampo,int longCampo){
	string campo;
	int i;
	
	for(i = offsetCampo;i<offsetCampo + longCampo;i++)
		//Corrimiento de 2, omitiendo el largo del campo
		campo[0] = registro[i + 2];
	return campo;
}
