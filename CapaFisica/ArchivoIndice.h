///////////////////////////////////////////////////////////////////////////
//	Archivo   : ArchivoIndice.h
//  Namespace : CapaFisica
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clases ArchivoIndice correspodientes
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

#ifndef ARCHIVOINDICE_H_
#define ARCHIVOINDICE_H_

#include "ArchivoIndicePadre.h"

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroGriego 
//		   (Implementa archivo de indices primarios de clave entera).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceEnteroGriego: public ArchivoIndiceArbol
{
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////		
		ArchivoIndiceEnteroGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice);
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		
		/*Interpreta un buffer como una clave de tipo ClaveEntera.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);

	
}; //Fin clave ArchivoIndiceEnteroGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceEnteroRomano 
//		   (Implementa archivo de indices secundarios de clave entera).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceEnteroRomano: public ArchivoIndiceSecundario
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceEnteroRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice);

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveEntera.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase ArchivoIndiceEnteroRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceBooleanGriego 
//		   (Implementa archivo de indices primarios de clave booleana).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceBooleanGriego: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		/*Interpreta un buffer como una clave de tipo ClaveBoolean.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////		
		ArchivoIndiceBooleanGriego(unsigned int tamNodo,string nombreArchivo, t_indice tipoIndice);
	
}; //Fin clase ArchivoIndiceBooleanGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharGriego
//		   (Implementa archivo de indices primarios de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceCharGriego: public ArchivoIndiceArbol
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCharGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice);
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveBoolean.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	

	
}; //Fin clase ArchivoIndiceCharGriego

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCharRomano
//		   (Implementa archivo de indices secundarios de clave de tipo char).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceCharRomano: public ArchivoIndiceSecundario
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCharRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice);

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Interpreta un buffer como una clave de tipo ClaveChar.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase ArchivoIndiceCharGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceShortGriego
//		   (Implementa archivo de indices primarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceShortGriego: public ArchivoIndiceArbol
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceShortGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveShort.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
		
}; //Fin clase ArchivoIndiceShortGriego


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceShortRomano
//		   (Implementa archivo de indices primarios de clave de tipo short).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceShortRomano: public ArchivoIndiceSecundario
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceShortRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice);

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveShort.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
		
}; //Fin clase ArchivoIndiceShortRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceRealRomano
//		   (Implementa archivo de indices primarios de clave de tipo float).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceRealRomano: public ArchivoIndiceSecundario
{	
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceRealRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice);

	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveReal.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
}; //Fin clase ArchivoIndiceRealRomano


///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceFechaGriego
//		   (Implementa archivo de indices primarios de clave de tipo fecha).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceFechaGriego: public ArchivoIndiceArbol
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceFechaGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		/*Interpreta un buffer como una clave de tipo ClaveFecha.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);	
		
}; //Fin clase ArchivoIndiceFechaGriego

//////////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceFechaRomano
//		   (Implementa archivo de indices secundarios de clave de tipo fecha).
//////////////////////////////////////////////////////////////////////////////
class ArchivoIndiceFechaRomano: public ArchivoIndiceSecundario
{	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceFechaRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		/*Interpreta un buffer como una clave de tipo ClaveFecha.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);

	
}; //Fin clase ArchivoIndiceFechaRomano

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceVariableGriego 
//		   (Implementa archivo de indices primarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceVariableGriego: public ArchivoIndiceArbol
{	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////		
		void copiarClaveNoHoja(Clave* clave,char* &buffer);
		void copiarClaveHoja(Clave* clave,char* &buffer);
		
		/*Interpreta un buffer como una clave de tipo ClaveVariable.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
	
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceVariableGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice);	
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceVariableRomano 
//		   (Implementa archivo de indices secundarios de clave de 
//			longitud variable).
///////////////////////////////////////////////////////////////////////////
class ArchivoIndiceVariableRomano: public ArchivoIndiceSecundario
{
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceVariableRomano(unsigned int tamNodo, string nombreArchivo, unsigned int tamBloqueLista, t_indice tipoIndice);
	
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
		void copiarClaveNoHoja(Clave* clave,char* &buffer);
		void copiarClaveHoja(Clave* clave,char* &buffer);
		
		/*Interpreta un buffer como una clave de tipo ClaveVariable.*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);	
};

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: ArchivoIndiceCompuestoGriego 
//		   (Implementa archivo de indices primarios de clave compuesta).
///////////////////////////////////////////////////////////////////////////
typedef list<string*> ListaTipos;

class ArchivoIndiceCompuestoGriego: public ArchivoIndiceArbol
{
	private:
		ListaTipos* tipos;
		
	public:
	//////////////////////////////////////////////////////////////////////
	// Constructor
	//////////////////////////////////////////////////////////////////////
		ArchivoIndiceCompuestoGriego(unsigned int tamNodo, string nombreArchivo, t_indice tipoIndice, ListaTipos* listaTipos);
		
	private:
	//////////////////////////////////////////////////////////////////////
	// Metodos privados
	//////////////////////////////////////////////////////////////////////
	
		/*Esta funcion se encarga de interpretar claves compuestas
		 * variable y copiarla al buffer de escritura.
		 */
		void copiarClaveHoja(Clave* clave,char* &puntero);
		void copiarClaveNoHoja(Clave* clave,char* &puntero);
		
		/*Esta funcion se encarga de leer claves compuestas de un buffer*/
		Clave* leerClaveHoja(char* &buffer);
		Clave* leerClaveNoHoja(char* &buffer);
};

#endif /*ARCHIVOINDICE_H_*/