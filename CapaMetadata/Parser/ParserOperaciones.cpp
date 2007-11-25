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

ParserOperaciones::ParserOperaciones(const string &nombreArchivo)
{
	this->archivo.open(nombreArchivo.c_str());
	this->numOperacion        = 0;
	this->mapValoresAtributos = NULL;
	this->listaClaves         = NULL;
}

ParserOperaciones::~ParserOperaciones()
{
	if (this->archivo.is_open()) 
		this->archivo.close();
	
	if (this->mapValoresAtributos)
		delete this->mapValoresAtributos;
	
	if (this->listaClaves)
		delete this->listaClaves;	
}

bool ParserOperaciones::proximaOperacion()
{	
	string linea;
	bool leyendoClaves; // es true si se trata de una modificacion y esta leyendo la parte que contiene la clave o lista de claves 
	bool procesado = false;
	
	while ( (!this->archivo.fail()) && (!procesado) ){
		leyendoClaves = false;
		getline(this->archivo, linea);
		
		if ( (linea.size() != 0) && (linea[0] != CARACTER_COMENTARIO) ) {
			if (this->mapValoresAtributos){
				delete this->mapValoresAtributos;
				this->mapValoresAtributos = NULL;
			}
			
			this->mapValoresAtributos = new DefinitionsManager::MapaValoresAtributos();
			
			if (this->listaClaves){
				delete this->listaClaves;
				this->listaClaves = NULL;
			}
			
			this->listaClaves = new DefinitionsManager::ListaClaves();
			
			procesado = true;
			this->numOperacion++;
									
			size_t separatorPos, nextSeparatorPos, proximoSeparatorCampos;
			string nombreAtributo, valorAtributo;
			
			separatorPos = linea.find(SEPARATOR_CAMPOS);
			nextSeparatorPos = linea.find(SEPARATOR_CAMPOS, separatorPos + 1);
			
			this->nombreTipo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);			
			
			proximoSeparatorCampos = linea.find(SEPARATOR_CAMPOS, linea.find(SEPARATOR_CAMPOS, nextSeparatorPos + 1));
			
			while (nextSeparatorPos != string::npos){
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = linea.find(ASIGNACION_ATRIBUTOS, separatorPos + 1);
				nombreAtributo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);
				
				separatorPos = nextSeparatorPos;
				nextSeparatorPos = linea.find(SEPARATOR_ATRIBUTOS, separatorPos + 1);
				
				if ( (!leyendoClaves) && (linea[0] == MODIFICACION) && ((nextSeparatorPos == string::npos) || (nextSeparatorPos > proximoSeparatorCampos)) ){
					nextSeparatorPos = proximoSeparatorCampos;
				}
				valorAtributo = linea.substr(separatorPos + 1, nextSeparatorPos - separatorPos - 1);

				switch(linea[0]){
					case ALTA:
						this->tipoOperacion = OperacionesCapas::METADATA_ALTA;
						(*this->mapValoresAtributos)[nombreAtributo] = valorAtributo;
						
						break;					
					case BAJA: {
						this->tipoOperacion = OperacionesCapas::METADATA_BAJA;
						DefinitionsManager::NodoListaClaves nodo;
						nodo.nombreClave = nombreAtributo;
						nodo.valorClave = valorAtributo;
						this->listaClaves->push_back(nodo);
						
					}break;
					
					case MODIFICACION:
						this->tipoOperacion = OperacionesCapas::METADATA_MODIFICACION;
						if (leyendoClaves){
							DefinitionsManager::NodoListaClaves nodo;
							nodo.nombreClave = nombreAtributo;
							nodo.valorClave = valorAtributo;
							this->listaClaves->push_back(nodo);
						}
						else{
							(*this->mapValoresAtributos)[nombreAtributo] = valorAtributo;
							if (nextSeparatorPos == proximoSeparatorCampos) leyendoClaves = true;
						}
						break;
						
					case CONSULTA: {
						this->tipoOperacion = OperacionesCapas::METADATA_CONSULTA;
						DefinitionsManager::NodoListaClaves nodo;
						nodo.nombreClave = nombreAtributo;
						nodo.valorClave = valorAtributo;
						this->listaClaves->push_back(nodo);
					
					}break;
						
				}
			}

		}
	}
	
	return procesado;
	
}

