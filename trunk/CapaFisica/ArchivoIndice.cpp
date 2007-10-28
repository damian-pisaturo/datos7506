///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndice.cpp
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Implementacion de las clases ArchivoIndice correspodientes
//      a cada tipo de dato (booleano, char, short, entero, real, fecha, 
//		string) e indice (Griego y Romano).
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////
#include "ArchivoIndice.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroGriego 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
	ArchivoIndiceEnteroGriego::ArchivoIndiceEnteroGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice):
		ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice){ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* ArchivoIndiceEnteroGriego::leerClaveHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer+=sizeof(int);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveEntera(valor, refRegistro);	
		}
		
		Clave* ArchivoIndiceEnteroGriego::leerClaveNoHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer+=sizeof(int);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveEntera(valor, refRegistro, hijoDer);
		}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroRomano
//		   (Implementa archivo de indices secundarios de clave entera).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceEnteroRomano::ArchivoIndiceEnteroRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
	
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		Clave* ArchivoIndiceEnteroRomano::leerClaveHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer += sizeof(int);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveEntera(valor, refRegistro);	
		}
		
		Clave* ArchivoIndiceEnteroRomano::leerClaveNoHoja(char* &buffer)
		{
			int valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor entero de la clave.
			memcpy(&valor, buffer, sizeof(int));
			buffer += sizeof(int);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveEntera(valor, refRegistro, hijoDer);
		}
		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceBooleanGriego
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceBooleanGriego::ArchivoIndiceBooleanGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice):
			ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////			
		Clave* ArchivoIndiceBooleanGriego::leerClaveHoja(char* &buffer)
		{
			bool valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor boolean de la clave.
			memcpy(&valor, buffer, sizeof(bool));
			buffer += sizeof(bool);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveEntera(valor, refRegistro);	
		}
			
		Clave* ArchivoIndiceBooleanGriego::leerClaveNoHoja(char* &buffer)
		{
			bool valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor booleano de la clave.
			memcpy(&valor, buffer, sizeof(bool));
			buffer += sizeof(bool);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveBoolean(valor, refRegistro, hijoDer);
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharGriego
//		   (Implementa archivo de indices primarios de clave tipo char).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCharGriego::ArchivoIndiceCharGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice):
			ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		Clave* ArchivoIndiceCharGriego::leerClaveHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor boolean de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveChar(valor, refRegistro);	
		}
				
		Clave* ArchivoIndiceCharGriego::leerClaveNoHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor de tipo char de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveChar(valor, refRegistro, hijoDer);
		}
	
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharRomano
//		   (Implementa archivo de indices secundarios de clave tipo char).
///////////////////////////////////////////////////////////////////////////	
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCharRomano::ArchivoIndiceCharRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* ArchivoIndiceCharRomano::leerClaveHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor boolean de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveChar(valor, refRegistro);	
		}
						
		Clave* ArchivoIndiceCharRomano::leerClaveNoHoja(char* &buffer)
		{
			char valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor de tipo char de la clave.
			memcpy(&valor, buffer, sizeof(char));
			buffer += sizeof(char);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveChar(valor, refRegistro, hijoDer);
		}
	
//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Nombre: ArchivoIndiceShortGriego
//		   (Implementa archivo de indices primarios de clave de tipo short).
//////////////////////////////////////////////////////////////////////////////	
					
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceShortGriego::ArchivoIndiceShortGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice):
			ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice)
		{ }
				
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* ArchivoIndiceShortGriego::leerClaveHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveShort(valor, refRegistro);	
		}
						
		Clave* ArchivoIndiceShortGriego::leerClaveNoHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveShort(valor, refRegistro, hijoDer);
		}
				
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceShortRomano
//		   (Implementa archivo de indices secundarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
		
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceShortRomano::ArchivoIndiceShortRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* ArchivoIndiceShortRomano::leerClaveHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveShort(valor, refRegistro);	
		}
						
		Clave* ArchivoIndiceShortRomano::leerClaveNoHoja(char* &buffer)
		{
			short valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(short));
			buffer += sizeof(short);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear clave
			return new ClaveShort(valor, refRegistro, hijoDer);
		}

//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceRealRomano
//		   (Implementa archivo de indices secundarios de clave de tipo float).
//////////////////////////////////////////////////////////////////////////////
							
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceRealRomano::ArchivoIndiceRealRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* ArchivoIndiceRealRomano::leerClaveHoja(char* &buffer)
		{
			float valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor float de la clave.
			memcpy(&valor, buffer, sizeof(float));
			buffer += sizeof(float);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveReal(valor, refRegistro);	
		}
						
		Clave* ArchivoIndiceRealRomano::leerClaveNoHoja(char* &buffer)
		{
			float valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor float de la clave.
			memcpy(&valor, buffer, sizeof(float));
			buffer += sizeof(float);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveReal(valor, refRegistro, hijoDer);
		}
		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceFechaGriego
//		   (Implementa archivo de indices primarios de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////
							
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceFechaGriego::ArchivoIndiceFechaGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice):
			ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* ArchivoIndiceFechaGriego::leerClaveHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor short de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro);	
		}
						
		Clave* ArchivoIndiceFechaGriego::leerClaveNoHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor fecha de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro, hijoDer);
		}

//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceFechaRomano
//		   (Implementa archivo de indices secundarios de clave de tipo fecha).
//////////////////////////////////////////////////////////////////////////////
									
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceFechaRomano::ArchivoIndiceFechaRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
								
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	
		Clave* ArchivoIndiceFechaRomano::leerClaveHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;
		
			//Se interpreta el valor float de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro);	
		}
						
		Clave* ArchivoIndiceFechaRomano::leerClaveNoHoja(char* &buffer)
		{
			ClaveFecha::TFECHA valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor fecha de la clave.
			memcpy(&valor, buffer, sizeof(ClaveFecha::TFECHA));
			buffer += sizeof(ClaveFecha::TFECHA);
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveFecha(&valor, refRegistro, hijoDer);
		}

		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceVariableGriego 
//		   (Implementa archivo de indices primarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceVariableGriego::ArchivoIndiceVariableGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice):
			ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////	

		void ArchivoIndiceVariableGriego::copiarClaveHoja(Clave* clave, char* &buffer)
		{
			string* valor = (string*) clave->getValor();
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
		} 
			
		void ArchivoIndiceVariableGriego::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			string* valor = (string*) clave->getValor();
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
		}	
			
			
		Clave* ArchivoIndiceVariableGriego::leerClaveHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;

			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveVariable(valor, refRegistro);	
		}
							
		Clave* ArchivoIndiceVariableGriego::leerClaveNoHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveVariable(valor, refRegistro, hijoDer);
		}
		
///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceVariableRomano 
//		   (Implementa archivo de indices secundarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceVariableRomano::ArchivoIndiceVariableRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice):
			ArchivoIndiceSecundario(tamNodo, nombreArchivo, tamBloqueLista, tipoIndice)
		{ }
						
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		void ArchivoIndiceVariableRomano::copiarClaveHoja(Clave* clave, char* &buffer)
		{
			string* valor = (string*) clave->getValor();
			
			//Copia del valor variable (char*) de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
		} 
			
		void ArchivoIndiceVariableRomano::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			string* valor = (string*) clave->getValor();
			
			//Copia del valor entero de la clave
			memcpy(buffer, valor->c_str(), clave->getTamanioValor());
			buffer += clave->getTamanioValor();
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
			}
		}	
	
		Clave* ArchivoIndiceVariableRomano::leerClaveHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;

			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveVariable(valor, refRegistro);	
		}
							
		Clave* ArchivoIndiceVariableRomano::leerClaveNoHoja(char* &buffer)
		{
			string valor;
			unsigned int refRegistro;		
			unsigned int hijoDer = 0;
			
			//Se interpreta el valor variable de la clave.
			valor = buffer;
			buffer += valor.size() + 1;
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveVariable(valor, refRegistro, hijoDer);
		}

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCompuestoGriego 
//		   (Implementa archivo de indices primarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCompuestoGriego::ArchivoIndiceCompuestoGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice, ListaTipos* listaTipos):
			ArchivoIndiceArbol(tamNodo, nombreArchivo, tipoIndice)
		{
			this->tipos = listaTipos;
		}
		
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		
		void ArchivoIndiceCompuestoGriego::copiarClaveHoja(Clave* clave, char* &buffer)
		{
			ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
			ListaClaves* listaClaves = claveCompuesta->getListaClaves();
			Clave* claveAux;
			
			for (ListaClaves::const_iterator iterClaves = listaClaves->begin();
				iterClaves != listaClaves->end(); ++iterClaves) {
				
				claveAux = (*iterClaves);
				//Copia del valor de la clave
				memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
				buffer += claveAux->getTamanioValor();
			}
			
			//Copia de la referencia a registro del archivo de datos. 
			unsigned int referencia = clave->getReferencia();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
		}
			
		void ArchivoIndiceCompuestoGriego::copiarClaveNoHoja(Clave* clave, char* &buffer)
		{
			unsigned int referencia = 0;
			ClaveCompuesta* claveCompuesta = static_cast<ClaveCompuesta*>(clave);
			ListaClaves* listaClaves = claveCompuesta->getListaClaves();
			Clave* claveAux;
			
			//Copia de los valores de las claves en el nodo
			for (ListaClaves::const_iterator iterClaves = listaClaves->begin();
				iterClaves != listaClaves->end(); ++iterClaves) {
				
				claveAux = (*iterClaves);
				//Copia del valor de la clave
				memcpy(buffer, claveAux->getValor(), claveAux->getTamanioValor());
				buffer += claveAux->getTamanioValor();
			}
			
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.			
				referencia = clave->getReferencia();
				memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Copia de la referencia al hijo derecho.
			referencia = clave->getHijoDer();
			memcpy(buffer, &referencia, Tamanios::TAMANIO_REFERENCIA);			
		}			
			
		Clave* ArchivoIndiceCompuestoGriego::leerClaveHoja(char* &buffer)
		{
			ListaClaves listaClaves;
			string* tipo;
			unsigned int refRegistro = 0, tamanio = 0;
			void* valor = NULL;
			
			//Copia de los valores de las claves en el nodo				
			for (ListaTipos::const_iterator iterTipos = this->tipos->begin();
				iterTipos != this->tipos->end(); ++iterTipos) {			
					
					tipo = (*iterTipos);
					
					if ((*tipo) == TipoDatos::TIPO_ENTERO){						
						//Copia del valor de la clave
						valor = new int;
						tamanio = sizeof(int);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveEntera(*((int*)valor)));
					
					}else if ((*tipo) == TipoDatos::TIPO_BOOL){
						valor = new bool;
						tamanio = sizeof(bool);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
					}else if ((*tipo) == TipoDatos::TIPO_CHAR){
						valor = new char;
						tamanio = sizeof(char);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveChar(*((char*)valor)));
						
					}else if ((*tipo) == TipoDatos::TIPO_SHORT){
						valor = new short;
						tamanio = sizeof(short);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveShort(*((short*)valor)));
						
					}else if ((*tipo) == TipoDatos::TIPO_REAL){
						valor = new float;
						tamanio = sizeof(float);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveReal(*((float*)valor)));
					}else if ((*tipo) == TipoDatos::TIPO_FECHA){
						valor = new ClaveFecha::TFECHA;
						tamanio = sizeof(ClaveFecha::TFECHA);
						
						memcpy(valor, buffer, tamanio);
						listaClaves.push_back(new ClaveFecha((ClaveFecha::TFECHA*)valor));
					}else if ((*tipo) == TipoDatos::TIPO_VARIABLE){
						valor = new string;
						valor = buffer;
						tamanio = ((string*)valor)->size() + 1;
	
						listaClaves.push_back(new ClaveVariable(*((string*)valor)));
					}
					
					delete valor;
					buffer += tamanio;
				}
	
			//Se interpreta la referencia al registro de datos.
			memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
		
			//Crear la clave
			return new ClaveCompuesta(listaClaves, refRegistro);	
		}
							
		Clave* ArchivoIndiceCompuestoGriego::leerClaveNoHoja(char* &buffer)
		{
			ListaClaves listaClaves;
			string* tipo;
			unsigned int refRegistro = 0, hijoDer = 0, tamanio = 0;
			void* valor = NULL;
			
			//Copia de los valores de las claves en el nodo				
			for (ListaTipos::const_iterator iterTipos = this->tipos->begin();
				iterTipos != this->tipos->end(); ++iterTipos) {			
					
				tipo = (*iterTipos);
				
				if (*tipo == TipoDatos::TIPO_ENTERO){						
					//Copia del valor de la clave
					valor = new int;
					tamanio = sizeof(int);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveEntera(*((int*)valor)));
				
				}else if (*tipo == TipoDatos::TIPO_BOOL){
					valor = new bool;
					tamanio = sizeof(bool);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveBoolean(*((bool*)valor)));
				}else if (*tipo == TipoDatos::TIPO_CHAR){
					valor = new char;
					tamanio = sizeof(char);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveChar(*((char*)valor)));
					
				}else if (*tipo == TipoDatos::TIPO_SHORT){
					valor = new short;
					tamanio = sizeof(short);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveShort(*((short*)valor)));
					
				}else if (*tipo == TipoDatos::TIPO_REAL){
					valor = new float;
					tamanio = sizeof(float);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveReal(*((float*)valor)));
				}else if (*tipo == TipoDatos::TIPO_FECHA){
					valor = new ClaveFecha::TFECHA;
					tamanio = sizeof(ClaveFecha::TFECHA);
					
					memcpy(valor, buffer, tamanio);
					listaClaves.push_back(new ClaveFecha((ClaveFecha::TFECHA*)valor));
				}else if (*tipo == TipoDatos::TIPO_VARIABLE){
					valor = new string;
					valor = buffer;
					tamanio = ((string*)valor)->size() + 1;
				
					listaClaves.push_back(new ClaveVariable(*((string*)valor)));
				}
					
				delete valor;
				buffer += tamanio;
			}			
	
			if (this->getTipoIndice() == ARBOL_BS){
				//Si el arbol es B*, copiar referencia al registro de datos.
				memcpy(&refRegistro, buffer, Tamanios::TAMANIO_REFERENCIA);
				buffer += Tamanios::TAMANIO_REFERENCIA;
			}
			
			//Se interpreta la referencia al hijo derecho de la clave.
			memcpy(&hijoDer, buffer, Tamanios::TAMANIO_REFERENCIA);
			
			//Crear la clave
			return new ClaveCompuesta(listaClaves, refRegistro);
		}


