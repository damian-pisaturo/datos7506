///////////////////////////////////////////////////////////////////////////
//	Archivo   : Bloque.h
//  Namespace : CapaIndice 
////////////////////////////////////////////////////////////////////////////
//	75.06 Organizacion de Datos
//	Trabajo practico: Framework de Persistencia
////////////////////////////////////////////////////////////////////////////
//	Descripcion
//		Cabeceras e interfaz de las clase Bloque.
///////////////////////////////////////////////////////////////////////////
//	Integrantes
//		- Alvarez Fantone, Nicolas;
//      - Caravatti, Estefania;
//		- Garcia Cabrera, Manuel;
//      - Grisolia, Nahuel;
//		- Pisaturo, Damian;	
//		- Rodriguez, Maria Laura.
///////////////////////////////////////////////////////////////////////////

#ifndef BLOQUE_H_
#define BLOQUE_H_
 
#include <cstdio>
#include <cstdlib>
#include <string>
#include <list>

#include "../../Common/Tamanios.h"
#include "../../Common/TipoDatos.h"
#include "../../Common/RegisterInfo/RegisterInfo.h"
#include "../Clave/Clave.h"
#include "../../Common/ResultadosIndices.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Clase
//------------------------------------------------------------------------
// Nombre: Bloque (Implementa manejo y control de bloques en memoria)
///////////////////////////////////////////////////////////////////////////
class Bloque
{
	private:	
	////////////////////////////////////////////////////////////////
	// Atributos
	////////////////////////////////////////////////////////////////
		unsigned int numero;		// Representa al numero de bloque dentro del archivo.
		unsigned int tamanio;		// Es el tamaño del bloque en bytes.
		unsigned int offsetADatos;	// Es el offset al primer registro del bloque.
		char *datos;				// Es una cadena de bytes tal cual se persiste en disco.
	
	///////////////////////////////////////////////////////////////////////
	// Metodos privados
	///////////////////////////////////////////////////////////////////////

		/*
		 * Organiza el bloque a luego de una baja dentro del mismo
		 **/
		void organizarBloque(int offsetToReg, int longReg);
		/*
		 * Inserta un registro dentro del bloque, actualizando el offset a espacio libre
		 **/
		void insertarRegistro(char *registro, unsigned short nuevoOffsetEspLibre, unsigned short longitudRegistro);
		
		/*
		 * Retorna un registro a partir del offset del mismo dentro del bloque
		 * y la longitud del registro
		 * */
		char* getRegistro(int longReg, int offsetToReg);	
		
		/**
		 * Retorna un campo específico del registro
		 * */
		char* getRegisterAtribute(string registro, int offsetCampo, int longCampo);

	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		Bloque();
		Bloque(unsigned int num,unsigned int tam);
		virtual ~Bloque();
	
	///////////////////////////////////////////////////////////////////////
	// Métodos públicos
	///////////////////////////////////////////////////////////////////////

		/*
		 * Busca al registro de clave "clavePrimaria".
		 * Si lo encuentra devuelve true y el offset al registro en "offsetReg"; de lo contrario, 
		 * devuelve false.
		 **/
		bool buscarRegistro(const ListaNodos *listaParam, Clave &clavePrimaria, unsigned short* offsetReg);
		
		/*
		 * Inserta un nuevo registro dentro del bloque
		 **/
		int altaRegistro(const ListaNodos *listaParam, char *registro);
		
		/*
		 * Elimina un registro del bloque, reorganizando el espacio libre
		 **/
		int bajaRegistro(const ListaNodos *listaParam, Clave &clavePrimaria);
		
		/*
		 * Modifica el contenido de un registro.
		 **/
		int modificarRegistro(const ListaNodos *listaParam, unsigned short longReg,Clave &clavePrimaria, 
							  char* registro);
		
		/*
		 * Verifica que el registro a insertar tiene espacio dentro del bloque
		 * Retorna "true" en caso de que haya espacio y "false" en caso contrario
		 **/
		bool verificarEspacioDisponible(unsigned short longReg, unsigned short offsetEspLibre);
		
		/*
		 * Devuelve la longitud del registro, ya sea de longitud fija o varaible.
		 **/
		unsigned short getTamanioRegistros(const ListaNodos *listaParam,char *registro);
		
		/*
		 * Devuelve la clave primaria de "registro".
		 **/
		Clave*  getClavePrimaria(const ListaNodos *listaParam, char* registro);
		
		/*
		 * Devuelve el tamaño del bloque.
		 **/
		unsigned int getTamanioBloque();
		
		/*
		 * Setea el tamaño del bloque.
		 **/
		void setTamanioBloque(unsigned int tam);
		
		/*
		 * Devuelve un puntero al contenido del bloque.
		 **/
		char * getDatos();
		
		/*
		 * Asigna el contenido al bloque.
		 **/
		void setDatos(char* data);
		
		/*
		 * Devuelve el némero del bloque.
		 **/
		unsigned int getNroBloque();
		
		/*
		 * Setea el número del bloque.
		 **/
		void setNroBloque(unsigned int num);
		
		unsigned short getOffsetADatos() const;
		
		void setOffsetADatos(unsigned short offset);

};

#endif /*BLOQUE_H_*/
