#include "CapaIndices.h"

void crearIndice(const string &nombreTipo, MapaIndices &mapaIndices,
				const DefinitionsManager::NodoListaIndices &nodo,
				DefinitionsManager &defManager) {
	
	Indice* indice;
	DefinitionsManager::ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	DefinitionsManager::EstructTipoIndice estructura = nodo.estructTipoIndice;
	
	
	switch (estructura.tipoEstructura) {
	
		case TipoIndices::ARBOL_BP:
		case TipoIndices::ARBOL_BS:	
			indice = new IndiceArbol(estructura.tipoIndice, estructura.tamanioBloque, estructura.tipoClave,
									defManager.getListaTipos(nombreTipo), estructura.tipoEstructura, estructura.tamanioBloque,
									estructura.nombreArchivo);
			break;
			
		case TipoIndices::HASH:
			indice = new IndiceHash(*listaTiposAtributos,estructura.tamanioBloque,estructura.nombreArchivo);
	}	
	mapaIndices[*(nodo.listaNombresClaves)] = indice;
}



int main(int argc, char* argv[]) {
	
	ComuDatos pipe(argv);
	
	unsigned char codOp;
	string nombreTipo, buffer;
	unsigned int tamanioBuffer;
	
	pipe.parametro(0, codOp);
	pipe.parametro(1, nombreTipo);
	
	//Leo el tamanio del buffer a recibir
	pipe.leer(&tamanioBuffer);
	
	char *bufferPipe = new char[tamanioBuffer];
	
	//Leo los datos provenientes de la capa de metadata
	pipe.leer(tamanioBuffer, bufferPipe);
	
	buffer = bufferPipe;
	
	string::size_type posAnterior = 0, posActual = 0;
	DefinitionsManager::ListaValoresClaves listaValoresClaves;
	
	while (posAnterior != string::npos) {
		posActual = buffer.find(COD_FIN_CLAVE);
		if (posActual != string::npos)
			listaValoresClaves.push_back(buffer.substr(posAnterior, posActual - posAnterior));
		posAnterior = posActual;
	}
	
	//falta terminar de procesar el bloque de datos
	
	delete[] bufferPipe;
}
 




/*
 * Este codigo es para probar ABM con registros variables.
 * Cabe aclarar que bloque no es el encargado de chequear las claves repetidas, esa funcionalidad 
 * esta en la clase bucket.
 **/
/********************************************************************************************************/
/*	int entero1 = 25;
char* cadena = new char[5];
strcpy(cadena, "HOLA");
unsigned short longCadena = 4;
int entero2 = 55;

unsigned short tamReg = 14; // 4 + 2 + 4 + 4
unsigned short tamBloque = 512;
char *registro = new char [tamReg + 3];

RegisterInfo * listaLoca = new RegisterInfo();
Bloque * bloque =  new Bloque(0, tamBloque);

// Llena el registro.
memcpy(registro,&tamReg,Tamanios::TAMANIO_LONGITUD);
memcpy(registro + 2,&entero1,sizeof(int));
memcpy(registro + 6,&longCadena,Tamanios::TAMANIO_LONGITUD);
memcpy(registro + 8,cadena,longCadena);
memcpy(registro + 12,&entero2,sizeof(int));	

int result = bloque->altaRegistro(listaLoca->getParameterList(),registro);

cout<<"Alta"<<result<<endl;

const char* datos = bloque->getDatos();
	
unsigned short eLibre = 0;
unsigned short cantRegs = 0;

eLibre = *((unsigned short*)datos);
cantRegs = *((unsigned short*)(datos + 2));
entero1 = *((int*)(datos + 6));

memcpy(cadena, datos + 12, 4);
*(cadena + 4) = 0;
 
entero2 = *((int*)(datos + 16));	

cout<<"esp libre: "<<eLibre<<endl;
cout<<"cant regs: "<<cantRegs<<endl;
cout<<"campo1: "<<entero1<<endl;
cout<<"campo2: "<<cadena<<endl;
cout<<"campo3: "<<entero2<<endl;


void** clave = new void*[1];
clave[0] = cadena;
clave[1] = &entero2;

result = bloque->bajaRegistro(listaLoca->getParameterList(),clave);	

cout<<"Baja: "<<result<<endl;

datos = bloque->getDatos();
eLibre = *((unsigned short*)datos);
cantRegs = *((unsigned short*)(datos + 2));
	
cout<<"esp libre: "<<eLibre<<endl;
cout<<"cant regs: "<<cantRegs<<endl;

result = bloque->altaRegistro(listaLoca->getParameterList(),registro);

cout<<"Alta: "<<result<<endl;
datos = bloque->getDatos();

eLibre = *((unsigned short*)datos);
cantRegs = *((unsigned short*)(datos + 2));
entero1 = *((int*)(datos + 6));

memcpy(cadena, datos + 12, 4);
*(cadena + 4) = 0;
 
entero2 = *((int*)(datos + 16));	

cout<<"esp libre: "<<eLibre<<endl;
cout<<"cant regs: "<<cantRegs<<endl;
cout<<"campo1: "<<entero1<<endl;
cout<<"campo2: "<<cadena<<endl;
cout<<"campo3: "<<entero2<<endl;

int entero1modificado = 78;
memcpy(registro + 2,&entero1modificado,sizeof(int));
	
result = bloque->modificarRegistro(listaLoca->getParameterList(),16,clave,registro);

cout<<"Modificacion"<<result<<endl;

datos = bloque->getDatos();
	
eLibre = *((unsigned short*)datos);
cantRegs = *((unsigned short*)(datos + 2));
entero1 = *((int*)(datos + 6));

memcpy(cadena, datos + 12, 4);
*(cadena + 4) = 0;
 
entero2 = *((int*)(datos + 16));	

cout<<"esp libre: "<<eLibre<<endl;
cout<<"cant regs: "<<cantRegs<<endl;
cout<<"campo1: "<<entero1<<endl;
cout<<"campo2: "<<cadena<<endl;
cout<<"campo3: "<<entero2<<endl;	*/
/********************************************************************************************************/
