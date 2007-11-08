#include "ClaveFactory.h"

ClaveFactory ClaveFactory::instance;

Clave* ClaveFactory::getClave(const ListaValoresClaves& listaValoresClaves, const ListaTipos& listaTipos) const {
	
	ListaValoresClaves::const_iterator iterLV;
	ListaTipos::const_iterator iterLT;
	Clave* clave = NULL;
	std::stringstream conversor;
	ListaClaves listaClaves;
	
	for (iterLV = listaValoresClaves.begin(), iterLT = listaTipos.begin();
		(iterLV != listaValoresClaves.end()) && (iterLT != listaTipos.end());
		++iterLV, ++iterLT) {
		
		switch(*iterLT){
					
			case TipoDatos::TIPO_BOOL:
				conversor << *iterLV;
				bool valor;
				conversor >> valor;
				conversor.clear();
				clave = new ClaveBoolean(valor);
				break;
			case TipoDatos::TIPO_CHAR:
				conversor << *iterLV;
				char valor;
				conversor >> valor;
				conversor.clear();
				clave = new ClaveChar(valor);
				break;
			case TipoDatos::TIPO_SHORT:
				conversor << *iterLV;
				short valor;
				conversor >> valor;
				conversor.clear();
				clave = new ClaveShort(valor);
				break;
			case TipoDatos::TIPO_ENTERO:
				conversor << *iterLV;
				int valor;
				conversor >> valor;
				conversor.clear();
				clave = new ClaveEntera(valor);
				break;
			case TipoDatos::TIPO_FLOAT:
				conversor << *iterLV;
				float valor;
				conversor >> valor;
				conversor.clear();
				clave = new ClaveReal(valor);
				break;
			case TipoDatos::TIPO_FECHA:
				conversor << *iterLV;
				ClaveFecha::TFECHA valor;
				conversor >> valor;
				conversor.clear();
				clave = new ClaveFecha(valor);
				break;
			case TipoDatos::TIPO_STRING:
				clave = new ClaveVariable(*iterLV);
				break;
				
		}
		
		listaClaves.push_back(clave);
		
	}
	
	if (listaValoresClaves.size() > 1)
		return new ClaveCompuesta(listaClaves);
	
	return clave;
	
}

