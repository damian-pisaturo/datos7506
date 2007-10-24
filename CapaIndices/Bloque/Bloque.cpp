#include "Bloque.h"

/**
 * Este metodo busca un registro en el bloque segun su clavePrimaria. Si lo encuentra
 * devuelve true y su offset dentro del bloque en "offsetReg"; y si no lo encuentra
 * devuelve false.
 */
bool Bloque::buscarRegistro(const list<string>& listaParam, void *clavePrimaria, unsigned short *offsetReg){
	int offsetToReg = 4;
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
	list<string>::const_iterator it;
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
		
	bool encontrado = false;
		
	int i = 1 ;
	//Mientras haya mas registros y no lo haya encontrado
	while( (i<cantRegistros + 1) && (!encontrado) ){
		
		//obtengo la longitud del registro;
		longRegistro = getSubstring(2,offsetToReg);
		longReg = atoi(longRegistro);
		registro = new char[longReg];
		//Omito la longitud del registro
		offsetToReg += 2;
		//Obtengo el registro (sin incluir la longitud del mismo)
		registro = getRegistro(longReg,offsetToReg);
	
		//Itero la lista de atributos del registro
		for(it = listaParam.begin(); it != listaParam.end(); ++it){
			   regAtribute = *it;
			   posOfDelim = regAtribute.find(";");
			   //Obtengo el tipo de atributo del registro
			   tipo = regAtribute.substr(0,posOfDelim);
			   //Obtengo el indicador de clave primaria
			   pk = regAtribute.substr(posOfDelim+1);
			   
			   if(tipo == "string"){
				   //obtengo la longitud del campo variable
				   memcpy(longAtributo,&registro[offsetToProxCampo],2);
				   longAtributo[2] = '\0';
				   longCampo = atoi(longAtributo);
				   offsetToProxCampo += 2;
				   //Si es pk, me preocupo por obtener el campo en si
				   if(pk == "true"){
					   campo = getRegisterAtribute(registro,offsetToProxCampo,longCampo);
			   		   if(strcmp((char*)clavePrimaria,campo) == 0)
			   			{
			   				*offsetReg = offsetToReg;
			   				encontrado = true;		
			   			}
			   		delete []campo;
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
					 
					   if(campoNumerico == *((int*)clavePrimaria)){
						   *offsetReg = offsetToReg;
						   encontrado = true;
					   }
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
	  					   
	   					if(campoNumerico == *((short int*)clavePrimaria)){
	   						*offsetReg = offsetToReg;
	   						encontrado = true;		   				    
	   					}
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
	  				   if(campoNumerico == *((float*)clavePrimaria)){
	  					 *offsetReg = offsetToReg;
	  					 encontrado = true;	  					   
	  				   }
	  				   delete[]campo;
	   			   }
	   			   offsetToProxCampo += 4;
			   }
			   else if (tipo == "char"){
				   if(pk == "true"){
					   campo = new char[1];
					   campo[0] = registro[offsetToProxCampo];
					   if(campo[0] == ((char*)clavePrimaria)[0]){
						   *offsetReg = offsetToReg;
						   encontrado = true;
					   }
					   delete[]campo;   
				   }
			   }
			   else if(tipo == "fecha"){
				   //TODO: Completar la fecha
			   }
			   
		   } 
		//La próxima iteración se levantará la longitud del registro siguiente
		offsetToReg += longReg;
		}
		if (!encontrado)
			return false;
		return true;
}

Bloque::Bloque(int num,int tam){
	numero = num;
	tamanio = tam;
	//contendrá offset a espacio libre, cantidad de registros, y datos.-
	datos = new char[tamanio];
}

Bloque::~Bloque(){
	delete [] datos;
}

/**
 * Este metodo levanta una longitud de caracteres a partir de datos[offset]
 */
char* Bloque::getSubstring(int longitud, int offset){
	char* cadena = new char[longitud+1];
	memcpy(cadena,&datos[offset],longitud);
	cadena[longitud] = '\0';
	
	return cadena;
}

/**
 * Este metodo reorganiza el bloque luego de una baja.
 */
void Bloque::organizarBloque(int offsetToReg,int longReg){
	
	char* datosAux = new char[tamanio];
	char * espLibreAux;
	char* cantRegsAux;
	short espLibre;
	short cantRegs;
	
	//Levanta el espacio libre en el bloque.
	espLibreAux = getSubstring(2,0);
	espLibre = atoi(espLibreAux);
	
	// Levanta la cantidad de registros del bloque.
	cantRegsAux = getSubstring(2,2); 
	cantRegs = atoi(cantRegsAux);
	
	// Hago una copia del bloque omitiendo el bloque borrado.
	memcpy(datosAux,datos,offsetToReg);
	memcpy(&datosAux[offsetToReg],&datos[offsetToReg+longReg],tamanio-offsetToReg-longReg);
	
	//Actualizo los valores de espacio libre y cantidad de registros.
	espLibre += longReg;
	cantRegs -= 1;
	memcpy(datosAux,&espLibre,2);
	memcpy(&datosAux[2],&cantRegs,2);
	
	//Actualizo datos.
	delete[] datos;
	datos = datosAux;
	
	delete[] espLibreAux;
	delete[] cantRegsAux;	
}

/*
 * Inserta un nuevo registro dentro del bloque
 * Retorna true si la inserción fue exitosa, o false en caso contrario
 **/
bool Bloque::altaRegistro(char *registro){
	unsigned short offsetEspLibre;
	unsigned short longReg;
	char *auxEspLibre; 
	char *auxLongReg = new char[3];
	
	//Obtengo la longitud del registro
	memcpy(auxLongReg,registro,2); 
	auxLongReg[2]= '\0';
	longReg = atoi(auxLongReg);
 	
	//Obtengo el offset al espacio libre
	auxEspLibre = getSubstring(2,0);
	offsetEspLibre = atoi(auxEspLibre);
	
	//Si el registro tiene espacio dentro del bloque realizo la inserción
	if(verificarEspacioDisponible(longReg,offsetEspLibre)){
		insertarRegistro(registro,offsetEspLibre,longReg); 
		return true;	
	}
	delete []auxEspLibre;
	delete []auxLongReg;
	return false;
}
	
/*
 * 
 **/
int Bloque::bajaRegistro(list <string>listaParam,void *clavePrimaria){
	bool registroBorrado = false;
	int offsetToReg = 4;
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
	
	int i = 1 ;
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
		
	
		//Itero la lista de atributos del registro
		   for(it = listaParam.begin(); it != listaParam.end(); ++it){
			   regAtribute = *it;
			   posOfDelim = regAtribute.find(";");
			   //Obtengo el tipo de atributo del registro
			   tipo = regAtribute.substr(0,posOfDelim);
			   //Obtengo el indicador de clave primaria
			   pk = regAtribute.substr(posOfDelim+1);
			   
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
			   		
			   			if(strcmp((char*)clavePrimaria,campo) == 0)
			   			{
			   				organizarBloque(offsetToReg,longReg);
			   				registroBorrado = true;		
			   			}
			   			delete[]campo;
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
					 
					   if(campoNumerico == *((int*)clavePrimaria)){
						   organizarBloque(offsetToReg,longReg);
						   registroBorrado = true;
					   }
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
   					   
	   					if(campoNumerico == *((short int*)clavePrimaria)){
	   						organizarBloque(offsetToReg,longReg);
	   						registroBorrado = true;
	   				    }
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
  					   if(campoNumerico == *((float*)clavePrimaria)){
  						   organizarBloque(offsetToReg,longReg);
  						   registroBorrado = true;
  					   }
  					   delete[]campo;
   				   }
   				   offsetToProxCampo += 4;
			   }
			   else if (tipo == "char"){
				   if(pk == "true"){
					   campo = new char[1];
					   campo[0] = registro[offsetToProxCampo];
					   if(campo[0] == ((char*)clavePrimaria)[0]){
						   organizarBloque(offsetToReg,longReg);
						   registroBorrado = true;
					   }
					   delete[]campo;
					   
				   }
			   }
			   else if(tipo == "fecha"){
				   //TODO: Completar la fecha
			   }
			   
		   } 
		//La próxima iteración se levantará la longitud del registro siguiente
		offsetToReg += longReg;
	}
	if (!registroBorrado)
		return false;
	return true;
}

int Bloque::modificarRegistro(const list<string>& listaParam, unsigned short longReg, char* registro){

	void* clavePrimaria;
	//TODO: buscar clave primaria.
	unsigned short offsetReg;
	bool encontrado = buscarRegistro(listaParam, clavePrimaria, &offsetReg);
	
	if (!encontrado)
		return NO_ENCONTRADO;
	
	unsigned short longRegOrig;
	// Levanto la longitud del registro original.
	memcpy(&longRegOrig,&datos[offsetReg-2],2);
	
	unsigned short offsetEspLibre;
	memcpy(&offsetEspLibre,datos,2);
	
	if ((tamanio-offsetEspLibre + longRegOrig)<longReg )
		return OVERFLOW;
	bajaRegistro(listaParam,clavePrimaria);
	altaRegistro(registro);
	return OK;
}

bool Bloque::verificarEspacioDisponible(int longReg,int offsetEspLibre){
	int espacioDisponible;
	
	espacioDisponible = tamanio - offsetEspLibre;
	if(espacioDisponible < longReg)
		return false;
	else
		return true;
}

void Bloque::insertarRegistro(char *registro, unsigned short offsetEspLibre,unsigned short longReg){
	// Se hace longReg + 2 porque esa longitud no incluye los 2 bytes que se usan para guardar la longitud
	// del registro.
	memcpy(&datos[offsetEspLibre],registro,longReg + 2);
	
	// Actualizo el offset al espacio libre.
	offsetEspLibre += (longReg + 2);
	memcpy(datos,&offsetEspLibre,2);
	
}

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
