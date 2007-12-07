///////////////////////////////////////////////////////////////////////////
//	Archivo   : ParserDefiniciones.cpp
//  Namespace : Common
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clase ParserOperaciones.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefanía;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "ParserDefiniciones.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ParserDefiniciones 
//	(Parsea el archivo de definiciones y arma las estructuras necesarias
// 	para ser consultadas desde el DefinitionsManager)
///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	ParserDefiniciones::ParserDefiniciones(const string& nombreArchivo)
	{
		this->archivo.open(nombreArchivo.c_str(), ios_base::in);
	}
	
	ParserDefiniciones::~ParserDefiniciones()
	{ 
		this->archivo.close();		
	}
	
	void ParserDefiniciones::parsearDefiniciones()
	{
		string linea("");
		DefinicionTipo* tipo = NULL;
		
		while ( (!this->archivo.fail()) && (linea != HEADER_FIN) ){
			this->leerLinea(linea);
			if (linea == HEADER_TIPO){
				
				tipo = new DefinicionTipo();				
				parsearTipo(tipo->nombreTipo);
				
				this->leerLinea(linea);
				
				while ( (!this->archivo.fail()) && (linea != HEADER_FIN) && (linea != HEADER_TIPO) ){				
					
					if (linea == HEADER_ORGREG) this->parsearOrgRegistro(tipo->tipoOrganizacion);
					else if (linea == HEADER_ATRIBUTOS) this->parsearAtributos(tipo->nombresAtributos, tipo->tiposAtributos);
					else if (linea == HEADER_INDICES) this->parsearIndices(tipo->datosIndicesGriegos, tipo->datosIndicesRomanos);
					else if (linea == HEADER_INTEGRIDAD) this->parsearIntegridad();
					
					this->leerLinea(linea);
				}			

				this->definicionesTipos.push_back(*tipo);
				delete tipo;
				
				if (linea != HEADER_FIN) this->retrocederLinea(linea);
			}
		}
	}
	
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////
	void ParserDefiniciones::leerLinea(string& linea)
	{
		getline(this->archivo,linea);
		this->aMayuscula(linea);
	}
	
	void ParserDefiniciones::aMayuscula(string& linea)
	{
		transform(linea.begin(), linea.end(), linea.begin(), (int (*)(int))std::toupper);
	}
	
	void ParserDefiniciones::retrocederLinea(const string& linea)
	{		
		string::size_type longUltimaLinea = linea.length() + 1;
		
		for (unsigned char i = 0; i < longUltimaLinea; i++)
			this->archivo.unget();
	}	
		
	void ParserDefiniciones::parsearTipo(string& nombreTipo)
	{
		string::size_type posIgual      = 0,
						  posPrimerChar = 0,
						  posUltimoChar = 0;
		bool continuarTipo = false;
		string linea(""), valor("");

		
		do{
			this->leerLinea(linea);
			this->aMayuscula(linea);
			posPrimerChar = linea.find_first_not_of(C_VACIO, 0);
			continuarTipo = (linea != HEADER_ATRIBUTOS) && (linea != HEADER_INDICES) && (linea != HEADER_INTEGRIDAD)
						&& (linea != HEADER_ORGREG);
		}while ( ((linea[posPrimerChar] == C_COMENTARIO) || (linea.find(NOMBRE_TIPO, posPrimerChar) == string::npos)) && 
				(continuarTipo) && (!this->archivo.fail()) );
		
		if (!this->archivo.fail() && (continuarTipo)){	
			
			// Obtencion del valor asociado al atributo.
			posIgual      = linea.find(C_IGUAL, posPrimerChar);
			posPrimerChar = linea.find_first_not_of(C_VACIO, posIgual + 1);
			posUltimoChar = linea.find_last_not_of(C_VACIO);
			valor         = linea.substr(posPrimerChar, posUltimoChar);	
			
			nombreTipo = valor; 
		}
	}
	
	void ParserDefiniciones::parsearOrgRegistro(unsigned char& orgRegistros)
	{
		string::size_type posIgual      = 0,
						  posPrimerChar = 0,
						  posUltimoChar = 0;
		bool continuarOrgRegistro = true;
		string linea(""), valor("");
		
		do{
			this->leerLinea(linea);
			continuarOrgRegistro = (linea != HEADER_ATRIBUTOS) && (linea != HEADER_INDICES) && (linea != HEADER_INTEGRIDAD);
			if (continuarOrgRegistro) posPrimerChar = linea.find_first_not_of(C_VACIO, 0);
		}while ( ( (linea[posPrimerChar] == C_COMENTARIO) || (linea.find(ORG_REGISTROS, posPrimerChar) == string::npos)) && 
				(continuarOrgRegistro) && (!this->archivo.fail()) );
		
		if (!this->archivo.fail() && (continuarOrgRegistro)){				
			// Obtencion del valor asociado al atributo.
			posIgual      = linea.find(C_IGUAL, posPrimerChar);		
			posPrimerChar = linea.find_first_not_of(C_VACIO, posIgual + 1);
			posUltimoChar = linea.find_last_not_of(C_VACIO);
			valor         = linea.substr(posPrimerChar, posUltimoChar);

			if (valor == REG_RVB)
				orgRegistros = TipoDatos::TIPO_VARIABLE;
			else if (valor == REG_RF)
				orgRegistros = TipoDatos::TIPO_FIJO;				
		}		
	}
	
	void ParserDefiniciones::parsearAtributos(ListaNombresAtributos* listaNombresAtributos, 
			VectorTiposAtributos* vTiposAtributos)
	{
		unsigned char tipoAtributo        = 0;
		string::size_type posIgual        = 0, 
		                  posPrimerChar   = 0, 
		                  posUltimoChar   = 0;
		string linea(""), valor(""), atributo("");
		bool continuarAtributos = (linea != HEADER_ORGREG) && (linea != HEADER_INDICES) 
									&& (linea != HEADER_INTEGRIDAD);
		
		while (continuarAtributos){			
			do{
				this->leerLinea(linea);
				continuarAtributos = (linea != HEADER_ORGREG) && (linea != HEADER_INDICES) 
										&& (linea != HEADER_INTEGRIDAD);
				if (continuarAtributos) posPrimerChar = linea.find_first_not_of(C_VACIO, 0);
				
			}while (  (linea[posPrimerChar] == C_COMENTARIO) || 
				    ( (linea.find(ATRIBUTO_NOMBRE, posPrimerChar) == string::npos) && 
					  (linea.find(ATRIBUTO_TIPO, posPrimerChar) == string::npos) ) && 
					  (continuarAtributos) && (!this->archivo.fail()) );
			
			if ( (!this->archivo.fail()) && (continuarAtributos) ){		
				// Obtencion del nombre del atributo.
				posIgual      = linea.find(C_IGUAL, posPrimerChar);
				posUltimoChar = linea.find_last_not_of(C_VACIO, posIgual - 1);
				atributo      = linea.substr(posPrimerChar, posUltimoChar - posPrimerChar + 1);	
				
				// Obtencion del valor asociado a ese atributo.
				posPrimerChar = linea.find_first_not_of(C_VACIO, posIgual + 1);
				posUltimoChar = linea.find_last_not_of(C_VACIO);
				valor         = linea.substr(posPrimerChar, posUltimoChar - posPrimerChar + 1);			
				
				if (atributo == ATRIBUTO_NOMBRE)
					listaNombresAtributos->push_back(valor);
				else if (atributo == ATRIBUTO_TIPO){					
					if (valor == T_ENTERO)
						tipoAtributo = TipoDatos::TIPO_ENTERO;
					else if (valor == T_SHORT)
						tipoAtributo = TipoDatos::TIPO_SHORT;
					else if (valor == T_CHAR)
						tipoAtributo = TipoDatos::TIPO_CHAR;
					else if (valor == T_BOOL)
						tipoAtributo = TipoDatos::TIPO_BOOL;
					else if (valor == T_FECHA)
						tipoAtributo = TipoDatos::TIPO_FECHA;
					else if (valor == T_STRING)
						tipoAtributo = TipoDatos::TIPO_STRING;
					else if (valor == T_REAL)
						tipoAtributo = TipoDatos::TIPO_FLOAT;					

					vTiposAtributos->push_back(tipoAtributo);
				}
			}
		}
		
		this->retrocederLinea(linea);
	}
	
	void ParserDefiniciones::parsearIndices(DatosIndices* datosIndicesGriegos, DatosIndices* datosIndicesRomanos)
	{
		unsigned char tipoIndice = 0;
		unsigned short tamNodo   = 0;
		ListaNombresClaves* listaNombresClaves = NULL, *l = NULL;
		DatosIndices* datosIndices = NULL;
		string::size_type posIgual        = 0, 
		                  posPrimerChar   = 0, 
		                  posUltimoChar   = 0;
		string linea(""), valor(""), atributo("");		
		bool continuarIndices = (linea != HEADER_ORGREG) && (linea != HEADER_ATRIBUTOS) 
								&& (linea != HEADER_INTEGRIDAD);
		bool continuarDefiniciones = true, continuarCampoIndice = true;
		
		while (continuarIndices){			
			do{
				this->leerLinea(linea);
				continuarIndices = (linea != HEADER_ORGREG) && (linea != HEADER_ATRIBUTOS) 
									&& (linea != HEADER_INTEGRIDAD);
				if (continuarIndices) posPrimerChar = linea.find_first_not_of(C_VACIO, 0);
			}while ( (linea[posPrimerChar] == C_COMENTARIO) || 
					( (linea.find(HEADER_GRIEGO, posPrimerChar) == string::npos) && (linea.find(HEADER_ROMANO, posPrimerChar) == string::npos))  
					&& (continuarIndices) && (!this->archivo.fail()) );
			
			if ( (!this->archivo.fail()) && (continuarIndices) ){
				
				continuarDefiniciones = true;
				if (linea == HEADER_GRIEGO) datosIndices = datosIndicesGriegos;
				else if (linea == HEADER_ROMANO) datosIndices = datosIndicesRomanos;
				
				while (continuarDefiniciones){
					do{
						this->leerLinea(linea);
						continuarDefiniciones = (linea != HEADER_ORGREG) && (linea != HEADER_ATRIBUTOS) 
											&& (linea != HEADER_INTEGRIDAD) && (linea != HEADER_GRIEGO)
											&& (linea != HEADER_ROMANO);
						if (continuarDefiniciones) posPrimerChar = linea.find_first_not_of(C_VACIO, 0);
					}while ( (linea[posPrimerChar] == C_COMENTARIO) || 
							( (linea.find(TIPO_INDICE, posPrimerChar) == string::npos) && (linea.find(CAMPO_INDICE, posPrimerChar) == string::npos)
							&& (linea.find(TAM_NODO, posPrimerChar) == string::npos) && (linea.find(TAM_BUCKET, posPrimerChar) == string::npos) ) 
							&& (continuarDefiniciones) && (!this->archivo.fail()) );
					
					if ( (!this->archivo.fail()) && (continuarDefiniciones) ){
						// Obtencion del nombre del atributo.
						posIgual      = linea.find(C_IGUAL, posPrimerChar);
						posUltimoChar = linea.find_last_not_of(C_VACIO, posIgual - 1);
						atributo      = linea.substr(posPrimerChar, posUltimoChar - posPrimerChar + 1);	
						
						// Obtencion del valor asociado a ese atributo.
						posPrimerChar = linea.find_first_not_of(C_VACIO, posIgual + 1);
						posUltimoChar = linea.find_last_not_of(C_VACIO);
						valor         = linea.substr(posPrimerChar, posUltimoChar - posPrimerChar + 1);
						
						if (atributo == TIPO_INDICE){
							if (valor == I_HASH) tipoIndice = TipoIndices::HASH;
							else if (valor == I_ARBOL_BS) tipoIndice = TipoIndices::ARBOL_BS;
							else if (valor == I_ARBOL_BP) tipoIndice = TipoIndices::ARBOL_BP;
							
							datosIndices->vTiposIndices->push_back(tipoIndice);
						}else if ( (atributo == TAM_NODO) || (atributo == TAM_BUCKET) ){
							tamNodo = atoi(valor.c_str());
							datosIndices->vTamBloques->push_back(tamNodo);
						}else if (atributo == CAMPO_INDICE){
							
							continuarCampoIndice = true;
							
							listaNombresClaves = new ListaNombresClaves();
							listaNombresClaves->push_back(valor);
											
							while (continuarCampoIndice){
								do{
									this->leerLinea(linea);
									posPrimerChar = linea.find_first_not_of(C_VACIO, 0);
									continuarCampoIndice = (linea != HEADER_ORGREG) 
														&& (linea != HEADER_ATRIBUTOS) 
														&& (linea != HEADER_INTEGRIDAD)
														&& (linea != HEADER_ROMANO) && (linea != HEADER_GRIEGO)
														&& (linea.find(TAM_NODO, posPrimerChar) == string::npos)
														&& (linea.find(TAM_BUCKET, posPrimerChar) == string::npos)
														&& (linea.find(TIPO_INDICE, posPrimerChar) == string::npos);									
								}while ( (linea[posPrimerChar] == C_COMENTARIO) || 
										( (linea.find(CAMPO_INDICE, posPrimerChar) == string::npos)										
										&& (continuarCampoIndice) && (!this->archivo.fail()) ) );
								
								if ( (continuarCampoIndice) && (!this->archivo.fail()) ){								
									// Obtencion del valor asociado al atributo.
									posIgual      = linea.find(C_IGUAL, posPrimerChar);	
									posPrimerChar = linea.find_first_not_of(C_VACIO, posIgual + 1);
									posUltimoChar = linea.find_last_not_of(C_VACIO);
									valor         = linea.substr(posPrimerChar, posUltimoChar - posPrimerChar + 1);		

									listaNombresClaves->push_back(valor);										
								}
							}
							
							this->retrocederLinea(linea);
							l = new ListaNombresClaves(*listaNombresClaves);						
							datosIndices->vNomClaves->push_back(l);
							
							delete listaNombresClaves;
							listaNombresClaves = NULL;
						}
					}
						
					continuarDefiniciones = (linea != HEADER_ORGREG) && (linea != HEADER_ATRIBUTOS) 
										&& (linea != HEADER_INTEGRIDAD) && (linea != HEADER_GRIEGO)
										&& (linea != HEADER_ROMANO);					
				}		
			}
			
			continuarIndices = (linea != HEADER_ORGREG) && (linea != HEADER_ATRIBUTOS) 
								&& (linea != HEADER_INTEGRIDAD);
		}
		
		
		this->retrocederLinea(linea);

	}
	
	void ParserDefiniciones::parsearIntegridad()
	{
		// TODO
	}
