#include "Registro.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Registro 
//			(Permite el manejo y parseo de los atributos contenidos
//				en un registro).
//////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////
    // Constructor/Destructor
	///////////////////////////////////////////////////////////////////////

		Registro::Registro(unsigned char tipoOrg, char* data, ListaTipos* listaTipos,
						   ListaNombresAtributos* listaNombresAtributos)
		{
			unsigned int offset = 0;
			stringstream conversor;
		
			if (tipoOrg == TipoDatos::TIPO_VARIABLE)
				offset += Tamanios::TAMANIO_LONGITUD;
			
			for (ListaTipos::const_iterator iterTipos = listaTipos->begin(); iterTipos != listaTipos->end(); ++iterTipos){
				
				switch(*iterTipos){
					case TipoDatos::TIPO_BOOL:
					{	
						conversor << *(bool*)(data + offset);				
						offset += sizeof(bool);
				
					}break;
					
					case TipoDatos::TIPO_CHAR:
					{	
						conversor << *(char*)(data + offset);				
						offset += sizeof(char);
			
					}break;
					
					case TipoDatos::TIPO_SHORT:
					{
						conversor << *(short*)(data + offset);				
						offset += sizeof(short);
						
					}break;
					
					case TipoDatos::TIPO_ENTERO:
					{
						conversor << *(int*)(data + offset);				
						offset += sizeof(int);
						
					}break;
					
					case TipoDatos::TIPO_FLOAT:
					{
						conversor << *(float*)(data + offset);				
						offset += sizeof(float);
						
					}break;
					
					case TipoDatos::TIPO_FECHA:
					{						
						unsigned short anio = *(unsigned short*)(data + offset);
						offset += sizeof(unsigned short);
						
						unsigned short mes = (short)(*(unsigned char*)(data + offset));
						offset += sizeof(unsigned char);
						
						unsigned short dia = (short)(*(unsigned char*)(data + offset));
						offset += sizeof(unsigned char);
						
						conversor << anio;
						unsigned longAnio = conversor.str().size(), cero = 0;
						if (longAnio < 4) {
							conversor.str("");
							for (unsigned i = 0; i < (4 - longAnio); ++i)
								conversor << cero;
							conversor << anio;
						}
						if (mes < 10) conversor << cero;
						conversor << mes;
						if (dia < 10) conversor << cero;
						conversor << dia;
						
					} break;
					
					case TipoDatos::TIPO_STRING:
					{
						unsigned char sizeCadena = *(unsigned char*)(data + offset);
						offset += sizeof(unsigned char);
						
						string cadena(data + offset, sizeCadena);		
						conversor << cadena;
						
						offset += sizeCadena;
							
					} break;
					
				}
				
				this->listaAtributos.push_back(conversor.str());
				conversor.clear();
				conversor.str("");
			}
			
			this->tipoOrg               = tipoOrg;
			this->listaNombresAtributos = listaNombresAtributos;
			this->listaTipos            = listaTipos;
			
		}
		
	
		Registro::~Registro()
		{
			//No se libera la memoria de las listas xq son propias del DefinitionsManager
		}

	///////////////////////////////////////////////////////////////////////
    // Metodos publicos
	///////////////////////////////////////////////////////////////////////
		
		unsigned char Registro::getCantidadAtributos()
		{
			return this->listaTipos->size();
		}
		
		
		string Registro::getAtributo(unsigned char posicion)
		{
			ListaValoresClaves::const_iterator iter = this->listaAtributos.begin();
			
			for (unsigned char i = 0; i < posicion; i++) ++iter;
			
			return *iter;			
		}
		
		
		int Registro::getTipoAtributo(unsigned char posicion)
		{
			ListaTipos::const_iterator iter = this->listaTipos->begin();
					
			for (unsigned char i = 0; i < posicion; i++) ++iter;
					
			return *iter;			
		}
		
		
		string Registro::getNombreAtributo(unsigned char posicion)
		{
			ListaNombresAtributos::const_iterator iter = this->listaNombresAtributos->begin();
					
			for (unsigned char i = 0; i < posicion; i++) iter++;
					
			return *iter;			
		}
		
		
		Clave* Registro::getClave(const ListaNombresClaves &listaNombresClaves)
		{
			// Atributos necesarios para armar la clave
			Clave* claveAux = NULL;
			ListaValoresClaves listaValoresClaves;
			ListaTipos listaTiposClaves;
			
			// Iteradores de las listas que tiene esta clase como atributos
			ListaNombresAtributos::const_iterator iterNomAtributos;
			ListaValoresClaves::const_iterator iterValAtributos;
			ListaTipos::const_iterator iterTipos;
			
			// Iterador de la lista pasada por parámetro
			ListaNombresClaves::const_iterator iterNomClaves;
			
			// Se recorre la lista de nombres pasada por parametro y las listas de nombres de los 
			// atributos del registro, sus valores y sus tipos. Si se encuentra una coincidencia entre
			// el nombre pasado y el nombre del atributo, se copia el valor de dicho atributo y su tipo
			// a dos listas definidas para tal fin.
			for (iterNomClaves = listaNombresClaves.begin(); iterNomClaves != listaNombresClaves.end();
				++iterNomClaves) {
				
				for (iterNomAtributos = this->listaNombresAtributos->begin(),
					 iterValAtributos = this->listaAtributos.begin(),
					 iterTipos = this->listaTipos->begin();
					 iterNomAtributos != this->listaNombresAtributos->end(); 
					 ++iterNomAtributos, ++iterTipos, ++iterValAtributos) {
					
					if (*iterNomAtributos == *iterNomClaves) {
						listaTiposClaves.push_back(*iterTipos);
						listaValoresClaves.push_back(*iterValAtributos);
						break;
					}
					
				}
				
			}
			
			//Si las listas de valores y tipos no estan vacias (es decir, hubo -al menos- una coincidencia en los nombres 
			//buscados en los ciclos anteriores, se instancia una clave con los valores y de los tipos contenidas en ellas.			
			if ( (listaValoresClaves.size() > 0) && (listaTiposClaves.size() > 0) )
				claveAux = ClaveFactory::getInstance().getClave(listaValoresClaves, listaTiposClaves);
			
			return claveAux;
		}
