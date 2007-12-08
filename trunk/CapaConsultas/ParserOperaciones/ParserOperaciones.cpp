///////////////////////////////////////////////////////////////////////////
//	Archivo   : ParserOperaciones.cpp
//  Namespace : CapaIndices
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de la clase ParserOperaciones.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#include "ParserOperaciones.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ParserOperaciones 
//	(Parsea el archivo de operaciones, indicando altas, bajas 
//	y modificaciones)
///////////////////////////////////////////////////////////////////////////

ParserOperaciones::ParserOperaciones(const string &nombreArchivo) {
	if (nombreArchivo.size() != 0){ 
		this->archivo.open(nombreArchivo.c_str());
		cin.rdbuf(this->archivo.rdbuf());
	}
	this->numOperacion        = 0;
	this->mapValoresAtributos = NULL;
}

ParserOperaciones::~ParserOperaciones() {
	if (this->archivo.is_open()) 
		this->archivo.close();
	
	if (this->mapValoresAtributos)
		delete this->mapValoresAtributos;
}

int ParserOperaciones::proximaOperacion() {	
	string linea;
	ListaStrings lineaParseada;
	ListaStrings::iterator iter;
	ListaStrings nombresAtributos;
	ListaStrings::iterator iterNombresAtributos;
	ListaStrings valoresAtributos;
	ListaStrings::iterator iterValoresAtributos;
	string tipoOp;
	unsigned char resultado = ResultadosParserOperaciones::FIN_ARCHIVO;
	char operacion = 0;
	
	while ( (!cin.fail()) && (resultado == ResultadosParserOperaciones::FIN_ARCHIVO) ){
		getline(cin, linea, FIN_OPERACION);
		
		reemplazarCaracter(&linea, '\n', SEPARATOR_CAMPOS);
		limpiarInicio(&linea, SEPARATOR_CAMPOS);

		if ( (linea.size() != 0) && (linea[0] != CARACTER_COMENTARIO) ) {
			if (this->mapValoresAtributos){
				delete this->mapValoresAtributos;
				this->mapValoresAtributos = NULL;
			}
			
			this->mapValoresAtributos = new MapaValoresAtributos();
			this->mapValoresAtributos = new MapaValoresAtributos();
			
			this->estructuraCampos.operacion = NO_OPERACION;
			this->estructuraCampos.listaCampos.clear();
			
			this->estructuraConsulta.listaCamposSeleccionados.clear();
			this->estructuraConsulta.listaNombresTipos.clear();
			this->estructuraConsulta.estructuraJoins.listaOperaciones.clear();
			this->estructuraConsulta.estructuraJoins.operacion = NO_OPERACION;
			this->estructuraConsulta.estructuraWhere.listaOperaciones.clear();
			this->estructuraConsulta.estructuraWhere.operacion = NO_OPERACION;
			this->estructuraConsulta.listaOrderBy.clear();
			
			this->nombreTipo = "";
			
			resultado = ResultadosParserOperaciones::SINTAXIS_CORRECTA;
			this->numOperacion++;
			
			tipoOp = linea.substr(0, linea.find(SEPARATOR_CAMPOS));
			mayus(&tipoOp);
			
			if (tipoOp == ALTA) this->tipoOperacion = OperacionesCapas::CONSULTAS_ALTA;
			else if (tipoOp == BAJA) this->tipoOperacion = OperacionesCapas::CONSULTAS_BAJA;
			else if (tipoOp == MODIFICACION) this->tipoOperacion = OperacionesCapas::CONSULTAS_MODIFICACION;
			else if (tipoOp == CONSULTA) this->tipoOperacion = OperacionesCapas::CONSULTAS_CONSULTA;
			else resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_TIPO_OPERACION;
			
			if (resultado == ResultadosParserOperaciones::SINTAXIS_CORRECTA){
				switch (this->tipoOperacion){
					case OperacionesCapas::CONSULTAS_ALTA: {
						lineaParseada = parsearString(linea, SEPARATOR_CAMPOS, '(', ')');
						iter = ++lineaParseada.begin();
						
						if ( *mayus(&(*(iter++))) != "INTO" ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_INTO;
							break;
						}
						
						this->nombreTipo = *(iter++);
						mayus(&(this->nombreTipo));
						
						if ( (((*iter)[0]) != '(') || (((*iter)[(*iter).size() - 1]) != ')') ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_PARENTESIS;
							break;
						}
			
						removerCaracter(&(*iter), '(');
						removerCaracter(&(*iter), ')');
						nombresAtributos = parsearString(*mayus(&(*(iter++))), SEPARATOR_ATRIBUTOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
						if ( *mayus(&(*(iter++))) != "VALUES" ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_VALUES;
							break;
						}
	
						if ( (((*iter)[0]) != '(') || (((*iter)[(*iter).size() - 1]) != ')') ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_PARENTESIS;
							break;
						}
	
						removerCaracter(&(*iter), '(');
						removerCaracter(&(*iter), ')');
						valoresAtributos = parsearString(*(iter++), SEPARATOR_ATRIBUTOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
						if (nombresAtributos.size() != valoresAtributos.size()){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_CANT_VALORES;
							break;
						}
		
						for (iterNombresAtributos = nombresAtributos.begin(), iterValoresAtributos = valoresAtributos.begin(); iterNombresAtributos != nombresAtributos.end() && iterValoresAtributos != valoresAtributos.end(); ++iterValoresAtributos, ++iterNombresAtributos){
							removerCaracter(&(*iterValoresAtributos), CARACTER_AGRUPADOR);
							(*this->mapValoresAtributos)[*iterNombresAtributos] = *iterValoresAtributos;
						}
	
					} break;
					
					case OperacionesCapas::CONSULTAS_BAJA: {
						lineaParseada = parsearString(linea, SEPARATOR_CAMPOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
						iter = ++lineaParseada.begin();
						
						if ( *mayus(&(*(iter++))) != "FROM" ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_FROM;
							break;
						}
						
						this->nombreTipo = *(iter++);
						mayus(&(this->nombreTipo));
						
						if ( *mayus(&(*(iter++))) != "WHERE" ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_WHERE;
							break;
						}
						
						while (iter != lineaParseada.end()){
							NodoListaCampos nodo;
							nodo.nombreCampo = *(iter++);
							mayus(&(nodo.nombreCampo));
							if (*iter == "<") nodo.operacion = ExpresionesLogicas::MENOR;
							else if (*iter == "<=") nodo.operacion = ExpresionesLogicas::MENOR_O_IGUAL;
							else if (*iter == ">") nodo.operacion = ExpresionesLogicas::MAYOR;
							else if (*iter == ">=") nodo.operacion = ExpresionesLogicas::MAYOR_O_IGUAL;
							else if (*iter == "=") nodo.operacion = ExpresionesLogicas::IGUAL;
							else{
								resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
								break;
							}
			
							++iter;
							nodo.valorCampo = *(iter++);
							removerCaracter(&nodo.valorCampo, CARACTER_AGRUPADOR);
							this->estructuraCampos.listaCampos.push_back(nodo);
					
							if (iter != lineaParseada.end()){
								if (*mayus(&(*iter)) == "AND") operacion = ExpresionesLogicas::AND;
								else if (*iter == "OR") operacion = ExpresionesLogicas::OR;
								else{
									resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
									break;
								}
								++iter;
								if ( (this->estructuraCampos.operacion != NO_OPERACION) && (this->estructuraCampos.operacion != operacion) ){
									resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_AND_OR;
									break;
								}
								else this->estructuraCampos.operacion = operacion;
							}
						}
						
					} break;
					
					case OperacionesCapas::CONSULTAS_MODIFICACION: {
						lineaParseada = parsearString(linea, SEPARATOR_CAMPOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
						iter = ++lineaParseada.begin();
						
						this->nombreTipo = *(iter++);
						mayus(&(this->nombreTipo));
						
						if ( *mayus(&(*(iter++))) != "SET" ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_SET;
							break;
						}
						
						string aux(1, SEPARATOR_ATRIBUTOS);
						do {
							nombresAtributos.push_back(*mayus(&(*(iter++))));

							if (*(iter++) != "="){
								resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
								break;
							}
							
							valoresAtributos.push_back(*(iter++));
						} while (*(iter++) == aux);
						
						//Una formalidad, porque así como está ahora el código si llegara a pasar esto, rompería...
						if (nombresAtributos.size() != valoresAtributos.size()){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_CANT_VALORES;
							break;
						}
						
						for (iterNombresAtributos = nombresAtributos.begin(), iterValoresAtributos = valoresAtributos.begin(); iterNombresAtributos != nombresAtributos.end() && iterValoresAtributos != valoresAtributos.end(); ++iterValoresAtributos, ++iterNombresAtributos){
							removerCaracter(&(*iterValoresAtributos), CARACTER_AGRUPADOR);
							(*this->mapValoresAtributos)[*iterNombresAtributos] = *iterValoresAtributos;
						}
						
						--iter;
						
						if ( (iter != lineaParseada.end()) && (*mayus(&(*(iter++))) != "WHERE") ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_WHERE;
							break;
						}
						
						while (iter != lineaParseada.end()){
							NodoListaCampos nodo;
							nodo.nombreCampo = *(iter++);
							mayus(&(nodo.nombreCampo));
							if (*iter == "<") nodo.operacion = ExpresionesLogicas::MENOR;
							else if (*iter == "<=") nodo.operacion = ExpresionesLogicas::MENOR_O_IGUAL;
							else if (*iter == ">") nodo.operacion = ExpresionesLogicas::MAYOR;
							else if (*iter == ">=") nodo.operacion = ExpresionesLogicas::MAYOR_O_IGUAL;
							else if (*iter == "=") nodo.operacion = ExpresionesLogicas::IGUAL;
							else{
								resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
								break;
							}
			
							++iter;
							nodo.valorCampo = *(iter++);
							removerCaracter(&nodo.valorCampo, CARACTER_AGRUPADOR);
							this->estructuraCampos.listaCampos.push_back(nodo);
					
							if (iter != lineaParseada.end()){
								if (*mayus(&(*iter)) == "AND") operacion = ExpresionesLogicas::AND;
								else if (*iter == "OR") operacion = ExpresionesLogicas::OR;
								else{
									resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
									break;
								}
								++iter;
								if ( (this->estructuraCampos.operacion != NO_OPERACION) && (this->estructuraCampos.operacion != operacion) ){
									resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_AND_OR;
									break;
								}
								else this->estructuraCampos.operacion = operacion;
							}
						}
						
					} break;
					
					case OperacionesCapas::CONSULTAS_CONSULTA: {
						lineaParseada = parsearString(linea, SEPARATOR_CAMPOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
						iter = ++lineaParseada.begin();
						
						ListaStrings campos = parsearString(*mayus(&(*(iter++))), SEPARATOR_ATRIBUTOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
						EstructuraNombres estructuraNombres;
						ListaStrings campo;
						NodoListaOperaciones nodoOperaciones;
						
						for (ListaStrings::iterator iterStrings = campos.begin(); iterStrings != campos.end(); ++iterStrings){
							campo = parsearString(*iterStrings, SEPARATOR_TIPO_NOMBRE, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
							estructuraNombres.nombreTipo = *(campo.begin());
							estructuraNombres.nombreCampo = *(++campo.begin());
							this->estructuraConsulta.listaCamposSeleccionados.push_back(estructuraNombres);
						}

						if ( *mayus(&(*(iter++))) != "FROM" ){
							resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_FROM;
							break;
						}
						
						campos = parsearString(*mayus(&(*(iter++))), SEPARATOR_ATRIBUTOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
						for (ListaStrings::iterator iterStrings = campos.begin(); iterStrings != campos.end(); ++iterStrings){
							this->estructuraConsulta.listaNombresTipos.push_back(*iterStrings);
						}
						
						while ( (iter != lineaParseada.end()) && (*mayus(&(*iter)) == "JOIN") ){
							++iter;
							
							this->estructuraConsulta.listaNombresTipos.push_back(*mayus(&(*(iter++))));
							
							if ( *mayus(&(*(iter++))) != "ON" ){
								resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_ON;
								break;
							}
							
							campo = parsearString(*mayus(&(*(iter++))), SEPARATOR_TIPO_NOMBRE, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
							nodoOperaciones.estructuraNombresIzq.nombreTipo = *(campo.begin());
							nodoOperaciones.estructuraNombresIzq.nombreCampo = *(++campo.begin());
							
							if (*iter == "<") nodoOperaciones.operacion = ExpresionesLogicas::MENOR;
							else if (*iter == "<=") nodoOperaciones.operacion = ExpresionesLogicas::MENOR_O_IGUAL;
							else if (*iter == ">") nodoOperaciones.operacion = ExpresionesLogicas::MAYOR;
							else if (*iter == ">=") nodoOperaciones.operacion = ExpresionesLogicas::MAYOR_O_IGUAL;
							else if (*iter == "=") nodoOperaciones.operacion = ExpresionesLogicas::IGUAL;
							else{
								resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
								break;
							}
							
							++iter;
							campo = parsearString(*mayus(&(*(iter++))), SEPARATOR_TIPO_NOMBRE, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
							nodoOperaciones.estructuraNombresDer.nombreTipo = *(campo.begin());
							nodoOperaciones.estructuraNombresDer.nombreCampo = *(++campo.begin());
							
							this->estructuraConsulta.estructuraJoins.listaOperaciones.push_back(nodoOperaciones);
						}
						
						if (this->estructuraConsulta.estructuraJoins.listaOperaciones.size() != 0 ) 
							this->estructuraConsulta.estructuraJoins.operacion = ExpresionesLogicas::AND;
						
						if ( (iter != lineaParseada.end()) && (*mayus(&(*iter)) == "WHERE") ){
							++iter;
							while ( (iter != lineaParseada.end()) && (*mayus(&(*iter)) != "ORDER") ){
								
								campo = parsearString(*mayus(&(*(iter++))), SEPARATOR_TIPO_NOMBRE, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
								nodoOperaciones.estructuraNombresIzq.nombreTipo = *(campo.begin());
								nodoOperaciones.estructuraNombresIzq.nombreCampo = *(++campo.begin());
								
								if (*iter == "<") nodoOperaciones.operacion = ExpresionesLogicas::MENOR;
								else if (*iter == "<=") nodoOperaciones.operacion = ExpresionesLogicas::MENOR_O_IGUAL;
								else if (*iter == ">") nodoOperaciones.operacion = ExpresionesLogicas::MAYOR;
								else if (*iter == ">=") nodoOperaciones.operacion = ExpresionesLogicas::MAYOR_O_IGUAL;
								else if (*iter == "=") nodoOperaciones.operacion = ExpresionesLogicas::IGUAL;
								else{
									resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
									break;
								}
								
								++iter;
								campo = parsearString(*(iter++), SEPARATOR_TIPO_NOMBRE, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
								if (campo.size() < 2){
									nodoOperaciones.estructuraNombresDer.nombreTipo = "";
									removerCaracter(&(*campo.begin()), CARACTER_AGRUPADOR);
									nodoOperaciones.estructuraNombresDer.nombreCampo = *(campo.begin());
									mayus(&(nodoOperaciones.estructuraNombresDer.nombreCampo));
									this->estructuraConsulta.estructuraWhere.listaOperaciones.push_front(nodoOperaciones);
								}
								else{
									nodoOperaciones.estructuraNombresDer.nombreTipo = *(campo.begin());
									nodoOperaciones.estructuraNombresDer.nombreCampo = *(++campo.begin());
									mayus(&(nodoOperaciones.estructuraNombresDer.nombreTipo));
									mayus(&(nodoOperaciones.estructuraNombresDer.nombreCampo));
									this->estructuraConsulta.estructuraWhere.listaOperaciones.push_back(nodoOperaciones);
								}
								
								if ( (iter != lineaParseada.end()) && (*mayus(&(*iter)) != "ORDER") ){
									if (*iter == "AND") operacion = ExpresionesLogicas::AND;
									else if (*iter == "OR") operacion = ExpresionesLogicas::OR;
									else{
										resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_OPERACION;
										break;
									}
			
									if ( (this->estructuraConsulta.estructuraWhere.operacion != NO_OPERACION) && (this->estructuraConsulta.estructuraWhere.operacion != operacion) ){
										resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_AND_OR;
										break;
									}
									else this->estructuraConsulta.estructuraWhere.operacion = operacion;
									
									++iter;
								}
							}
						}
						
						if (this->estructuraConsulta.estructuraWhere.listaOperaciones.size() == 1 ) 
							this->estructuraConsulta.estructuraWhere.operacion = ExpresionesLogicas::AND;
						
						if ( (iter != lineaParseada.end()) && (*mayus(&(*(iter++))) == "ORDER") ){
							if ( *mayus(&(*(iter++))) != "BY" ){
								resultado = ResultadosParserOperaciones::ERROR_SINTAXIS_ORDER_BY;
								break;
							}
							
							campos = parsearString(*mayus(&(*(iter++))), SEPARATOR_ATRIBUTOS, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
							
							
							for (ListaStrings::iterator iterStrings = campos.begin(); iterStrings != campos.end(); ++iterStrings){
								campo = parsearString(*iterStrings, SEPARATOR_TIPO_NOMBRE, CARACTER_AGRUPADOR, CARACTER_AGRUPADOR);
								estructuraNombres.nombreTipo = *(campo.begin());
								estructuraNombres.nombreCampo = *(++campo.begin());
								this->estructuraConsulta.listaOrderBy.push_back(estructuraNombres);
							}

						}
						
					} break;
					
				}
			}
			

		}
	
	}
	
	return resultado;
	
}

list<string> ParserOperaciones::parsearString(string s, char caracterDelimitador, char caracterAgrupadorOpen, char caracterAgrupadorClose) {
	list<string> parseado;
	int separatorPos = -1;
	size_t nextSeparatorPos = 0, nextCaracterAgrupador = 0;

	nextSeparatorPos = s.find(caracterDelimitador);
	nextCaracterAgrupador = s.find(caracterAgrupadorOpen);
	while (nextSeparatorPos < string::npos){
		if (nextSeparatorPos > nextCaracterAgrupador){
			nextCaracterAgrupador = s.find(caracterAgrupadorClose, nextCaracterAgrupador + 1);
			nextSeparatorPos = s.find(caracterDelimitador, nextCaracterAgrupador + 1);
			nextCaracterAgrupador = s.find(caracterAgrupadorOpen, nextSeparatorPos + 1);
		}

		if (nextSeparatorPos != string::npos){
			parseado.push_back(s.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1));
			
			separatorPos = nextSeparatorPos;
			nextSeparatorPos = s.find(caracterDelimitador, separatorPos + 1);
			
			nextCaracterAgrupador = s.find(caracterAgrupadorOpen, separatorPos + 1);
		}
	}
	parseado.push_back(s.substr(separatorPos + 1));

	return parseado;
}

void ParserOperaciones::reemplazarCaracter(string* s, char caracterViejo, char caracterNuevo) {
	if (!s) return;
	
	for (string::iterator iter = s->begin(); iter != s->end(); ++iter){
		if ( (*iter) == caracterViejo ) s->replace(iter, iter+1, 1, caracterNuevo);
	}
}

void ParserOperaciones::limpiarInicio(string* s, char caracterALimpiar){
	for (string::iterator iter = s->begin(); (iter != s->end()) && (*iter == caracterALimpiar); ++iter) {
		s->erase(iter);
	}
}

void ParserOperaciones::removerCaracter(string* s, char caracter) {
	string::iterator iter; 
	for (iter = s->begin(); iter != (s->end() - 1); ++iter){
		if (*iter == caracter) s->erase(iter);
	}
	if (*iter == caracter) s->erase(iter); //TODO: PARCHE
}

string* ParserOperaciones::mayus(string* s){
	transform(s->begin(), s->end(), s->begin(), (int(*) (int))std::toupper);
	return s;
}

