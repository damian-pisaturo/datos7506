#include "CapaIndices.h"

#include <iostream>
#include <vector>
#include "Bloque/Bloque.h"
#include "Common/DefinitionsManager.h"
#include "Parser/ParserOperaciones.h"
#include "Indices/IndiceArbol.h"
#include "Indices/IndiceHash.h"

using namespace std;

void crearIndice(const string &nombreTipo, VectorIndices &vectorIndicesPersona,
				const DefinitionsManager::EstructTipoIndice &estructura,
				DefinitionsManager &defManager);


int main(int argc, char** argv) {
	
	if (argc < 2) return ERROR;
	
	// Se instancia el DefinitionsManager. Esta clase tiene todas las definiciones.
	DefinitionsManager defManager;
	
	//Para probar esta capa hay cargadas definiciones de dos tipos (Persona y Pelicula)
	
	//Se crean los indices para los registros de tipo persona.
	DefinitionsManager::ListaTiposIndices* listaTiposIndices;
	
	listaTiposIndices = defManager.getListaTiposIndices("PERSONA");
	
	DefinitionsManager::NodoListaIndices nodoListaIndices;
	
	VectorIndices vectorIndicesPersona;
	
	unsigned char tipoIndice;
	
	
	for (DefinitionsManager::ListaTiposIndices::const_iterator iter = listaTiposIndices->begin();
		iter != listaTiposIndices->end(); ++iter) {
		
		nodoListaIndices = *iter;
		
		tipoIndice = nodoListaIndices.estructTipoIndice.tipoEstructura;
		
		crearIndice("PERSONA", vectorIndicesPersona, nodoListaIndices.estructTipoIndice,defManager);
		
	}
	
	// Se crean los índices para los registros de tipo Pelicula.
	listaTiposIndices = defManager.getListaTiposIndices("PELICULA");
		
	
	VectorIndices vectorIndicesPelicula;
	
	for (DefinitionsManager::ListaTiposIndices::const_iterator iter = listaTiposIndices->begin();
		iter != listaTiposIndices->end(); ++iter) {
		
		nodoListaIndices = *iter;
		
		tipoIndice = nodoListaIndices.estructTipoIndice.tipoEstructura;
		
		crearIndice("PELICULA", vectorIndicesPelicula, nodoListaIndices.estructTipoIndice,defManager);
	}
		
	ParserOperaciones parserOperaciones(argv[1], vectorIndicesPersona, vectorIndicesPelicula);
	//TODO: modificar parser
	
}
 


void crearIndice(const string &nombreTipo, VectorIndices &vectorIndicesPersona,
				const DefinitionsManager::EstructTipoIndice &estructura,
				DefinitionsManager &defManager) {
	
	Indice* indice;
	DefinitionsManager::ListaTiposAtributos *listaTiposAtributos = defManager.getListaTiposAtributos(nombreTipo);
	
	switch (estructura.tipoEstructura) {
	
		case TipoIndices::ARBOL_BP:
		case TipoIndices::ARBOL_BS:	
			indice = new IndiceArbol(estructura.tipoIndice, estructura.tamanioBloque, estructura.tipoClave,
									defManager.getListaTipos(nombreTipo), estructura.tipoEstructura, estructura.tamanioBloque,
									estructura.nombreArchivo);
			vectorIndicesPersona.push_back(indice);
			break;
			
		case TipoIndices::HASH:
			indice = new IndiceHash(*listaTiposAtributos,estructura.tamanioBloque,estructura.nombreArchivo);
			vectorIndicesPersona.push_back(indice);
	}	
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
