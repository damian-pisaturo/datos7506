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
				unsigned short anio;
				unsigned char dia, mes;
				conversor << (*iterLV).substr(0, 4);
				conversor >> anio;
				conversor.clear();
				conversor << (*iterLV).substr(4, 2);
				conversor >> mes;
				conversor.clear();
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
				unsigned short anio;
				unsigned char dia, mes;
				conversor << (*iterLC).valorClave.substr(0, 4);
				conversor >> anio;
				conversor.clear();
				conversor << (*iterLC).valorClave.substr(4, 2);
				conversor >> mes;
				conversor.clear();
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
		
	}
	
	if (listaClaves.size() > 1)
		return new ClaveCompuesta(listaClaves);
	
	return clave;
	
}

