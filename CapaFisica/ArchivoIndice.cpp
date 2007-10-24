///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndice.cpp
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases ArchivoIndice correspodientes
//      a cada tipo de dato (booleano, char, short, entero, real, fecha, 
//		string) e indice (Griego y Romano).
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "ArchivoIndice.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroGriego 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
	ArchivoIndiceEnteroGriego::ArchivoIndiceEnteroGriego(int tamNodo, string nombreArchivo):
		ArchivoIndiceArbol(tamNodo, nombreArchivo){ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
	void ArchivoIndiceEnteroGriego::copiarClaveHoja(Clave* clave, char* &puntero)
	{	
		ClaveEntera* claveEntera = static_cast<ClaveEntera*>(clave);
		
		/*Copio el valor*/
		memcpy(puntero, claveEntera->getValor(), sizeof(int));
		puntero += sizeof(int);
		
		/*Copio la refRegistro*/
		unsigned int referencia = claveEntera->obtenerReferencia();
		memcpy(puntero,&referencia,sizeof(int));
		puntero += sizeof(int);
	}
	
	void ArchivoIndiceEnteroGriego::copiarClaveNoHoja(Clave* clave, char* &puntero)
	{
		this->copiarClaveHoja(clave,puntero);
	}

	Clave* ArchivoIndiceEnteroGriego::leerClaveHoja(char* &buffer)
	{
		int* codigo;
		int* refRegistro;
		
		/*Interpreto el codigo*/
		codigo = (int*)buffer;
		buffer += sizeof(int);		
		
		/*Interpreto la refRegistro*/	
		refRegistro = (int*)buffer;
		buffer += sizeof(int);
		
		/*Crear la clave*/
		return new ClaveEntera(*codigo,*refRegistro);	
	}
	
	Clave* ArchivoIndiceEnteroGriego::leerClaveNoHoja(char* &buffer)
	{
		int* codigo;
		int* refRegistro;
		unsigned int* hijoDer;
		
		/*Interpreto el codigo*/
		codigo = (int*)buffer;
		buffer += sizeof(int);		
		
		/*Interpreto la refRegistro*/	
		refRegistro = (int*)buffer;
		buffer += sizeof(int);
		
		/*Interpreto la refRegistro*/	
		hijoDer = (int*)buffer;
		buffer += sizeof(int);
		
		/*Crear la clave*/
		return new ClaveEntera(*codigo,*refRegistro, *hijoDer);
	}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroRomano
//		   (Implementa archivo de indices secundarios de clave entera).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceEnteroRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamanioBloqueLista)
		{ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		void ArchivoIndiceEnteroRomano::copiarClaveNoHoja(Clave* clave, char* &puntero)
		{			
			ClaveEntera* claveEntera = static_cast<ClaveEntera*>(clave);
			
			//Obtencion del valor entero
			int valor = *((int*)claveEntera->getValor());
			
			//Copia del entero al buffer
			memcpy(puntero, &valor, sizeof(int));
			puntero += sizeof(int);
		
			//Copia de la referencia al hijo derecho
			unsigned int hijoDer = claveEntera->getHijoDerecho();
			memcpy(puntero,&hijoDer,sizeof(unsigned int));
			puntero += sizeof(int);
			
			//Si la referencia es -1, se trata de un nodo B+ y
			//no debe persistirse en disco.
			if (claveEntera->getReferencia() != -1 ){
				//Copia de la referencia a la lista.
				int referencia = claveEntera->obtenerReferencia();
				memcpy(puntero,&referencia,sizeof(int));
				puntero += sizeof(int);
			}
		}
		
		void ArchivoIndiceEnteroRomano::copiarClaveHoja(Clave* clave, char* &puntero)
		{
			//En este caso la claveHoja no tiene refRegistro
			
			ClaveEntera* claveEntera = static_cast<ClaveEntera*>(clave);
						
			//Obtencion del valor entero.
			int valor = *((int*)claveEntera->getValor());
			
			//Copia del entero al buffer.
			memcpy(puntero, &valor, sizeof(int));
			puntero += sizeof(int);
		
			//Copia de la referencia a la lista de 
			//claves primarias.
			unsigned int referencia = claveEntera->getReferencia();
			memcpy(puntero, &referencia, sizeof(int));
			puntero += sizeof(int);
		}
		
		Clave* ArchivoIndiceEnteroRomano::leerClaveNoHoja(char* &buffer)
		{
			int* valor;
			int* refRegistro;
			unsigned int* refListaPrim;
			
			//Interpreto el nombre
			valor = (int*) buffer;
			buffer += sizeof(int);		
			
			//Interpreto la refListaPrim
			refListaPrim = (int*)buffer;
			buffer += sizeof(int);
			
			//Interpreto la refRegistro	
			refRegistro = (int*)buffer;
			buffer += sizeof(int);
			
			//Crear y devolver la clave
			return new ClaveEntera(*valor, *refListaPrim, *refRegistro);				
		}
		
		Clave* ArchivoIndiceEnteroRomano::leerClaveHoja(char* &buffer)
		{
			return this->leerClaveNoHoja(buffer);
		}
		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceBooleanGriego
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceBooleanGriego(unsigned int tamNodo, string nombreArchivo):
			ArchivoIndiceArbol(tamNodo, nombreArchivo)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		void ArchivoIndiceBooleanGriego::copiarClaveHoja(Clave* clave, char* &puntero)
		{	
			ClaveBoolean* claveBoolean = static_cast<ClaveBoolean*>(clave);
			
			/*Copia del valor booleano*/
			memcpy(puntero, claveEntera->getValor(), sizeof(bool));
			puntero += sizeof(bool);
			
			/*Copio la refRegistro*/
			unsigned int referencia = claveEntera->obtenerReferencia();
			memcpy(puntero,&referencia, sizeof(int));
			puntero += sizeof(int);
		}
		
		void ArchivoIndiceBooleanGriego::copiarClaveNoHoja(Clave* clave, char* &puntero)
		{
			ClaveBoolean* claveBoolean = static_cast<ClaveBoolean*>(clave);
			
			//Copia del valor booleano
			memcpy(puntero, claveEntera->getValor(), sizeof(bool));
			puntero += sizeof(bool);
			
			//Copia la refRegistro
			int referencia = claveEntera->obtenerReferencia();
			if (referencia != -1){
				memcpy(puntero, &referencia, sizeof(unsigned int));
				puntero += sizeof(int);
			}
		}
		
		Clave* ArchivoIndiceBooleanGriego::leerClaveNoHoja(char* &buffer)
		{
			bool* codigo;
			int* refRegistro;
			unsigned int* hijoDer;
			
			//Interpreto el codigo
			codigo = (bool*)buffer;
			buffer += sizeof(int);		
			
			//Interpreto la refRegistro	
			refRegistro = (int*)buffer;
			buffer += sizeof(int);
			
			//Interpreto la referencia al hijo derecho
			hijoDer = (unsigned int*)buffer;
			buffer += sizeof(unsigned int);
			
			//Crear la clave
			return new ClaveBoolean(*codigo,*refRegistro, *hijoDer);
		}
		
		Clave* ArchivoIndiceBooleanGriego::leerClaveHoja(char* &buffer)
		{
			bool* codigo;
			unsigned int* refRegistro;
			
			//Interpreto el codigo
			codigo = (bool*)buffer;
			buffer += sizeof(bool);		
			
			//Interpreto la refRegistro	
			refRegistro = (int*)buffer;
			buffer += sizeof(int);
			
			//Crear la clave
			return new ClaveBoolean(*codigo, *refRegistro);	
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharGriego
//		   (Implementa archivo de indices primarios de clave tipo char).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCharGriego(unsigned int tamNodo, string nombreArchivo):
			ArchivoIndiceArbol(tamNodo, nombreArchivo)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		void ArchivoIndiceCharGriego::copiarClaveHoja(Clave* clave, char* &puntero)
		{	
			ClaveChar* claveChar = static_cast<ClaveChar*>(clave);
			
			//Copia del valor char
			memcpy(puntero, claveChar->getValor(), sizeof(char));
			puntero += sizeof(char);
			
			//Copia de la referencia al registro
			int referencia = claveChar->obtenerReferencia();
			memcpy(puntero,&referencia, sizeof(int));
			puntero += sizeof(int);
		}
		
		void ArchivoIndiceCharGriego::copiarClaveNoHoja(Clave* clave, char* &puntero)
		{
			ClaveChar* claveChar = static_cast<ClaveChar*>(clave);
			
			//Copia del valor char
			memcpy(puntero, claveChar->getValor(), sizeof(bool));
			puntero += sizeof(bool);
			
			//Copia la refRegistro
			int referencia = claveChar->obtenerReferencia();
			
			if (referencia != -1){
				//El nodo no pertenece a un arbol B+.
				memcpy(puntero, &referencia, sizeof(int));
				puntero += sizeof(int);
			}
		}
		
		Clave* ArchivoIndiceCharGriego::leerClaveNoHoja(char* &buffer)
		{
			char* codigo;
			int* refRegistro;
			unsigned int* hijoDer;
			
			//Interpreto el codigo
			codigo  = (char*)buffer;
			buffer += sizeof(int);		
			
			//Interpreto la refRegistro	
			refRegistro = (int*)buffer;
			buffer     += sizeof(int);
			
			//Interpreto la referencia al hijo derecho
			hijoDer = (unsigned int*)buffer;
			buffer += sizeof(unsigned int);
			
			//Crear la clave
			return new ClaveChar(*codigo,*refRegistro, *hijoDer);
		}
		
		Clave* ArchivoIndiceCharGriego::leerClaveHoja(char* &buffer)
		{
			char* codigo;
			int* refRegistro;
			
			//Interpreto el codigo
			codigo = (char*)buffer;
			buffer += sizeof(char);		
			
			//Interpreto la refRegistro	
			refRegistro = (int*)buffer;
			buffer += sizeof(int);
			
			//Crear la clave
			return new ClaveChar(*codigo, *refRegistro);	
		}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharRomano
//		   (Implementa archivo de indices secundarios de clave tipo char).
///////////////////////////////////////////////////////////////////////////	
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCharRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamanioBloqueLista)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos publicos
	//////////////////////////////////////////////////////////////////////
		void ArchivoIndiceCharRomano::copiarClaveNoHoja(Clave* clave, char* &puntero)
		{			
			ClaveChar* claveEntera = static_cast<ClaveChar*>(clave);
			
			//Obtencion del valor tipo char
			char valor = *((char*)claveEntera->getValor());
			
			//Copia del char al buffer
			memcpy(puntero, &valor, sizeof(char));
			puntero += sizeof(char);
		
			//Copia de la referencia al hijo derecho
			unsigned int hijoDer = claveEntera->getHijoDerecho();
			memcpy(puntero,&hijoDer,sizeof(unsigned int));
			puntero += sizeof(int);
			
			//Si la referencia es -1, se trata de un nodo B+ y
			//no debe persistirse en disco.
			if (claveEntera->getReferencia() != -1 ){
				//Copia de la referencia a la lista.
				int referencia = claveEntera->obtenerReferencia();
				memcpy(puntero,&referencia,sizeof(int));
				puntero += sizeof(int);
			}
		}
			
		void ArchivoIndiceCharRomano::copiarClaveHoja(Clave* clave, char* &puntero)
		{			
			ClaveChar* claveEntera = static_cast<ClaveChar*>(clave);
						
			//Obtencion del valor tipo char.
			char valor = *((char*)claveEntera->getValor());
			
			//Copia del entero al buffer.
			memcpy(puntero, &valor, sizeof(char));
			puntero += sizeof(cjar);
		
			//Copia de la referencia a la lista de 
			//claves primarias.
			int referencia = claveEntera->getReferencia();
			memcpy(puntero, &referencia, sizeof(int));
			puntero += sizeof(int);
		}
			
		Clave* ArchivoIndiceCharRomano::leerClaveNoHoja(char* &buffer)
		{
			char* valor;
			int* refRegistro;
			unsigned int* refListaPrim;
			
			/*Interpreto el nombre*/
			valor = (char) buffer;
			buffer += sizeof(char);		
			
			/*Interpreto la refListaPrim*/
			refListaPrim = (int*)buffer;
			buffer += sizeof(int);
			
			/*Interpreto la refRegistro*/	
			refRegistro = (int*)buffer;
			buffer += sizeof(int);
			
			/*Crear y devolver la clave*/			
			return new ClaveChar(*valor, *refListaPrim,*refRegistro);
		}
		
		Clave* ArchivoIndiceCharRomano::leerClaveHoja(char* &buffer)
		{
			char* valor;
			int* refRegistro;
			unsigned int* refListaPrim;
			
			/*Interpreto el nombre*/
			valor = (char) buffer;
			buffer += sizeof(char);		
			
			/*Interpreto la refListaPrim*/
			refListaPrim = (int*)buffer;
			buffer += sizeof(int);
			
			/*Crear y devolver la clave*/			
			return new ClaveChar(*valor, *refListaPrim);		
		}
		
/******************************************************************************/
/* Clase ArchivoIndiceCadena (Nombre o Marca de Producto) - Indice Secundario */
/*----------------------------------------------------------------------------*/


ArchivoIndiceCadena::ArchivoIndiceCadena(int tamNodo,string nombreArchivo,unsigned int tamanioBloqueLista):ArchivoIndiceSecundario(tamNodo,nombreArchivo,tamanioBloqueLista){
				

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
