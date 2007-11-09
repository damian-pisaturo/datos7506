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

	ClaveEntera::ClaveEntera(int clave, unsigned int referencia, unsigned int hijoDer)
	{
		this->setValor(new int(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(int) + Tamanios::TAMANIO_REFERENCIA;
		//Si la clave se insertara en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
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
		return new ClaveEntera(clave, this->getReferencia(), this->getHijoDer());
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
		salida<<" Referencia: "<<this->getReferencia()<<endl;
	}
	
	bool ClaveEntera::operator < (const Clave& clave) const {
		
		int estaClave = *((int*)this->getValor());
		int paramClave = *((int*)clave.getValor());
		return (estaClave < paramClave);
		
	}

	bool ClaveEntera::operator == (const Clave& clave) const {
		
		int estaClave = *((int*)this->getValor());
		int paramClave = *((int*)clave.getValor());
		return (estaClave == paramClave);
		
	}
	
	void ClaveEntera::setValor(void* nuevoValor) {
		
		if (this->valor) delete (int*)this->valor;
		this->valor = nuevoValor;
		
	}
	
	unsigned int ClaveEntera::getTamanioValor() const {
		return sizeof(int);
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
	
	ClaveBoolean::ClaveBoolean(bool clave, unsigned int referencia,
											unsigned int hijoDer)
	{
		this->setValor(new bool(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(bool) + Tamanios::TAMANIO_REFERENCIA;
		//Si la clave si insertará en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
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
		return new ClaveBoolean(clave,this->getReferencia(), this->getHijoDer());
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
		bool clave = *(static_cast<bool*>(this->getValor()));

		salida<<"Clave: "<<clave;
		salida<<" Referencia: "<<this->getReferencia()<<endl;
	}
	
	bool ClaveBoolean::operator < (const Clave& clave) const {
			
			bool estaClave = *((bool*)this->getValor());
			bool paramClave = *((bool*)clave.getValor());
			return (estaClave < paramClave);
			
	}

	bool ClaveBoolean::operator == (const Clave& clave) const {
		
		bool estaClave = *((bool*)this->getValor());
		bool paramClave = *((bool*)clave.getValor());
		return (estaClave == paramClave);
		
	}
	
	void ClaveBoolean::setValor(void* nuevoValor) {
		
		if (this->valor) delete (bool*)this->valor;
		this->valor = nuevoValor;
		
	}
	
	unsigned int ClaveBoolean::getTamanioValor() const {
		return sizeof(bool);
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

	ClaveChar::ClaveChar(char clave, unsigned int referencia,
										unsigned int hijoDer)
	{
		this->setValor(new char(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(char) + Tamanios::TAMANIO_REFERENCIA;
		//Si la clave si insertará en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
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
		return new ClaveChar(clave,this->getReferencia(), this->getHijoDer());
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
		salida<<" Referencia: "<<this->getReferencia()<<endl;
	}
	
	bool ClaveChar::operator < (const Clave& clave) const {
			
			char estaClave = *((char*)this->getValor());
			char paramClave = *((char*)clave.getValor());
			return (estaClave < paramClave);
			
	}
	
	bool ClaveChar::operator == (const Clave& clave) const {
		
		char estaClave = *((char*)this->getValor());
		char paramClave = *((char*)clave.getValor());
		return (estaClave == paramClave);
		
	}
	
	void ClaveChar::setValor(void* nuevoValor) {
		
		if (this->valor) delete (char*)this->valor;
		this->valor = nuevoValor;
		
	}

	unsigned int ClaveChar::getTamanioValor() const {
		return sizeof(char);
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

	ClaveShort::ClaveShort(short clave, unsigned int referencia,
											unsigned int hijoDer)
	{
		this->setValor(new short(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(short) + Tamanios::TAMANIO_REFERENCIA;
		//Si la clave si insertará en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
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
		return new ClaveShort(clave,this->getReferencia(), this->getHijoDer());
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
		salida<<" Referencia: "<<this->getReferencia()<<endl;
	}
	
	bool ClaveShort::operator < (const Clave& clave) const {
			
		short estaClave = *((short*)this->getValor());
		short paramClave = *((short*)clave.getValor());
		return (estaClave < paramClave);
			
	}

	bool ClaveShort::operator == (const Clave& clave) const {
		
		short estaClave = *((short*)this->getValor());
		short paramClave = *((short*)clave.getValor());
		return (estaClave == paramClave);
		
	}
	
	void ClaveShort::setValor(void* nuevoValor) {
		
		if (this->valor) delete (short*)this->valor;
		this->valor = nuevoValor;
		
	}
	
	unsigned int ClaveShort::getTamanioValor() const {
		return sizeof(short);
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

	ClaveReal::ClaveReal(float clave, unsigned int referencia,
										unsigned int hijoDer)
	{
		this->setValor(new float(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(float) + Tamanios::TAMANIO_REFERENCIA;
		//Si la clave si insertará en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
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
		float clave = *((float*)this->getValor());
		return new ClaveReal(clave,this->getReferencia(), this->getHijoDer());
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
		salida<<" Referencia: "<<this->getReferencia()<<endl;
	}
	
	bool ClaveReal::operator < (const Clave& clave) const {
			
		float estaClave = *((float*)this->getValor());
		float paramClave = *((float*)clave.getValor());
		return (estaClave < paramClave);
		
	}

	bool ClaveReal::operator == (const Clave& clave) const {
		
		float estaClave = *((float*)this->getValor());
		float paramClave = *((float*)clave.getValor());
		return (estaClave == paramClave);
		
	}
	
	void ClaveReal::setValor(void* nuevoValor) {
		
		if (this->valor) delete (float*)this->valor;
		this->valor = nuevoValor;
		
	}
	
	unsigned int ClaveReal::getTamanioValor() const {
		return sizeof(float);
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

	ClaveFecha::ClaveFecha(const TFECHA &clave, unsigned int referencia,
											unsigned int hijoDer)
	{
		this->setValor(new TFECHA(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = sizeof(fecha) + Tamanios::TAMANIO_REFERENCIA;
		//Si la clave si insertará en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
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
		return new ClaveFecha(*clave, this->getReferencia(), this->getHijoDer());
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
		salida<<" Referencia: "<<this->getReferencia()<<endl;
	}

	bool ClaveFecha::operator < (const Clave& clave) const {
			
		fecha* estaClave = (fecha*)this->getValor();
		fecha* paramClave = (fecha*)clave.getValor();
		return ((estaClave->anio < paramClave->anio) ||
		((estaClave->anio == paramClave->anio)&&(estaClave->mes < paramClave->mes)) ||
		((estaClave->anio == paramClave->anio)&&(estaClave->mes == paramClave->mes) &&
		(estaClave->dia < paramClave->dia)));
		
	}

	bool ClaveFecha::operator == (const Clave& clave) const {
		
		fecha* estaClave = (fecha*)this->getValor();
		fecha* paramClave = (fecha*)clave.getValor();
		return ((estaClave->anio == paramClave->anio) &&
				(estaClave->mes == paramClave->mes) &&
				(estaClave->dia == paramClave->dia));
		
	}
	
	void ClaveFecha::setValor(void* nuevoValor) {
		
		if (this->valor) delete (fecha*)this->valor;
		this->valor = nuevoValor;
		
	}
	
	unsigned int ClaveFecha::getTamanioValor() const {
		return sizeof(fecha);
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
														unsigned int hijoDer)
	{			
		this->setValor(new string(clave));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		//El tamaño de esta clave esta compuesto por la cantidad de caracteres,
		//el byte utilizado para indicar el fin de la cadena y
		//los bytes utilizados por la referencia.
		this->tamanio = (clave.size()+1)*sizeof(char) /*+ Tamanios::TAMANIO_LONGITUD_CADENA*/ + Tamanios::TAMANIO_REFERENCIA;
		//Si la clave si insertará en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
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

		return new ClaveVariable(*clave,this->getReferencia(),this->getHijoDer());
	}

	char ClaveVariable::comparar(Clave* otraClave)
	{
		string* palabraOtra = (string*) otraClave->getValor();
		string* palabraThis = (string*) this->getValor();

		
		for(unsigned int i=0; i < palabraOtra->length(); i++)
			(*palabraOtra)[i] = tolower((*palabraOtra)[i]);

		for(unsigned int j=0; j < palabraThis->length(); j++)
			(*palabraThis)[j] = tolower((*palabraThis)[j]);
		
		return palabraThis->compare(*palabraOtra);
	}


	void ClaveVariable::imprimir(ostream& salida)
	{
		string* clave = static_cast<string*>(this->getValor());

		salida<<"Clave: "<<*clave;
		salida<<" Referencia: "<<this->getReferencia()<<endl;
	}
	
	bool ClaveVariable::operator < (const Clave& clave) const {
			
		string* palabraOtra = (string*) clave.getValor();
		string* palabraThis = (string*) this->getValor();

		for(unsigned int i=0; i < palabraOtra->length(); i++)
			(*palabraOtra)[i] = tolower((*palabraOtra)[i]);

		for(unsigned int j=0; j < palabraThis->length(); j++)
			(*palabraThis)[j] = tolower((*palabraThis)[j]);
		
		return (*palabraThis < *palabraOtra);
			
	}

	bool ClaveVariable::operator == (const Clave& clave) const {
		
		string* palabraOtra = (string*) clave.getValor();
		string* palabraThis = (string*) this->getValor();

		for(unsigned int i=0; i < palabraOtra->length(); i++)
			(*palabraOtra)[i] = tolower((*palabraOtra)[i]);

		for(unsigned int j=0; j < palabraThis->length(); j++)
			(*palabraThis)[j] = tolower((*palabraThis)[j]);
		
		return (*palabraThis == *palabraOtra);
		
	}
	
	void ClaveVariable::setValor(void* nuevoValor) 
	{		
		if (this->valor) delete (string*)this->valor;
		this->valor = nuevoValor;		
	}
	
	unsigned int ClaveVariable::getTamanioValor() const
	{
		return (((string*)this->valor)->size()+1)*sizeof(char);
	}
	
	void* ClaveVariable::getValor() const
	{
		return (void*)(((string*)this->valor)->c_str());
	}

///////////////////////////////////////////////////////////////////////////
// Clase
//-------------------------------------------------------------------------
// Nombre: ClaveCompuesta
// Descripcion: Implementa claves compuestas (lista de claves).
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ClaveCompuesta::ClaveCompuesta(const ListaClaves& listaClaves,
										unsigned int referencia,
										unsigned int hijoDer)
	{
		this->setValor(new ListaClaves(listaClaves));
		this->setReferencia(referencia);
		this->setHijoDer(hijoDer);
		this->tamanio = Tamanios::TAMANIO_REFERENCIA + this->calcularTamanioValoresClaves(listaClaves);	
		//Si la clave si insertará en un nodo interno
		//agrego el tamaño de la referencia al hijo derecho
		if (hijoDer != 0) this->tamanio += Tamanios::TAMANIO_REFERENCIA;
	}

	ClaveCompuesta::~ClaveCompuesta()
	{
		this->eliminarLista();
	}

	///////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////
	Clave* ClaveCompuesta::copiar()
	{
		ListaClaves* listaClaves = this->getListaClaves();
		ListaClaves listaCopia;
		Clave* claveCopia;
		
		for (ListaClaves::const_iterator iter = listaClaves->begin();
			iter != listaClaves->end(); ++iter) {
			
			claveCopia = (*iter)->copiar();
			listaCopia.push_back(claveCopia);
		}
		
		return new ClaveCompuesta(listaCopia,this->getReferencia(),this->getHijoDer());
	}

	char ClaveCompuesta::comparar(Clave* otraClave)
	{
		ListaClaves* listaClavesThis = this->getListaClaves();
		ListaClaves* listaClavesParam = (static_cast<ClaveCompuesta*>(otraClave))->getListaClaves();
		
		for (ListaClaves::const_iterator iterThis = listaClavesThis->begin(),
			iterParam = listaClavesParam->begin();
			(iterThis != listaClavesThis->end()) &&
			(iterParam != listaClavesParam->end());
			++iterThis, ++iterParam) {
			
			if (*(*iterThis) < *(*iterParam)) return -1;
			else if (*(*iterParam) < *(*iterThis)) return 1;
		}
		
		return 0;
	}


	void ClaveCompuesta::imprimir(ostream& salida)
	{
		ListaClaves* listaClaves = this->getListaClaves();

		salida << "ClaveCompuesta: " << endl;
		
		for (ListaClaves::iterator iter = listaClaves->begin();
			iter != listaClaves->end(); ++iter)
			(*iter)->imprimir(salida);
		
		salida << " Referencia: " << this->getReferencia() << endl;
	}
	
	bool ClaveCompuesta::operator < (const Clave& clave) const {
			
		ListaClaves* listaClavesThis = this->getListaClaves();
		ListaClaves* listaClavesParam = ((ClaveCompuesta&)clave).getListaClaves();
		
		for (ListaClaves::const_iterator iterThis = listaClavesThis->begin(),
			iterParam = listaClavesParam->begin();
			(iterThis != listaClavesThis->end()) &&
			(iterParam != listaClavesParam->end());
			++iterThis, ++iterParam) {
			
			if (*(*iterThis) < *(*iterParam)) return true;
			else if (*(*iterParam) < *(*iterThis)) return false;
		}
		
		return false;
			
	}

	bool ClaveCompuesta::operator == (const Clave& clave) const {
		
		ListaClaves* listaClavesThis = this->getListaClaves();
		ListaClaves* listaClavesParam = ((ClaveCompuesta&)clave).getListaClaves();
		
		for (ListaClaves::const_iterator iterThis = listaClavesThis->begin(),
			iterParam = listaClavesParam->begin();
			(iterThis != listaClavesThis->end()) &&
			(iterParam != listaClavesParam->end());
			++iterThis, ++iterParam)
			if (!(*(*iterThis) == *(*iterParam))) return false;
		
		return true;
		
	}
	
	void ClaveCompuesta::setValor(void* nuevoValor) {
		
		if (this->valor) this->eliminarLista();
		this->valor = nuevoValor;
		
	}
	
	unsigned int ClaveCompuesta::getTamanioValor() const {
		return this->calcularTamanioValoresClaves(*(this->getListaClaves()));
	}
	
	ListaClaves* ClaveCompuesta::getListaClaves() const {
		return (ListaClaves*)this->getValor();
	}
	
	void ClaveCompuesta::eliminarLista() {
		
		ListaClaves* listaClaves = this->getListaClaves();
		
		for (ListaClaves::iterator iter = listaClaves->begin();
			iter != listaClaves->end(); ++iter)
			delete *iter; //Libero la memoria reservada para cada clave
		
		delete listaClaves; //Libero la memoria reservada para la lista de claves
		
		this->valor = NULL;
		
	}
	
	unsigned int ClaveCompuesta::calcularTamanioValoresClaves(const ListaClaves& listaClaves) const {
		
		unsigned int tamanioValores = 0;
		
		for (ListaClaves::const_iterator iter = listaClaves.begin();
			iter != listaClaves.end(); ++iter)
			tamanioValores += (*iter)->getTamanioValor();
		
		return tamanioValores;
		
	}
	
	int ClaveCompuesta::getCantidadClaves() const {
		
		ListaClaves* lista = getListaClaves();
		
		return lista->size();
	}
	
	void** ClaveCompuesta::getValorParaHash() const {
		ListaClaves* listaClaves = this->getListaClaves();
		ListaClaves::iterator iter;
		int cantidadClaves = getCantidadClaves();
		void** clave = new void*[cantidadClaves];
		int i = 0;
		
		for (iter = listaClaves->begin(); iter != listaClaves->end(); ++iter){
			clave[i] = (*iter)->getValor();
			i++;
		}	
		return clave;
	}
	
