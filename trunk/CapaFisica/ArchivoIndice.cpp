#include "ArchivoIndice.h"


/******************************************************************************/
/* Clase ArchivoIndice Codigo Producto */
/*-------------------------------------*/

/*Constructor del hijo llama al constructor del padre*/
ArchivoIndiceCodigo::ArchivoIndiceCodigo(int tamNodo,string nombreArchivo):ArchivoIndice(tamNodo,nombreArchivo){
		
		  
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceCodigo::copiarClaveHoja(Clave* clave,char* &puntero){
	
	ClaveCodigo* claveCod = static_cast<ClaveCodigo*>(clave);
	
	/*Copio el valor*/
	memcpy(puntero,claveCod->getValor(),sizeof(int));
	puntero += sizeof(int);
	
	/*Copio la refRegistro*/
	int referencia = claveCod->obtenerReferencia();
	memcpy(puntero,&referencia,sizeof(int));
	puntero += sizeof(int);
	
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceCodigo::copiarClaveNoHoja(Clave* clave,char* &puntero){
	this->copiarClaveHoja(clave,puntero);
}
/*--------------------------------------------------------------------------------------------*/
Clave* ArchivoIndiceCodigo::leerClaveHoja(char* &buffer){
	
	ClaveCodigo* claveNueva;
	int* codigo;
	int* refRegistro;
	
	/*Interpreto el codigo*/
	codigo = (int*)buffer;
	buffer += sizeof(int);		
	
	/*Interpreto la refRegistro*/	
	refRegistro = (int*)buffer;
	buffer += sizeof(int);
	
	/*Crear la clave*/
	claveNueva = new ClaveCodigo(*codigo,*refRegistro);
	
	return claveNueva;	
}
/*--------------------------------------------------------------------------------------------*/
Clave* ArchivoIndiceCodigo::leerClaveNoHoja(char* &buffer){
	return this->leerClaveHoja(buffer);
}
/******************************************************************************/
/* Clase ArchivoIndiceCadena (Nombre o Marca de Producto) - Indice Secundario */
/*----------------------------------------------------------------------------*/


ArchivoIndiceCadena::ArchivoIndiceCadena(int tamNodo,string nombreArchivo,int tamanioBloqueLista):ArchivoIndiceSecundario(tamNodo,nombreArchivo,tamanioBloqueLista){
				

}

/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceCadena::copiarClaveNoHoja(Clave* clave,char* &puntero){
	
	ClaveCadena* claveNom = static_cast<ClaveCadena*>(clave);
	
	/*Copio el nombre*/
	string nombre = claveNom->getNombre();
	/*Copio el char* al buffer*/ 
	memcpy(puntero,nombre.c_str(),nombre.size()+1);
	puntero += (nombre.size()+1)*sizeof(char) ;
	
	/*Copio la RefListaPrim*/
	int refLista = claveNom->getRefListaPrim();
	memcpy(puntero,&refLista,sizeof(int));
	puntero += sizeof(int);

	/*Copio la refRegistro*/
	int referencia = claveNom->obtenerReferencia();
	memcpy(puntero,&referencia,sizeof(int));
	puntero += sizeof(int);
	

}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceCadena::copiarClaveHoja(Clave* clave,char* &puntero){
/*En este caso la claveHoja no tiene refRegistro*/
	
//	ClaveCadena* claveNom = static_cast<ClaveCadena*>(clave);
//	
//	/*Copio el nombre*/
//	string nombre = claveNom->getNombre();
//	/*Copio el char* al buffer*/ 
//	memcpy(puntero,nombre.c_str(),nombre.size()+1);
//	puntero += (nombre.size()+1)*sizeof(char) ;
//	
//	/*Copio la RefListaPrim*/
//	int refLista = claveNom->getRefListaPrim();
//	memcpy(puntero,&refLista,sizeof(int));
//	puntero += sizeof(int);	
	
	this->copiarClaveNoHoja(clave,puntero);
}
/*--------------------------------------------------------------------------------------------*/
Clave* ArchivoIndiceCadena::leerClaveNoHoja(char* &buffer){
	
	ClaveCadena* claveNueva;
	string nombre;
	int* refRegistro;
	int* refListaPrim;
	
	/*Interpreto el nombre*/
	nombre = buffer;
	buffer += nombre.size()+1;		
	
	/*Interpreto la refListaPrim*/
	refListaPrim = (int*)buffer;
	buffer += sizeof(int);
	
	/*Interpreto la refRegistro*/	
	refRegistro = (int*)buffer;
	buffer += sizeof(int);
	
	/*Crear la clave*/
	claveNueva = new ClaveCadena(nombre,*refListaPrim,*refRegistro);
	
	return claveNueva;
		
}
/*--------------------------------------------------------------------------------------------*/
Clave* ArchivoIndiceCadena::leerClaveHoja(char* &buffer){
	
//	ClaveCadena* claveNueva;
//	string nombre;
//	int* refListaPrim;
//	
//	/*Interpreto el nombre*/
//	nombre = buffer;
//	buffer += nombre.size()+1;		
//	
//	/*Interpreto la refListaPrim*/
//	refListaPrim = (int*)buffer;
//	buffer += sizeof(int);
//	
//	
//	/*Crear la clave*/
//	claveNueva = new ClaveCadena(nombre,*refListaPrim);
//	
//	return claveNueva;

	return this->leerClaveNoHoja(buffer);
}
/********************************************************************************************/
/* Clase ArchivoIndice Nombre y Marca de Producto (Clave Candidata) - Indice Identificacion */
/*------------------------------------------------------------------------------------------*/

ArchivoIndiceNomMarc::ArchivoIndiceNomMarc(int tamNodo,string nombreArchivo):ArchivoIndice(tamNodo,nombreArchivo){
		
	
}

/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceNomMarc::copiarClaveHoja(Clave* clave,char* &puntero){
	
	ClaveNomMarc* claveNom = static_cast<ClaveNomMarc*>(clave);
	

	/*Obtengo el nombre y la marca*/
	ClaveNomMarc::Cadenas nomMarc;
	nomMarc = *((ClaveNomMarc::Cadenas*)claveNom->getValor());
	
	/*Copio el nombre*/
	/*Copio el char* al buffer*/ 
	memcpy(puntero,nomMarc.nombre.c_str(),nomMarc.nombre.size()+1);
	puntero += (nomMarc.nombre.size()+1)*sizeof(char);
	
	
	/*Copio la marca*/
	/*Copio el char* al buffer*/
	memcpy(puntero,nomMarc.marca.c_str(),nomMarc.marca.size()+1);
	puntero += (nomMarc.marca.size()+1)*sizeof(char);
	
	/*Copio la refRegistro*/
	int referencia = claveNom->obtenerReferencia();
	memcpy(puntero,&referencia,sizeof(int));
	puntero += sizeof(int);
		
}
/*--------------------------------------------------------------------------------------------*/
void ArchivoIndiceNomMarc::copiarClaveNoHoja(Clave* clave,char* &puntero){
	this->copiarClaveHoja(clave,puntero);
}
/*--------------------------------------------------------------------------------------------*/
Clave* ArchivoIndiceNomMarc::leerClaveHoja(char* &buffer){
	
	ClaveNomMarc* claveNueva;
	string nombre;
	string marca;
	int* refRegistro;
	
	/*Interpreto el nombre*/
	nombre = buffer;
	buffer += nombre.size()+1;	
	
	/*Interpreto la marca*/
	marca = buffer;
	buffer += marca.size()+1;
	
	/*Interpreto la refRegistro*/	
	refRegistro = (int*)buffer;
	buffer += sizeof(int);
	
	/*Crear la clave*/
	claveNueva = new ClaveNomMarc(nombre,marca,*refRegistro);
	
	return claveNueva;	
}
/*--------------------------------------------------------------------------------------------*/
Clave* ArchivoIndiceNomMarc::leerClaveNoHoja(char* &buffer){
	return this->leerClaveHoja(buffer);	
}
