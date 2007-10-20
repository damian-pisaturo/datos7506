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
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "Clave.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveEntera
// Descripcion: Implementa claves de tipo enteras (longitud fija).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveEntera::ClaveEntera()
	{
		/* ATTENZIONE!
		LOS TAMANIOS AHORA VARIAN PORQUE AGREGAMOS EL
		ATRIBUTO hijoDer. SI LA CLAVE ESTA EN UN NODO HOJA, hijoDer
		NO DEBE PERSISTIR. SI EL ARBOL ES B+, EL ATRIBUTO ESTA DE MAS (esta D+ --redobles de bateria).
		*/

		this->tamanio = 2*sizeof(int);
	}

	ClaveEntera::ClaveEntera(int clave, unsigned int referencia, unsigned int hijoDer)
	{
		this->setValor(new int(clave));
		this->setReferencia(referencia);
		this->tamanio = 2*sizeof(int);
		this->setHijoDer(hijoDer);
	}

	ClaveEntera::~ClaveEntera()
	{
		//Se libera la memoria del valor
		delete (int*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

	Clave* ClaveEntera::copiar()
	{
		int clave = *((int*)this->getValor());
		return new ClaveEntera(clave, this->obtenerReferencia(), this->getHijoDer());
	}

	char ClaveEntera::comparar(Clave* otraClave)
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

	void ClaveEntera::imprimir(ostream& salida)
	{
		int clave = *(static_cast<int*>(this->getValor()));

		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;
	}

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveBoolean
// Descripcion: Implementa claves de tipo booleanas (verdadero/falso).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveBoolean::ClaveBoolean()
	{
		this->tamanio = sizeof(bool) + sizeof(unsigned int);
	}

	ClaveBoolean::ClaveBoolean(bool clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new bool(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(bool) + sizeof(unsigned int);
	}

	ClaveBoolean::~ClaveBoolean()
	{
		//Se libera la memoria del valor
		delete (bool*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

	Clave* ClaveBoolean::copiar()
	{
		bool clave = *((bool*)this->getValor());
		return new ClaveBoolean(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char ClaveBoolean::comparar(Clave* otraClave)
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
		else if(*codigoOtro > *codigoPropio)
			resultado = -1;
		else
			resultado = 1;

		return resultado;
	}

	void ClaveBoolean::imprimir(ostream& salida)
	{
		int clave = *(static_cast<bool*>(this->getValor()));

		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;
	}


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveChar
// Descripcion: Implementa claves de tipo char (1 byte).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveChar::ClaveChar()
	{
		this->tamanio = sizeof(char) + sizeof(unsigned int);
	}

	ClaveChar::ClaveChar(char clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new char(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(char) + sizeof(unsigned int);
	}

	ClaveChar::~ClaveChar()
	{
		//Se libera la memoria del valor
		delete (char*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

	Clave* ClaveChar::copiar()
	{
		char clave = *((char*)this->getValor());
		return new ClaveChar(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char ClaveChar::comparar(Clave* otraClave)
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

	void ClaveChar::imprimir(ostream& salida)
	{
		char clave = *(static_cast<char*>(this->getValor()));

		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;
	}


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveShort
// Descripcion: Implementa claves de tipo entero corto (2 bytes).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveShort::ClaveShort()
	{
		this->tamanio = sizeof(short) + sizeof(unsigned int);
	}

	ClaveShort::ClaveShort(short clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new short(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(short) + sizeof(unsigned int);
	}

	ClaveShort::~ClaveShort()
	{
		//Se libera la memoria del valor
		delete (short*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

	Clave* ClaveShort::copiar()
	{
		short clave = *((short*)this->getValor());
		return new ClaveShort(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char ClaveShort::comparar(Clave* otraClave)
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

	void ClaveShort::imprimir(ostream& salida)
	{
		short clave = *(static_cast<short*>(this->getValor()));

		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;
	}

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveReal
// Descripcion: Implementa claves de tipo float (4 bytes).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveReal::ClaveReal()
	{
		this->tamanio = sizeof(float) + sizeof(unsigned int);
	}

	ClaveReal::ClaveReal(float clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(new float(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(float) + sizeof(unsigned int);
	}

	ClaveReal::~ClaveReal()
	{
		//Se libera la memoria del valor
		delete (float*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

	Clave* ClaveReal::copiar()
	{
		short clave = *((short*)this->getValor());
		return new ClaveShort(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char ClaveReal::comparar(Clave* otraClave)
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

	void ClaveReal::imprimir(ostream& salida)
	{
		float clave = *(static_cast<float*>(this->getValor()));

		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;
	}


///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveFecha
// Descripcion: Implementa claves de tipo fecha (AAAAMMDD).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveFecha::ClaveFecha()
	{
		this->tamanio = sizeof(fecha) + sizeof(unsigned int);
	}

	ClaveFecha::ClaveFecha(fecha* clave, unsigned int referencia, int hijoDer)
	{
		this->setValor(clave);
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(fecha) + sizeof(unsigned int);
	}

	ClaveFecha::~ClaveFecha()
	{
		//Se libera la memoria del valor
		delete (fecha*)this->getValor();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////

	Clave* ClaveFecha::copiar()
	{
		fecha* clave = (fecha*)this->getValor();
		return new ClaveFecha(clave,this->obtenerReferencia(), this->getHijoDer());
	}

	char ClaveFecha::comparar(Clave* otraClave)
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

	void ClaveFecha::imprimir(ostream& salida)
	{
		fecha* clave = (static_cast<fecha*>(this->getValor()));

		salida<<"Clave: "<<clave->dia << "/" << clave->mes << "/" << clave->anio;
		salida<<" Referencia: "<<this->obtenerReferencia()<<endl;
	}



///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveVariable
// Descripcion: Implementa claves de longitud variable.
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveVariable::ClaveVariable(const string& clave, unsigned int referencia,
                                                                int hijoDer)
	{
		//Se trunca el nombre si es de mas de longMaxCadena caracteres
//		if (clave.size()> ClaveCadena::longMaxCadena)
//			clave.erase(ClaveCadena::longMaxCadena);

		this->setValor(new string(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = (clave.size()+1)*sizeof(char) + 2*sizeof(unsigned int);
	}

	ClaveVariable::~ClaveVariable()
	{
		delete (string*)this->getValor();
	}


	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
	Clave* ClaveVariable::copiar()
	{
		string* clave = (string*)this->getValor();

		return new ClaveVariable(*clave,this->obtenerReferencia(),this->getHijoDer());
	}

	char ClaveVariable::comparar(Clave* otraClave)
	{
		string* palabraOtra = (string*) ((ClaveVariable*)otraClave)->getValor();
		string* palabraThis = (string*) this->getValor();

		/*
		for(unsigned int i=0;i<palabraOtra->length();i++)
			*(palabraOtra + i) = tolower((*palabraOtra)[i]);

		for(unsigned int j=0;j<palabraThis->length();j++)
			*(palabraThis + j) = tolower((*palabraThis)[j]);
		*/

		char resultado = palabraThis->compare(*palabraOtra);

		return resultado;
	}


	void ClaveVariable::imprimir(ostream& salida)
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
// Nombre: ClaveCompuesta
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

