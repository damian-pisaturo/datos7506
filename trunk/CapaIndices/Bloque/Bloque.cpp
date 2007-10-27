///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bucket.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Bucket.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Bloque.h"

///////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////

Bloque::Bloque()
{
	numero = 0;
	tamanio = 0;
	datos = NULL;
}

/**
 * Este constructor recibe el número de bloque dentro del archivo, y el tamaño del 
 * bloque medido en bytes.
 */
Bloque::Bloque(int num,int tam){
	numero = num;
	tamanio = tam;
	//contendrá offset a espacio libre, cantidad de registros, y datos.-
	datos = new char[tamanio];
}

Bloque::~Bloque(){
	if (datos)
		delete [] datos;
}

/**
 * Este metodo busca un registro en el bloque segun su clavePrimaria. Si lo encuentra
 * devuelve true y su offset dentro del bloque en "offsetReg"; y si no lo encuentra
 * devuelve false.
 */
bool Bloque::buscarRegistro(const list<string>& listaParam, void *clavePrimaria, unsigned short *offsetReg){

	int offsetToReg = getOffsetToRegs();
	int offsetToProxCampo = 0;

	//Obtengo la cantidad de registros dentro del bloque
	unsigned short cantRegistros;
	memcpy(&cantRegistros,&datos[2],Tamanios::TAMANIO_LONGITUD);
	
	bool encontrado = false;
		
	unsigned short longReg;
	char *registro;
	list<string>::const_iterator it;
	string regAtribute;
	size_t posOfDelim;
	string tipo;
	string pk;
	unsigned short longRegistro;
	unsigned short longCampo;
	char *campo;
	float campoNumerico;
	short int campoShort;

	int i = 1 ;
	//Mientras haya mas registros y no lo haya encontrado
	while( (i<cantRegistros + 1) && (!encontrado) ){
		
		//obtengo la longitud del registro;
		memcpy(&longRegistro,&datos[offsetToReg],Tamanios::TAMANIO_LONGITUD);
		
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
				   memcpy(&longCampo,&registro[offsetToProxCampo],Tamanios::TAMANIO_LONGITUD);
				   
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
					   					   
					   memcpy((int*)&campoNumerico,&registro[offsetToProxCampo],sizeof(int));
					 
					   if(campoNumerico == *((int*)clavePrimaria)){
						   *offsetReg = offsetToReg;
						   encontrado = true;
					   }
				   	}
				   offsetToProxCampo += sizeof(int);
			   }
			   else if(tipo == "shortInt"){
				   if(pk == "true"){
					 
					   memcpy(&campoShort,&registro[offsetToProxCampo],Tamanios::TAMANIO_LONGITUD);
					 	   
	   					if(campoShort == *((short int*)clavePrimaria)){
	   						*offsetReg = offsetToReg;
	   						encontrado = true;		   				    
	   					}  
				   }
				   offsetToProxCampo += sizeof(short int);
			   }
			   else if(tipo == "float"){
				   if(pk == "true"){
					   
					   memcpy(&campoNumerico, &registro[offsetToProxCampo], sizeof(float));
					   
	  				   if(campoNumerico == *((float*)clavePrimaria)){
	  					 *offsetReg = offsetToReg;
	  					 encontrado = true;	  					   
	  				   }
	   			   }
	   			   offsetToProxCampo += sizeof(float);
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
		delete []registro;
		}
		
		if (!encontrado)
			return false;
		return true;
}

/**
 * Este metodo reorganiza el bloque luego de una baja.
 */
void Bloque::organizarBloque(int offsetToReg,int longReg){
	
	char* datosAux = new char[tamanio];
	unsigned short espLibre;
	unsigned short cantRegs;
	
	//Levanta el espacio libre en el bloque.
	memcpy(&espLibre,datos,Tamanios::TAMANIO_LONGITUD);
	Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS
	// Levanta la cantidad de registros del bloque.
	memcpy(&cantRegs,&datos[2],Tamanios::TAMANIO_LONGITUD);
	
	// Hago una copia del bloque omitiendo el bloque borrado.
	memcpy(datosAux,datos,offsetToReg);
	memcpy(&datosAux[offsetToReg],&datos[offsetToReg+longReg],tamanio-offsetToReg-longReg);
	
	//Actualizo los valores de espacio libre y cantidad de registros.
	espLibre += longReg;
	cantRegs -= 1;
	memcpy(datosAux,&espLibre,Tamanios::TAMANIO_LONGITUD);
	memcpy(&datosAux[2],&cantRegs,Tamanios::TAMANIO_LONGITUD);
	
	//Actualizo datos.
	delete[] datos;
	datos = datosAux;	
}

/*
 * Inserta un nuevo registro dentro del bloque
 * Retorna true si la inserción fue exitosa, o false en caso contrario
 **/
bool Bloque::altaRegistro(char *registro){
	unsigned short offsetEspLibre;
	unsigned short longReg;
	
	//Obtengo la longitud del registro
	memcpy(&longReg, registro, Tamanios::TAMANIO_LONGITUD);
 	
	//Obtengo el offset al espacio libre
	memcpy(&offsetEspLibre,&datos[0],Tamanios::TAMANIO_LONGITUD);
	
	//Si el registro tiene espacio dentro del bloque realizo la inserción
	if(verificarEspacioDisponible(longReg,offsetEspLibre)){
		// registro incluye su longitud en los primeros 2 bytes.
		insertarRegistro(registro,offsetEspLibre,longReg); 
		return true;	
	}
	return false;
}
	
/*
 * Da de baja dentro del bloque al registro cuya clave es clavePrimaria.
 **/
int Bloque::bajaRegistro(list <string>listaParam,void *clavePrimaria){

	unsigned short offsetToReg = getOffsetToRegs();
	unsigned short offsetToProxCampo = 0;

	unsigned short longReg;
	unsigned short cantRegistros;
	bool registroBorrado = false;
	char *registro;
	list <string>::iterator it;
	string regAtribute;
	size_t posOfDelim;
	string tipo;
	string pk;
	unsigned short longCampo;
	char *campo;
	float campoNumerico;
	unsigned short campoShort;
	
	//Obtengo la cantidad de registros dentro del bloque
	memcpy(&cantRegistros,&datos[2],Tamanios::TAMANIO_LONGITUD);
	
	list <>int i = 1 ;
	//Mientras no itero sobre la cantidad total de registros y no borré el registro busco la clave primaria
	while( (i<cantRegistros + 1) && (!registroBorrado) ){
	
		//obtengo la longitud del registro;
		memcpy(&longReg,&datos[offsetToReg],Tamanios::TAMANIO_LONGITUD);
		registro = new char[longReg];
		
		//Omito la longitud del registro
		offsetToReg += Tamanios::TAMANIO_LONGITUD;
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
				   memcpy(&longCampo,&registro[offsetToProxCampo],Tamanios::TAMANIO_LONGITUD);
				   offsetToProxCampo += Tamanios::TAMANIO_LONGITUD;
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
			   		//Seteo el nuevo offse//	char *campo;t del próximo campo para la siguiente iteración
			   		offsetToProxCampo += longCampo;
			   }
			   else if(tipo == "int"){
				   
				   if(pk == "true"){
					   
					   memcpy((int*)&campoNumerico,&registro[offsetToProxCampo],sizeof(int));
					 
					   if(campoNumerico == *((int*)clavePrimaria)){
						   organizarBloque(offsetToReg,longReg);
						   registroBorrado = true;
					   }
				   	}
				   offsetToProxCampo += sizeof(int);
			   }
			   else if(tipo == "shortInt"){
				   if(pk == "true"){
					   memcpy(&campoShort,&registro[offsetToProxCampo],sizeof(short int));
					   if(campoNumerico == *((short int*)clavePrimaria)){
	   						organizarBloque(offsetToReg,longReg);
	   						registroBorrado = true;
	   				    }
				   }
				   offsetToProxCampo += sizeof(short int);
			   }
			   else if(tipo == "float"){
				   if(pk == "true"){
					   memcpy(&campoNumerico,&registro[offsetToProxCampo],sizeof(float));
					   if(campoNumerico == *((float*)clavePrimaria)){
  						   organizarBloque(offsetToReg,longReg);
  						   registroBorrado = true;
  					   }
   				   }
   				   offsetToProxCampo += sizeof(float);
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
		delete []registro;
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
	
	// Levanto la longitud del registro original.
	unsigned short longRegOrig;	
	memcpy(&longRegOrig,&datos[offsetReg-2],Tamanios::TAMANIO_LONGITUD);
	
	unsigned short offsetEspLibre;
	memcpy(&offsetEspLibre,datos,Tamanios::TAMANIO_ESPACIO_LIBRE);
	
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
	// Se hace longReg + TAMANIO_LONGITUD porque esa longitud no incluye los 2 bytes que se usan para
	// guardar la longitud del registro.
	memcpy(&datos[offsetEspLibre],registro,longReg + Tamanios::TAMANIO_LONGITUD);
	
	// Actualizo el offset al espacio libre.
	offsetEspLibre += (longReg + Tamanios::TAMANIO_LONGITUD);
	memcpy(datos,&offsetEspLibre,Tamanios::TAMANIO_ESPACIO_LIBRE);
	
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

/**
 * Retorna el offset al primer registro del bloque.
 */
unsigned short Bloque::getOffsetToRegs()
{
	return (Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS);
}
