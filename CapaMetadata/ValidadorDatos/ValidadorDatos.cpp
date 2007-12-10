///////////////////////////////////////////////////////////////////////////
//	Archivo   : ValidadorDatos.cpp
//  Namespace : CapaMetadata
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de la clase ValidadorDatos.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "ValidadorDatos.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ValidadorDatos 
//	(Valida los registros y claves cuyos tipos y valores corresponden a
//	las estructuras pasadas como argumento de los metodos)
///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////////
	ValidadorDatos::ValidadorDatos()
		{ }
	
	ValidadorDatos::~ValidadorDatos()
		{ }
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos publicos
	///////////////////////////////////////////////////////////////////////////
	char ValidadorDatos::validarRegistro(const ListaTipos& tiposDatos, const ListaValoresAtributos& valoresAtributos) const
	{
		char resultado = ResultadosMetadata::OK;
		bool registroValido = true;
		
		ListaTipos::const_iterator itTipos;
		ListaValoresAtributos::const_iterator itValores;
		
		for (itTipos = tiposDatos.begin(), itValores = valoresAtributos.begin(); 
			(itTipos != tiposDatos.end()) && (registroValido); ++itTipos, ++itValores){
			
			if (*itValores != VAL_VACIO){
				switch(*itTipos){			
					case TipoDatos::TIPO_ENTERO: 
						resultado = this->validarEntero(*itValores); 
						break;
					case TipoDatos::TIPO_SHORT: 
						resultado = this->validarShort(*itValores);
						break;
					case TipoDatos::TIPO_CHAR:
						resultado = this->validarChar(*itValores); 
						break;
					case TipoDatos::TIPO_FLOAT: 
						resultado = ResultadosMetadata::OK;
						break;
					case TipoDatos::TIPO_FECHA: 
						resultado = this->validarFecha(*itValores);
						break;
					case TipoDatos::TIPO_STRING: 
						resultado = this->validarString(*itValores);
						break;
					case TipoDatos::TIPO_BOOL: 
						resultado = this->validarBool(*itValores);
						break;
				}
			}
			
			if (resultado != ResultadosMetadata::OK) registroValido = false;
			
		}
		
		return resultado;		
	}
	
	char ValidadorDatos::validarClave(const ListaTipos& tiposDatos, const ListaValoresClaves& valoresClaves) const
	{
		return this->validarRegistro(tiposDatos,valoresClaves);
	}
	
	char ValidadorDatos::validarNombresAtributos(const ListaNombresAtributos& nombresAtributos, const MapaValoresAtributos& mapValAtributos) const
	{
		char resultado = ResultadosMetadata::OK;
		bool nombreEncontrado = true;
		
		MapaValoresAtributos::const_iterator iterMap;
		ListaNombresAtributos::const_iterator iterNA;
		
			
		for (iterMap = mapValAtributos.begin(); (iterMap != mapValAtributos.end()) && (nombreEncontrado);
			++iterMap){
			
			nombreEncontrado = false;
			for (iterNA = nombresAtributos.begin(); (iterNA != nombresAtributos.end()) && (!nombreEncontrado);
				++iterNA)
				if (*iterNA == iterMap->first) nombreEncontrado = true;			
		}
			
		
		if (!nombreEncontrado) resultado = ResultadosMetadata::NOM_ATRIBUTO_INVALIDO;
		
		return resultado;
	}
	
	char ValidadorDatos::validarNombresClaves(const ListaNombresAtributos& nombresAtributos, const ListaNombresClaves& nombresClaves) const
	{
		char resultado    = ResultadosMetadata::OK;
		bool existeNombre = true;
		
		ListaNombresClaves::const_iterator iterNC;
		ListaNombresAtributos::const_iterator iterNA;
		
		for (iterNC = nombresClaves.begin(); (iterNC != nombresClaves.end()) && (existeNombre); ++iterNC){
			existeNombre = false;
			for (iterNA = nombresAtributos.begin(); (iterNA != nombresAtributos.end()) && (!existeNombre);
				++iterNA){
				if (*iterNA == *iterNC) existeNombre = true;				
			}
		}
		
		if (!existeNombre) resultado = ResultadosMetadata::NOM_CLAVE_INVALIDA;
		
		return resultado;		
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////////

	char ValidadorDatos::validarEntero(const string& valorEntero) const
	{
		char resultado = ResultadosMetadata::OK;
		stringstream conversor;
		string numeroEntero;
		string::size_type posNoNumero = valorEntero.find_first_not_of(VALORES_NUMERICOS);
		string::size_type posSignoMenos = 0, posPrimerNum = 0;
		
		if (posNoNumero == string::npos){ // El valor entero no contiene caracteres alfabeticos
			posSignoMenos = valorEntero.find_first_of(SIGNO_MENOS);
			
			if (posSignoMenos != string::npos){ // Valor entero negativo
				conversor << INT_MIN;			
				posPrimerNum = valorEntero.find_first_not_of('0', 1);
				numeroEntero += valorEntero[0];
				numeroEntero += valorEntero.substr(posPrimerNum);
			}else{ // Valor entero positivo
				conversor << INT_MAX;
				posPrimerNum = valorEntero.find_first_not_of('0');
				numeroEntero = valorEntero.substr(posPrimerNum);
			}
			
			if ( (numeroEntero.size() > conversor.str().size())  || 
			   (numeroEntero.size() == conversor.str().size()) && (numeroEntero > conversor.str()) )
				resultado = ResultadosMetadata::ENTERO_INVALIDO;
		
			
		}else resultado = ResultadosMetadata::ENTERO_INVALIDO;
				
		return resultado;
	}
		
	char ValidadorDatos::validarShort(const string& valorShort) const
	{
		char resultado = ResultadosMetadata::OK;
		stringstream conversor;
		string numeroShort;
		string::size_type posNoNumero = valorShort.find_first_not_of(VALORES_NUMERICOS);
		string::size_type posSignoMenos = 0, posPrimerNum = 0;
		
		if (posNoNumero == string::npos){ // El valor entero no contiene caracteres alfabeticos
			posSignoMenos = valorShort.find_first_of(SIGNO_MENOS);
			
			if (posSignoMenos != string::npos){ // Valor entero negativo
				conversor << SHRT_MIN;			
				posPrimerNum = valorShort.find_first_not_of('0', 1);
				numeroShort += valorShort[0];
				numeroShort += valorShort.substr(posPrimerNum);
			}else{ // Valor entero positivo
				conversor << SHRT_MAX;
				posPrimerNum = valorShort.find_first_not_of('0');
				numeroShort = valorShort.substr(posPrimerNum);
			}
			
			if ( (numeroShort.size() > conversor.str().size())  || 
			   (numeroShort.size() == conversor.str().size()) && (numeroShort > conversor.str()) )
				resultado = ResultadosMetadata::SHORT_INVALIDO;
		
			
		}else resultado = ResultadosMetadata::SHORT_INVALIDO;
				
		return resultado;
	}

		
	char ValidadorDatos::validarChar(const string& valorChar) const
	{
		char resultado = ResultadosMetadata::OK;
		
		if (valorChar.size() > 1) 
			resultado = ResultadosMetadata::CHAR_INVALIDO;
				
		return resultado;
	}
	
	char ValidadorDatos::validarBool(string valorBool) const
	{
		char resultado = ResultadosMetadata::OK;
		
		transform(valorBool.begin(), valorBool.end(), valorBool.begin(), (int (*)(int))std::toupper);
		
		if ( (valorBool != VAL_TRUE) || (valorBool != VAL_FALSE) ) 
			resultado = ResultadosMetadata::BOOL_INVALIDO;
		
		return resultado;
	}
		
	char ValidadorDatos::validarFecha(const string& fecha) const
	{
		char resultado = ResultadosMetadata::OK;
		
		if (fecha.find_first_not_of(VALORES_NUMERICOS_POS) == string::npos ){ // La fecha contiene solo caracteres numericos.
			if ( (fecha.size() != 8) || (fecha.substr(4,2) > "12") || (fecha.substr(6,2) > "31") )  
				resultado = ResultadosMetadata::FECHA_INVALIDA; 	
		}else resultado = ResultadosMetadata::FECHA_INVALIDA; // Le fecha contiene caracteres no numericos.
		 
		return resultado;
	}
	
	char ValidadorDatos::validarString(const string& s) const
	{
		char resultado = ResultadosMetadata::OK;
		
		if (s.size() > UCHAR_MAX)
			resultado = ResultadosMetadata::STRING_INVALIDO;
		
		return resultado;
	}
