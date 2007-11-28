#include "ClaveFactory.h"

ClaveFactory ClaveFactory::instance;

Clave* ClaveFactory::getClave(const DefinitionsManager::ListaValoresClaves& listaValoresClaves, const ListaTipos& listaTipos) const {
	
	DefinitionsManager::ListaValoresClaves::const_iterator iterLV;
	ListaTipos::const_iterator iterLT;
	Clave* clave = NULL;
	std::stringstream conversor;
	ListaClaves listaClaves;
	
	for (iterLV = listaValoresClaves.begin(), iterLT = listaTipos.begin();
		(iterLV != listaValoresClaves.end()) && (iterLT != listaTipos.end());
		++iterLV, ++iterLT) {
		
		switch(*iterLT){
					
			case TipoDatos::TIPO_BOOL:
			{	
				conversor << *iterLV;
				bool valor;
				conversor >> valor;
				clave = new ClaveBoolean(valor);
				break;
			}
			case TipoDatos::TIPO_CHAR:
			{	
				conversor << *iterLV;
				char valor;
				conversor >> valor;
				clave = new ClaveChar(valor);
				break;
			}
			case TipoDatos::TIPO_SHORT:
			{
				conversor << *iterLV;
				short valor;
				conversor >> valor;
				clave = new ClaveShort(valor);
				break;
			}
			case TipoDatos::TIPO_ENTERO:
			{
				conversor << *iterLV;
				int valor;
				conversor >> valor;
				clave = new ClaveEntera(valor);
				break;
			}
			case TipoDatos::TIPO_FLOAT:
			{
				conversor << *iterLV;
				float valor;
				conversor >> valor;
				clave = new ClaveReal(valor);
				break;
			}
			case TipoDatos::TIPO_FECHA:
			{
				ClaveFecha::TFECHA valor;
				unsigned short anio, dia, mes;
				conversor << (*iterLV).substr(0, 4);
				conversor >> anio;
				conversor.clear();
				conversor.str("");
				conversor << (*iterLV).substr(4, 2);
				conversor >> mes;
				conversor.clear();
				conversor.str("");
				conversor << (*iterLV).substr(6, 2);
				conversor >> dia;
				valor.crear(dia, mes, anio);
				clave = new ClaveFecha(valor);
				break;
			}
			case TipoDatos::TIPO_STRING:
				clave = new ClaveVariable(*iterLV);
				break;
				
		}
		
		listaClaves.push_back(clave);
		conversor.clear();
		conversor.str("");
		
		
	}
	
	if (listaValoresClaves.size() > 1)
		return new ClaveCompuesta(listaClaves);
	
	return clave;
	
}


Clave* ClaveFactory::getClave(const DefinitionsManager::ListaClaves& listaValoresClaves, const ListaNodos& listaNodos) const {
	
	DefinitionsManager::ListaClaves::const_iterator iterLC;
	ListaNodos::const_iterator iterLN;
	Clave* clave = NULL;
	std::stringstream conversor;
	ListaClaves listaClaves;
	
	for (iterLC = listaValoresClaves.begin(), iterLN = ++(listaNodos.begin());
		(iterLC != listaValoresClaves.end()) && (iterLN != listaNodos.end());
		++iterLC, ++iterLN) {
		
		switch((*iterLN).tipo) {
					
			case TipoDatos::TIPO_BOOL:
			{
				conversor << (*iterLC).valorClave;
				bool valor;
				conversor >> valor;
				clave = new ClaveBoolean(valor);
				break;
			}
			case TipoDatos::TIPO_CHAR:
			{
				conversor << (*iterLC).valorClave;
				char valor;
				conversor >> valor;
				clave = new ClaveChar(valor);
				break;
			}
			case TipoDatos::TIPO_SHORT:
			{
				conversor << (*iterLC).valorClave;
				short valor;
				conversor >> valor;
				clave = new ClaveShort(valor);
				break;
			}
			case TipoDatos::TIPO_ENTERO:
			{
				conversor << (*iterLC).valorClave;
				int valor;
				conversor >> valor;
				clave = new ClaveEntera(valor);
				break;
			}
			case TipoDatos::TIPO_FLOAT:
			{
				conversor << (*iterLC).valorClave;
				float valor;
				conversor >> valor;
				clave = new ClaveReal(valor);
				break;
			}
			case TipoDatos::TIPO_FECHA:
			{
				ClaveFecha::TFECHA valor;
				unsigned short anio, dia, mes;
				conversor << (*iterLC).valorClave.substr(0, 4);
				conversor >> anio;
				conversor.clear();
				conversor.str("");
				conversor << (*iterLC).valorClave.substr(4, 2);
				conversor >> mes;
				conversor.clear();
				conversor.str("");
				conversor << (*iterLC).valorClave.substr(6, 2);
				conversor >> dia;
				valor.crear(dia, mes, anio);
				clave = new ClaveFecha(valor);
				break;
			}
			case TipoDatos::TIPO_STRING:
				clave = new ClaveVariable((*iterLC).valorClave);
				break;
				
		}
		
		listaClaves.push_back(clave);
		conversor.clear();
		conversor.str("");
		
	}
	
	if (listaClaves.size() > 1)
		return new ClaveCompuesta(listaClaves);
	
	return clave;
	
}


Clave* ClaveFactory::getClave(const char* lista, const ListaTipos& listaTipos) const {
	
	ListaTipos::const_iterator iterLT;
	Clave* clave = NULL;
	unsigned short offset = 0;
	ListaClaves listaClaves;
	
	for (iterLT = listaTipos.begin(); iterLT != listaTipos.end(); ++iterLT) {
		
		switch(*iterLT) {
					
			case TipoDatos::TIPO_BOOL:
			{	
				bool valor = *((bool*)(lista + offset));
				clave = new ClaveBoolean(valor);
				offset += sizeof(bool);
				break;
			}
			case TipoDatos::TIPO_CHAR:
			{	
				char valor = *(lista + offset);
				clave = new ClaveChar(valor);
				offset += sizeof(char);
				break;
			}
			case TipoDatos::TIPO_SHORT:
			{
				short valor = *((short*)(lista + offset));
				clave = new ClaveShort(valor);
				offset += sizeof(short);
				break;
			}
			case TipoDatos::TIPO_ENTERO:
			{
				int valor = *((int*)(lista + offset));
				clave = new ClaveEntera(valor);
				offset += sizeof(int);
				break;
			}
			case TipoDatos::TIPO_FLOAT:
			{
				float valor = *((float*)(lista + offset));
				clave = new ClaveReal(valor);
				offset += sizeof(float);
				break;
			}
			case TipoDatos::TIPO_FECHA:
			{
				ClaveFecha::TFECHA valor;
				unsigned short anio;
				unsigned char dia, mes;
				anio = *((unsigned short*)(lista + offset));
				offset += sizeof(unsigned short);
				mes = *((unsigned char*)(lista + offset));
				offset += sizeof(unsigned char);
				dia = *((unsigned char*)(lista + offset));
				offset += sizeof(unsigned char);
				valor.crear(dia, mes, anio);
				clave = new ClaveFecha(valor);
				break;
			}
			case TipoDatos::TIPO_STRING:
			{	
				unsigned char longitud = *((unsigned char*)(lista + offset));
				offset += sizeof(unsigned char);
				string cadena(lista + offset, longitud);
				clave = new ClaveVariable(cadena);
				break;
			}	
		}
		
		listaClaves.push_back(clave);
		
	}
	
	if (listaClaves.size() > 1)
		return new ClaveCompuesta(listaClaves);
	
	return clave;

	
}

