///////////////////////////////////////////////////////////////////////////
//	Archivo   : Clave.cpp
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Definicion de los metodos de las clases ClaveEntera,
//      ClaveVariable, ClaveCompuesta, ClaveBoolean, ClaveFecha,
//      ClaveCaracter, ClaveShort, ClaveReal.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel.
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "Clave.h"
#include <fstream>


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveEntera 
// Descripcion: Implementa claves de tipo enteras (longitud fija).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	CClaveEntera::CClaveEntera()
	{
		/* ATTENZIONE!
		LOS TAMANIOS AHORA VARIAN PORQUE AGREGAMOS EL
		ATRIBUTO hijoDer. SI LA CLAVE ESTA EN UN NODO HOJA, hijoDer
		NO DEBE PERSISTIR. SI EL ARBOL ES B+, EL ATRIBUTO ESTA DE MAS (esta D+ --redobles de bateria).
		*/

		this->tamanio = 2*sizeof(int); 
	}

	CClaveEntera::CClaveEntera(int clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new int(clave));
		this->setReferencia(referencia);
		this->tamanio = 2*sizeof(int);
		this->setHijoDer(hijoDer);
	}

	CClaveEntera::~CClaveEntera()
	{
		//Se libera la memoria del valor
		delete (int*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////	

	CClave* CClaveEntera::copiar()
	{
		int clave = *((int*)this->getValor());
		return new CClaveEntera(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char CClaveEntera::comparar(CClave* otraClave)
	{
		char resultado;
		void* puntero;
		int* codigoOtro;
		int* codigoPropio;
	
		/*Se obtiene el codigo de la otra clave*/
		puntero = otraClave->getValor();
		codigoOtro = static_cast<int*>(puntero);
		
		/*Se obtiene el propio codigo*/
		puntero = this->getValor();
		codigoPropio = static_cast<int*>(puntero);
		
		/*Comparacion*/
		if(*codigoPropio == *codigoOtro)
			resultado = 0;
		else if(*codigoOtro > *codigoPropio)
			resultado = -1;
		else
			resultado = 1;
		
		return resultado;
	}

	void CClaveEntera::imprimir(ostream& salida)
	{
		int clave = *(static_cast<int*>(this->getValor()));
		
		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;		
	}

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveBoolean
// Descripcion: Implementa claves de tipo booleanas (verdadero/falso).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	CClaveBoolean::CClaveBoolean()
	{
		this->tamanio = sizeof(bool) + sizeof(unsigned int);
	}

	CClaveBoolean::CClaveBoolean(bool clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new bool(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(bool) + sizeof(unsigned int); 
	}

	CClaveBoolean::~CClaveBoolean()
	{
		//Se libera la memoria del valor
		delete (bool*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////	

	CClave* CClaveBoolean::copiar()
	{
		bool clave = *((bool*)this->getValor());
		return new CClaveBoolean(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char CClaveBoolean::comparar(CClave* otraClave)
	{
		char resultado;
		void* puntero;
		bool* codigoOtro;
		bool* codigoPropio;
	
		/*Se obtiene el codigo de la otra clave*/
		puntero = otraClave->getValor();
		codigoOtro = static_cast<bool*>(puntero);
		
		/*Se obtiene el propio codigo*/
		puntero = this->getValor();
		codigoPropio = static_cast<bool*>(puntero);
		
		/*Comparacion*/
		if(*codigoPropio == *codigoOtro)
			resultado = 0;
		else
			resultado = 1;
		
		return resultado;
	}

	void CClaveBoolean::imprimir(ostream& salida)
	{
		int clave = *(static_cast<bool*>(this->getValor()));
		
		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;		
	}


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveChar
// Descripcion: Implementa claves de tipo char (1 byte).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	CClaveChar::CClaveChar()
	{
		this->tamanio = sizeof(char) + sizeof(unsigned int);
	}

	CClaveChar::CClaveChar(char clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new char(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(char) + sizeof(unsigned int); 
	}

	CClaveChar::~CClaveChar()
	{
		//Se libera la memoria del valor
		delete (char*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////	

	CClave* CClaveChar::copiar()
	{
		char clave = *((char*)this->getValor());
		return new CClaveChar(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char CClaveChar::comparar(CClave* otraClave)
	{
		char resultado;
		void* puntero;
		char* codigoOtro;
		char* codigoPropio;
	
		/*Se obtiene el codigo de la otra clave*/
		puntero = otraClave->getValor();
		codigoOtro = static_cast<char*>(puntero);
		
		/*Se obtiene el propio codigo*/
		puntero = this->getValor();
		codigoPropio = static_cast<char*>(puntero);
		
		/*Comparacion*/
		if(*codigoPropio == *codigoOtro)
			resultado = 0;
		else if(*codigoOtro > *codigoPropio)
			resultado = -1;
		else
			resultado = 1;
		
		return resultado;
	}

	void CClaveChar::imprimir(ostream& salida)
	{
		char clave = *(static_cast<char*>(this->getValor()));
		
		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;		
	}


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveShort
// Descripcion: Implementa claves de tipo entero corto (2 bytes).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	CClaveShort::CClaveShort()
	{
		this->tamanio = sizeof(short) + sizeof(unsigned int);
	}

	CClaveShort::CClaveShort(short clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new short(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(short) + sizeof(unsigned int); 
	}

	CClaveShort::~CClaveShort()
	{
		//Se libera la memoria del valor
		delete (short*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////	

	CClave* CClaveShort::copiar()
	{
		short clave = *((short*)this->getValor());
		return new CClaveShort(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char CClaveShort::comparar(CClave* otraClave)
	{
		char resultado;
		void* puntero;
		short* codigoOtro;
		short* codigoPropio;
	
		/*Se obtiene el codigo de la otra clave*/
		puntero = otraClave->getValor();
		codigoOtro = static_cast<short*>(puntero);
		
		/*Se obtiene el propio codigo*/
		puntero = this->getValor();
		codigoPropio = static_cast<short*>(puntero);
		
		/*Comparacion*/
		if(*codigoPropio == *codigoOtro)
			resultado = 0;
		else if(*codigoOtro > *codigoPropio)
			resultado = -1;
		else
			resultado = 1;
		
		return resultado;
	}

	void CClaveShort::imprimir(ostream& salida)
	{
		short clave = *(static_cast<short*>(this->getValor()));
		
		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;		
	}

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveReal
// Descripcion: Implementa claves de tipo float (4 bytes).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	CClaveReal::CClaveReal()
	{
		this->tamanio = sizeof(float) + sizeof(unsigned int);
	}

	CClaveReal::CClaveReal(float clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new float(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(float) + sizeof(unsigned int); 
	}

	CClaveReal::~CClaveReal()
	{
		//Se libera la memoria del valor
		delete (float*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////	

	CClave* CClaveReal::copiar()
	{
		short clave = *((short*)this->getValor());
		return new CClaveShort(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char CClaveReal::comparar(CClave* otraClave)
	{
		char resultado;
		void* puntero;
		float* codigoOtro;
		float* codigoPropio;
	
		/*Se obtiene el codigo de la otra clave*/
		puntero = otraClave->getValor();
		codigoOtro = static_cast<float*>(puntero);
		
		/*Se obtiene el propio codigo*/
		puntero = this->getValor();
		codigoPropio = static_cast<float*>(puntero);
		
		/*Comparacion*/
		if(*codigoPropio == *codigoOtro)
			resultado = 0;
		else if(*codigoOtro > *codigoPropio)
			resultado = -1;
		else
			resultado = 1;
		
		return resultado;
	}

	void CClaveReal::imprimir(ostream& salida)
	{
		float clave = *(static_cast<float*>(this->getValor()));
		
		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;		
	}


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveFecha
// Descripcion: Implementa claves de tipo fecha (AAAAMMDD).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	CClaveFecha::CClaveFecha()
	{
		this->tamanio = sizeof(fecha) + sizeof(unsigned int);
	}

	CClaveFecha::CClaveFecha(fecha* clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(clave);
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(fecha) + sizeof(unsigned int); 
	}

	CClaveFecha::~CClaveFecha()
	{
		//Se libera la memoria del valor
		delete (fecha*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////	

	CClave* CClaveFecha::copiar()
	{
		fecha* clave = (fecha*)this->getValor();
		return new CClaveFecha(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char CClaveFecha::comparar(CClave* otraClave)
	{
		char resultado;
		void* puntero;
		fecha* codigoOtro;
		fecha* codigoPropio;
	
		/*Se obtiene el codigo de la otra clave*/
		puntero = otraClave->getValor();
		codigoOtro = static_cast<fecha*>(puntero);
		
		/*Se obtiene el propio codigo*/
		puntero = this->getValor();
		codigoPropio = static_cast<fecha*>(puntero);
		
		/*Comparacion*/
		if (codigoPropio->anio == codigoOtro->anio){
			if (codigoPropio->mes == codigoOtro->mes){
				if (codigoPropio->dia == codigoOtro->dia)
					resultado = 0;
				else if (codigoPropio->dia < codigoOtro->dia)
					resultado = -1;
				else
					resultado = 1;
			}else if (codigoPropio->mes < codigoOtro->mes)
				resultado = -1;
			else
				resultado = 1;
		}else if (codigoPropio->anio < codigoOtro->anio)
			resultado = -1;
		else
			resultado = 1;
		
		return resultado;
	}

	void CClaveFecha::imprimir(ostream& salida)
	{
		fecha* clave = (static_cast<fecha*>(this->getValor()));
		
		salida<<"Clave: "<<clave->dia << "/" << clave->mes << "/" << clave->anio;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;		
	}



///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveVariable
// Descripcion: Implementa claves de longitud variable.
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	CClaveVariable::CClaveVariable(string clave, unsigned int referencia, int hijoDer)
	{
		//Se trunca el nombre si es de mas de longMaxCadena caracteres
//		if (clave.size()> CClaveCadena::longMaxCadena)
//			clave.erase(CClaveCadena::longMaxCadena);			
		
		this->setValor(new string(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = (clave.size()+1)*sizeof(char) + 2*sizeof(unsigned int);
	}

	CClaveVariable::~CClaveVariable()
	{	
		delete (string*)this->getValor();
	}

	
	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
	CClave* CClaveVariable::copiar()
	{
		string* clave = (string*)this->getValor();
		
		return new CClaveVariable(*clave,this->obtenerReferencia(),this->getHijoDer());
	}
	
	char CClaveVariable::comparar(CClave* otraClave)
	{	
		string* palabraOtra = (string*) ((CClaveVariable*)otraClave)->getValor();
		string* palabraThis = (string*) this->getValor();
		
		for(unsigned int i=0;i<palabraOtra->length();i++)
			*(palabraOtra + i) = tolower((*palabraOtra)[i]);
		
		for(unsigned int j=0;j<palabraThis->length();j++)
			*(palabraThis + j) = tolower((*palabraThis)[j]);
			
		char resultado = palabraThis->compare(*palabraOtra);
		
		return resultado; 
	}


	void CClaveVariable::imprimir(ostream& salida)
	{	
		 /*
		if(this->obtenerReferencia()==-1){
			salida<<"Valor: "<<this->getNombre();
			salida<<" ReferenciaLista: "<<this->getRefListaPrim()<<endl;
		}
		else{ 
			salida<<"Valor: "<<this->getNombre();
			salida<<" Referencia:"<<this->obtenerReferencia();
			salida<<" ReferenciaLista: "<<this->getRefListaPrim()<<endl;
		}
		*/		
	}


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: CClaveCompuesta
// Descripcion: Implementa claves compuestas (lista de claves).
///////////////////////////////////////////////////////////////////////////

/*HASTA ACA LLEGO*/

	/*
ClaveNomMarc::ClaveNomMarc(string nombre,string marca, int referencia){
	
	/*Trunco el nombre si es de mas de 40 caracteres*/
/*	if(nombre.size()>40)
		nombre.erase(40);
	
	/*Trunco la marca si es de mas de 40 caracteres*/
/*	if(marca.size()>40)
		marca.erase(40); 
	
	Cadenas* nueva = new Cadenas;
	
	nueva->marca = marca;
	nueva->nombre = nombre;
	this->setValor(nueva);
	this->setReferencia(referencia);
	this->tamanio = (marca.size()+1 + nombre.size()+1)*sizeof(char) + sizeof(int);
}
 			

Clave* ClaveNomMarc::copiar(){
	Cadenas* nueva = (Cadenas*)this->getValor();
 	return new ClaveNomMarc(nueva->nombre,nueva->marca,this->obtenerReferencia());		
}
 
int ClaveNomMarc::comparar(Clave* otraClave){
	
	Cadenas cadOtra = *(Cadenas*)otraClave->getValor();
	Cadenas cadThis = *(Cadenas*)this->getValor();
	
	string nombreOtra = cadOtra.nombre;
	string nombreThis = cadThis.nombre;
	
	for(unsigned int i=0;i<nombreOtra.length();i++)
		nombreOtra[i] = tolower(nombreOtra[i]);
	
	for(unsigned int j=0;j<nombreThis.length();j++)
		nombreThis[j] = tolower(nombreThis[j]);
		
	int result = nombreThis.compare(nombreOtra);
	
	/*Si los nombres son iguales comparo por marca*/
/*	if(result==0){
		
		string marcaOtra = cadOtra.marca;
		string marcaThis = cadThis.marca;
	
		for(unsigned int k=0;k<marcaOtra.length();k++)
			marcaOtra[k] = tolower(marcaOtra[k]);
	
		for(unsigned int w=0;w<marcaThis.length();w++)
			marcaThis[w] = tolower(marcaThis[w]);
		
		result = marcaThis.compare(marcaOtra);
			
	}
	
	return result;
}

void ClaveNomMarc::setValor(void* nuevoValor){
	
	Cadenas* nuevaCadena = (Cadenas*)nuevoValor;
	
	/*Trunco el nombre si es de mas de 40 caracteres*/
/*	if(nuevaCadena->nombre.size()>40)
		nuevaCadena->nombre.erase(40);
	
	/*Trunco la marca si es de mas de 40 caracteres*/
/*	if(nuevaCadena->marca.size()>40)
		nuevaCadena->marca.erase(40); 
	
	this->tamanio = (nuevaCadena->nombre.size() + nuevaCadena->marca.size() 
						+ 2)*sizeof(char) + sizeof(int);
	
	Clave::setValor(nuevoValor);
}
			
void ClaveNomMarc::imprimir(ostream& salida){
/*
	Cadenas* cadenas;
	cadenas = static_cast<Cadenas*>(this->getValor());
	
	salida<<"Nombre: "<<cadenas->nombre;
	salida<<" Marca: "<<cadenas->marca;
	salida<<" Referencia: "<<this->obtenerReferencia()<<endl;
	*/
	
/*}

ClaveNomMarc::~ClaveNomMarc(){
	delete (Cadenas*)this->getValor();
	
}*/

