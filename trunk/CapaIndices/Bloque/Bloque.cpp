///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bloque.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase Bloque.
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

/**
 * Este constructor recibe el número de bloque dentro del archivo, y el tamaño del 
 * bloque medido en bytes.
 */
Bloque::Bloque(unsigned int num,unsigned int tam){
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

bool Bloque::buscarRegistro(const list<nodoLista>& listaParam, void *clavePrimaria, unsigned short *offsetReg){
	
	int offsetToReg = getOffsetToRegs();
	int offsetToProxCampo = 0;

	// Se obtiene la cantidad de registros dentro del bloque.
	unsigned short cantRegistros;
	memcpy(&cantRegistros,&datos[2],Tamanios::TAMANIO_LONGITUD);
	
	bool encontrado = false;
	char *registro;
	list<nodoLista>::const_iterator it;
	nodoLista regAtribute;
	int tipo;
	string pk;
	unsigned short longRegistro;
	unsigned short longCampo;
	char *campo;
	float campoNumerico;
	short int campoShort;

	int i = 1 ;
	it = listaParam.begin();
	regAtribute = *it;
	
	// Se obtiene el tipo de atributo del registro.
	tipo = regAtribute.tipo;
	// Mientras haya mas registros y no lo haya encontrado.
	while( (i<cantRegistros + 1) && (!encontrado) ){
		
		
		if(tipo == TipoDatos::TIPO_VARIABLE){
			//Se obtiene la longitud del registro.
			memcpy(&longRegistro,&datos[offsetToReg],Tamanios::TAMANIO_LONGITUD);
			
			// Se omite la longitud del registro.
			offsetToReg += 2;
		}
		else {
			// Se obtiene la longitud del registro.
			//memcpy(&longRegistro,&regAtribute[posOfDelim + 1],Tamanios::TAMANIO_LONGITUD);
			memcpy(&longRegistro,&regAtribute.pk[0],Tamanios::TAMANIO_LONGITUD);
			offsetToReg += longRegistro;
		}
		
		// Se obtiene el registro (sin incluir la longitud del mismo)
		registro = getRegistro(longRegistro,offsetToReg);
	
		// Se itera la lista de atributos del registro.
		// Se arranco del segundo nodo ya que el primero se utiliza para guardar
		// si el registro es de longitud fija o variable.
		for(it = (++listaParam.begin()); it != listaParam.end(); ++it){
			   regAtribute = *it;
	
			   // Se obtiene el tipo de atributo del registro.
			   tipo = regAtribute.tipo;
			   
			   // Se obtiene el indicador de clave primaria.
			   pk = regAtribute.pk;
			   
			   if(tipo == TipoDatos::TIPO_STRING){
				   // Se obtiene la longitud del campo variable.
				   memcpy(&longCampo,&registro[offsetToProxCampo],Tamanios::TAMANIO_LONGITUD);
				   
				   offsetToProxCampo += 2;
				   
				   // Si es pk, se obtiene el campo en sí.
				   if(pk == "true"){
					   campo = getRegisterAtribute(registro,offsetToProxCampo,longCampo);
			   		   if(strcmp((char*)clavePrimaria,campo) == 0)
			   			{
			   				*offsetReg = offsetToReg;
			   				encontrado = true;		
			   			}
			   		delete []campo;
			   		}
			   		// Se setea el nuevo offset del próximo campo para la siguiente iteración.
			   		offsetToProxCampo += longCampo;
			   }
			   else if(tipo == TipoDatos::TIPO_ENTERO){
				   
				   if(pk == "true"){
					   					   
					   memcpy((int*)&campoNumerico,&registro[offsetToProxCampo],sizeof(int));
					 
					   if(campoNumerico == *((int*)clavePrimaria)){
						   *offsetReg = offsetToReg;
						   encontrado = true;
					   }
				   	}
				   offsetToProxCampo += sizeof(int);
			   }
			   else if(tipo == TipoDatos::TIPO_SHORT){
				   if(pk == "true"){
					 
					   memcpy(&campoShort,&registro[offsetToProxCampo],Tamanios::TAMANIO_LONGITUD);
					 	   
	   					if(campoShort == *((short int*)clavePrimaria)){
	   						*offsetReg = offsetToReg;
	   						encontrado = true;		   				    
	   					}  
				   }
				   offsetToProxCampo += sizeof(short int);
			   }
			   else if(tipo == TipoDatos::TIPO_FLOAT){
				   if(pk == "true"){
					   
					   memcpy(&campoNumerico, &registro[offsetToProxCampo], sizeof(float));
					   
	  				   if(campoNumerico == *((float*)clavePrimaria)){
	  					 *offsetReg = offsetToReg;
	  					 encontrado = true;	  					   
	  				   }
	   			   }
	   			   offsetToProxCampo += sizeof(float);
			   }
			   else if (tipo == TipoDatos::TIPO_CHAR){
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
			   else if(tipo == TipoDatos::TIPO_FECHA){
				   //TODO: Completar la fecha
			   }
			   
		   } 
		//La próxima iteración se levantará la longitud del registro siguiente
		offsetToReg += longRegistro;
		delete []registro;
		}
		
		if (!encontrado)
			return false;
		return true;
}

/**
 * Este metodo reorganiza el bloque luego de una baja.
 * longReg incluye los bytes utilizados para la longitud del registro en caso
 * de que el mismo sea variable.
 */
void Bloque::organizarBloque(int offsetToReg,int longReg){
	
	char* datosAux = new char[tamanio];
	unsigned short espLibre;
	unsigned short cantRegs;
	
	// Levanta el espacio libre en el bloque.
	memcpy(&espLibre,datos,Tamanios::TAMANIO_LONGITUD);
	
	// Levanta la cantidad de registros del bloque.
	memcpy(&cantRegs,&datos[Tamanios::TAMANIO_LONGITUD],Tamanios::TAMANIO_LONGITUD);
	
	// Se hace una copia del bloque omitiendo el bloque borrado.
	memcpy(datosAux,datos,offsetToReg);
	memcpy(&datosAux[offsetToReg],&datos[offsetToReg+longReg],tamanio-offsetToReg-longReg);
	
	// Se actualizan los valores de espacio libre y cantidad de registros.
	// Se resta la longitud del registro porque espLibre contiene el offset al espacio libre
	// y no el espacio libre en sí.
	espLibre -= longReg;
	cantRegs -= 1;
	memcpy(datosAux,&espLibre,Tamanios::TAMANIO_LONGITUD);
	memcpy(&datosAux[Tamanios::TAMANIO_LONGITUD],&cantRegs,Tamanios::TAMANIO_LONGITUD);
	
	// Se actualizan los datos.
	delete[] datos;
	datos = datosAux;	
}

/*
 * Inserta un nuevo registro dentro del bloque
 * Retorna true si la inserción fue exitosa, o false en caso contrario
 **/
bool Bloque::altaRegistro(const list<nodoLista>& listaParam,char *registro){
	unsigned short offsetEspLibre;
	unsigned short longReg;
	
	// Se obtiene la longitud del registro.
	longReg = getTamanioRegistros(listaParam,registro);
 	
	// Se obtiene el offset al espacio libre.
	memcpy(&offsetEspLibre,&datos[0],Tamanios::TAMANIO_LONGITUD);
	
	//Si el registro tiene espacio dentro del bloque se realiza la inserción.
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
int Bloque::bajaRegistro(const list <nodoLista>& listaParam,void *clavePrimaria){

	unsigned short offsetToReg = getOffsetToRegs();
	unsigned short offsetToProxCampo = 0;

	unsigned short longReg;
	unsigned short cantRegistros;
	bool registroBorrado = false;
	char *registro;
	list<nodoLista>::const_iterator it = listaParam.begin();
	nodoLista regAtribute;
	int tipo;
	string pk;
	unsigned short longCampo;
	char *campo;
	float campoNumerico;
	unsigned short campoShort;
	
	// Se obtiene la cantidad de registros dentro del bloque.
	memcpy(&cantRegistros,&datos[Tamanios::TAMANIO_ESPACIO_LIBRE],Tamanios::TAMANIO_CANTIDAD_REGISTROS);
	
	// Se obtiene la longitud del registrstringo.
	longReg = getTamanioRegistros(listaParam,&datos[offsetToReg]);
	
	
	regAtribute = *it;
	// Se obtiene el tipo de atributo del registro.
	tipo = regAtribute.tipo;
	
	int i = 1 ;
	// Mientras no se borre al registro y haya más registros..
	while( (i<cantRegistros + 1) && (!registroBorrado) ){
	
		// Se obtiene la longitud del registro.
		longReg = getTamanioRegistros(listaParam,&datos[offsetToReg]);
		registro = new char[longReg];
		
		if (tipo == TipoDatos::TIPO_VARIABLE)
				// Se omite la longitud del registro.
				offsetToReg += Tamanios::TAMANIO_LONGITUD;
		
		// Se obtiene el registro.
		registro = getRegistro(longReg,offsetToReg);
	
		//Itero la lista de atributos del registro
		for(++it ; it != listaParam.end(); ++it){
		   regAtribute = *it;

		   //Obtengo el tipo de atributo del registro
		   tipo = regAtribute.tipo;
		   //Obtengo el indicador de clave primaria
		   pk = regAtribute.pk;
		   
		   if(tipo == TipoDatos::TIPO_STRING){
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
		   		//Seteo el nuevo offset del próximo campo para la siguiente iteración
		   		offsetToProxCampo += longCampo;
		   }
		   else if(tipo == TipoDatos::TIPO_ENTERO){
			   
			   if(pk == "true"){
				   
				   memcpy((int*)&campoNumerico,&registro[offsetToProxCampo],sizeof(int));
				 
				   if(campoNumerico == *((int*)clavePrimaria)){
					   organizarBloque(offsetToReg,longReg);
					   registroBorrado = true;
				   }
			   	}
			   offsetToProxCampo += sizeof(int);
		   }
		   else if(tipo == TipoDatos::TIPO_SHORT){
			   if(pk == "true"){
				   memcpy(&campoShort,&registro[offsetToProxCampo],sizeof(short int));
				   if(campoNumerico == *((short int*)clavePrimaria)){
   						organizarBloque(offsetToReg,longReg);
   						registroBorrado = true;
   				    }
			   }
			   offsetToProxCampo += sizeof(short int);
		   }
		   else if(tipo == TipoDatos::TIPO_FLOAT){
			   if(pk == "true"){
				   memcpy(&campoNumerico,&registro[offsetToProxCampo],sizeof(float));
				   if(campoNumerico == *((float*)clavePrimaria)){
					   organizarBloque(offsetToReg,longReg);
					   registroBorrado = true;
				   }
			   }
			   offsetToProxCampo += sizeof(float);
		   }
		   else if (tipo == TipoDatos::TIPO_CHAR){
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
		   else if(tipo == TipoDatos::TIPO_FECHA){
			   //TODO: Completar la fecha
		   }
	   } 
	   //La próxima iteración se levantará la longitud del registro siguiente
	   offsetToReg += longReg;
	   delete []registro;
	}
	if (!registroBorrado)
		return NO_ENCONTRADO;
	return OK;
}

int Bloque::modificarRegistro(const list<nodoLista>& listaParam, unsigned short longReg, void* clavePrimaria, char* registro){

	unsigned short offsetReg;
	bool encontrado = buscarRegistro(listaParam, clavePrimaria, &offsetReg);
	
	if (!encontrado)
		return NO_ENCONTRADO;
	
	// Levanta la longitud del registro original.
	unsigned short longRegOrig;	
	longRegOrig = getTamanioRegistros(listaParam,&datos[offsetReg]);
	memcpy(&longRegOrig,&datos[offsetReg-2],Tamanios::TAMANIO_LONGITUD);
	
	unsigned short offsetEspLibre;
	memcpy(&offsetEspLibre,datos,Tamanios::TAMANIO_ESPACIO_LIBRE);
	
	if ((getTamanioBloque()-offsetEspLibre + longRegOrig)<longReg )
		return OVERFLOW;
	bajaRegistro(listaParam,clavePrimaria);
	altaRegistro(listaParam,registro);
	return OK;
}

bool Bloque::verificarEspacioDisponible(int longReg,int offsetEspLibre){
	int espacioDisponible;
	
	espacioDisponible = getTamanioBloque() - offsetEspLibre;
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

/*
 * Retorna el offset al primer registro del bloque.
 **/
unsigned short Bloque::getOffsetToRegs()
{
	return (Tamanios::TAMANIO_ESPACIO_LIBRE + Tamanios::TAMANIO_CANTIDAD_REGISTROS);
}

/*
 * Devuelve la longitud del registro, sin incluir los 2 bytes de la longitud.
 **/
unsigned short Bloque::getTamanioRegistros(const list<nodoLista>& listaParam,char *registro)
{
	list<nodoLista>::const_iterator it = listaParam.begin();
	nodoLista regAtribute = *it;

	
	// Se obtiene el tipo de atributo del registro.
	int tipo = regAtribute.tipo;
	unsigned short longReg;

	if(tipo == TipoDatos::TIPO_VARIABLE)
		// Se obtiene la longitud del registro variable.
		memcpy(&longReg,registro,Tamanios::TAMANIO_LONGITUD);
	else 
		// Se obtiene la longitud del registro fijo.
		memcpy(&longReg,&regAtribute.pk[0],Tamanios::TAMANIO_LONGITUD);
	
	return longReg;
}

unsigned int Bloque::getTamanioBloque()
{
	return tamanio;
}

void Bloque::setTamanioBloque(unsigned int tam)
{
	tamanio = tam;
}

char * Bloque::getDatos()
{
	return datos;
}

void Bloque::setDatos(char* d)
{
	datos = d;
}
	
unsigned int Bloque::getNroBloque()
{
	return numero;
}

void Bloque::setNroBloque(unsigned int num)
{
	numero = num;
}
	
	
	
