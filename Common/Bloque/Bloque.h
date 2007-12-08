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
#include "../../Common/ResultadosIndices.h"
#include "../Clave/Clave.h"

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
		unsigned int numero;		  // Representa al numero de bloque dentro del archivo.
		unsigned int tamanio;		  // Es el tamaño del bloque en bytes.
		unsigned int offsetADatos;	  // Es el offset al primer registro del bloque.
		char *datos;				  // Es una cadena de bytes tal cual se persiste en disco.
		unsigned int offsetToProxReg; // Es el offset al proximo registro, utilizado para recuperar todos los registros de un bloque.
		int tipoOrganizacion;         // Indica si la organizacion del bloque es de registros variables o fijos.
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
		
		/**
		 * Retorna un campo específico del registro
		 * */
		char* getRegisterAtribute(string registro, int offsetCampo, int longCampo) const;
		
		
	
	public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
		Bloque();
		Bloque(unsigned int num, unsigned int tam);
		Bloque(unsigned int numBloque, unsigned int tamanioBloque, int tipoOrga); 
		virtual ~Bloque();
	
	///////////////////////////////////////////////////////////////////////
	// Métodos públicos
	///////////////////////////////////////////////////////////////////////

		/*
		 * Busca al registro de clave "clavePrimaria".
		 * Si lo encuentra devuelve true y el offset al registro en "offsetReg"; de lo contrario, 
		 * devuelve false.
		 **/
		bool buscarRegistro(const ListaInfoRegistro *listaInfoReg, Clave &clavePrimaria, unsigned short *offsetReg);
		
		/*
		 * Inserta un nuevo registro dentro del bloque
		 **/
		int altaRegistro(const ListaInfoRegistro *listaInfoReg, char *registro);
		
		/*
		 * Elimina un registro del bloque, reorganizando el espacio libre
		 **/
		int bajaRegistro(const ListaInfoRegistro *listaInfoReg, Clave &clavePrimaria);
		
		/*
		 * Modifica el contenido de un registro.
		 **/
		int modificarRegistro(const ListaInfoRegistro *listaInfoReg, unsigned short longReg, Clave &clavePrimaria, 
							  char *registro);
		
		/*
		 * Verifica que el registro a insertar tiene espacio dentro del bloque
		 * Retorna "true" en caso de que haya espacio y "false" en caso contrario
		 **/
		bool verificarEspacioDisponible(unsigned short longReg, unsigned short offsetEspLibre);
		
		/*
		 * Devuelve la longitud del registro, ya sea de longitud fija o varaible.
		 * No incluye los bytes que se utilizan para guardar la longitud de los registros variables.
		 **/
		unsigned short getTamanioRegistros(const ListaInfoRegistro *listaInfoReg, char *registro) const;

		/*
		 * Devuelve la longitud del registro, ya sea de longitud fija o varaible.
		 * En este último caso incluye los bytes utilizados para guardar la longitud del mismo.
		 **/
		unsigned short getTamanioRegistroConPrefijo(const ListaInfoRegistro *listaInfoReg, char *registro) const;
		
		/*
		 * Devuelve la clave primaria de "registro".
		 **/
		Clave*  getClavePrimaria(const ListaInfoRegistro *listaInfoReg, char *registro);
		
		/*
		 * Devuelve el tamaño del bloque.
		 **/
		unsigned int getTamanioBloque() const;
		
		/*
		 * Setea el tamaño del bloque.
		 **/
		void setTamanioBloque(unsigned int tam);
		
		/*
		 * Devuelve un puntero al contenido del bloque.
		 **/
		char* getDatos();
		
		/*
		 * Asigna el contenido al bloque.
		 **/
		void setDatos(char* data);
		
		/*
		 * Devuelve el número del bloque.
		 **/
		unsigned int getNroBloque() const;
		
		/*
		 * Setea el número del bloque.
		 **/
		void setNroBloque(unsigned int num);
		
		unsigned short getOffsetADatos() const;
		
		void setOffsetADatos(unsigned short offset);
		
		static char* serializarClave(Clave* clave, const ListaTipos* listaTipos);
		
		/*
		 * Obtiene el proximo registro dentro del bloque
		 * */
		char* getNextRegister();
		
		/*
		 * Asigna el tipo de organizacion al bloque
		 * */
		void setTipoOrganizacion(int tipo);
		
		int getTipoOrganizacion() const;
		
		void setOffsetToReg(unsigned int offset);
		
		unsigned int getOffsetToReg();
		
		/*
		 * Resetea el offset a los registros al primero del bloque
		 * */
		void resetOffsetToReg();
		
		unsigned short getCantidadRegistros();
		
		/*
		 * Si el tipo de organizacion es de registros de longitud fija calcula el tamanio de los mismos.
		 * Si es de longitud variable, obtiene la longitud del registro corriente
		 * */
		unsigned short getTamanioRegistros() const;
		
		/*
		 * Este método limpia el bloque poniendo la cantidad de registros en cero y actualizando el
		 * offset al espacio libre.
		 */
		void clear();
		
		/*
		 * Retorna un registro a partir del offset del mismo dentro del bloque
		 * y la longitud del registro
		 * */
		char* getRegistro(int longReg, int offsetToReg) const;
		
		/*
		 * Retorna el registro que se encuentra en la posición 'numReg',
		 * comenzando desde 0. En 'tamReg' devuelve el tamaño del mismo.
		 * */
		char* getRegistroPorNro(unsigned short numReg, unsigned short &tamReg) const;
		
		unsigned short getTamanioEspacioLibre() const;

};

#endif /*BLOQUE_H_*/
