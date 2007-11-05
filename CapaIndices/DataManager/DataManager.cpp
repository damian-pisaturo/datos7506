#include "DataManager.h"
// listaAtributos { numeroNodoListaLoca:valor para modif}
DataManager::DataManager()
{
	this->registro = NULL;
}

DataManager::~DataManager()
{
	if(this->registro)
		this->registro = NULL;
}

void DataManager::crearRegistroAlta(DefinitionsManager::ListaValoresAtributos &listaVA,
									DefinitionsManager::ListaTiposAtributos &listaTipos) {
	
	DefinitionsManager::ListaTiposAtributos::const_iterator itTipos = listaTipos.begin();
	DefinitionsManager::ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	int tipo; //Indica si el tipo de campo del registro es variable o fijo
	nodoLista regAttribute;
	unsigned short offsetToCampo = 0;
	unsigned short longRegistro = this->getTamanioRegistro(listaTipos,listaVA);
	char *registro = new char[longRegistro];
	string campoRegistro;
	
	regAttribute = *itTipos;
	// Si el registro es de tipo variable le guardo la longitud al registro
	if(regAttribute.tipo == TipoDatos::TIPO_VARIABLE){
		// Guardo la longitud del registro en los primeros dos bytes del mismo
		memcpy(registro,&longRegistro,Tamanios::TAMANIO_LONGITUD);
		offsetToCampo = sizeof(unsigned short);
	}
	// Se itera la lista de atributos del registro.
	// Y se van guardando los datos del registro
	for(++itTipos ; itTipos != listaTipos.end() ; ++itTipos,++itValoresAtributos){
		regAttribute = *itTipos;
		tipo = regAttribute.tipo;
		// Obtengo el campo del registro
		campoRegistro = *itValoresAtributos;
		if(tipo == TipoDatos::TIPO_ENTERO){
			int *entero = (int*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],entero,sizeof(int));
			offsetToCampo += sizeof(int);
		}
		else if(tipo == TipoDatos::TIPO_FECHA){
			ClaveFecha::TFECHA *fecha = (ClaveFecha::TFECHA*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],fecha,sizeof(ClaveFecha::TFECHA));
			offsetToCampo += sizeof(ClaveFecha::TFECHA);
		}
		else if(tipo == TipoDatos::TIPO_FLOAT){
			float *campoFloat = (float*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],campoFloat,sizeof(float));  
			offsetToCampo += sizeof(float);
		}
		else if(tipo == TipoDatos::TIPO_SHORT){
			short *campoShort = (short*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],campoShort,sizeof(short));  
			offsetToCampo += sizeof(short);
		}
		else if(tipo == TipoDatos::TIPO_STRING){
			// Obtengo la longitud del campo variable
			unsigned short longCampoVariable =  campoRegistro.size();
			// Guardo la longitud del campo variable en el registro
			memcpy(&registro[offsetToCampo],&longCampoVariable,sizeof(unsigned short));
			// Al offset le sumo los dos bytes de la longitud
			offsetToCampo += sizeof(unsigned short);
			const char *campoVariable = campoRegistro.c_str();
			// Guardo el campo variable sin el '/0'
			memcpy(&registro[offsetToCampo],campoVariable,longCampoVariable);
			offsetToCampo += longCampoVariable;
		}
		else if(tipo == TipoDatos::TIPO_BOOL){
			bool *campoBooleano = (bool*)campoRegistro.c_str();
			memcpy(&registro[offsetToCampo],campoBooleano,sizeof(bool));
			offsetToCampo += sizeof(bool);
		}
	}
	if(this->registro){
		delete[] this->registro;
		this->registro = NULL;
	}
	this->registro = registro;
}	
unsigned short DataManager::getTamanioRegistro(DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
											   DefinitionsManager::ListaValoresAtributos &listaVA){
	DefinitionsManager::ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	DefinitionsManager::ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	unsigned short tamanioRegistro = 0;
	nodoLista regAttribute = *itTipos;
	string campoRegistro;
	int tipo;
	
	// Si el tipo de registro es fijo obtengo directamente la longitud 
		if(regAttribute.tipo == TipoDatos::TIPO_FIJO)
			tamanioRegistro = ((unsigned short)atoi(regAttribute.pk.c_str()));
			
		else{
			// Le sumo los bytes que contendran la longitud del registro
			tamanioRegistro = sizeof(unsigned short);
			unsigned short tamanioRegOriginal = sizeof(unsigned short);
			// Itero la listas para ir obteniendo la longitud del registro
			for(++itTipos ;itTipos != listaTiposAtributos.end(); ++itTipos,++itValoresAtributos){
				regAttribute = *itTipos;
				tipo = regAttribute.tipo;
				
				
				if(tipo == TipoDatos::TIPO_ENTERO){
					tamanioRegistro += sizeof(int);
					tamanioRegOriginal += sizeof(int);
				}
				
				
				else if(tipo == TipoDatos::TIPO_FECHA){
					tamanioRegistro += sizeof(ClaveFecha::TFECHA);
					tamanioRegOriginal += sizeof(ClaveFecha::TFECHA);
				}
				
				else if(tipo == TipoDatos::TIPO_FLOAT){
				  	tamanioRegistro += sizeof(float);
				  	tamanioRegOriginal += sizeof(float);
				}
				
				else if(tipo == TipoDatos::TIPO_SHORT){
					tamanioRegistro +=sizeof(short);
					tamanioRegOriginal += sizeof(short);
				}
				else if(tipo == TipoDatos::TIPO_CHAR){
					tamanioRegistro += sizeof(char);
					tamanioRegOriginal += sizeof(char);
				}
				else if(tipo == TipoDatos::TIPO_STRING){
					// Obtengo la longitud del campo variable
					unsigned short longCampoVariable;
					
					
					if (*itValoresAtributos != "")
						longCampoVariable = itValoresAtributos->size();
					else
						longCampoVariable = (unsigned short)*(registro + tamanioRegOriginal);
					
					// le sumo al tamanio la longitud del string mas los dos bytes que indicaran la longitud del mismo
					tamanioRegistro += sizeof(unsigned short) + longCampoVariable;

				}
				else if(tipo == TipoDatos::TIPO_BOOL)
					tamanioRegistro += sizeof(bool);
				
				
			}
			
		}
		return tamanioRegistro;
}


void DataManager::crearRegistroModificacion(DefinitionsManager::ListaTiposAtributos &listaTiposAtributos,
											DefinitionsManager::ListaValoresAtributos &listaVA,
											DefinitionsManager::ListaClaves &listaClaves){
	
	DefinitionsManager::ListaTiposAtributos::const_iterator itTipos = listaTiposAtributos.begin();
	DefinitionsManager::ListaValoresAtributos::const_iterator itValoresAtributos = listaVA.begin();
	
	char *registroModificado;
	unsigned short offsetToProxCampo = 0;
	
	unsigned char longRegModificado = getTamanioRegistro(listaTiposAtributos,listaVA);
	
	registroModificado = new char[longRegModificado];
	
	++itTipos;
	
}

int DataManager::insertar(const string& nombreTipo, const DefinitionsManager::ListaValoresAtributos* listaVA,
				 		  const DefinitionsManager::ListaTiposAtributos* listaTipos) {
	
	return 0;
	
}

int DataManager::eliminar(const string& nombreTipo, DefinitionsManager::ListaClaves* listaClaves) {
	
	return 0;
	
}

int DataManager::modificar(const string& nombreTipo, const DefinitionsManager::ListaValoresAtributos* listaVA,
			  			   const DefinitionsManager::ListaTiposAtributos* listaTipos,
			  			   DefinitionsManager::ListaClaves* listaClaves) {
	
	return 0;
}

