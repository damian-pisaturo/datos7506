#ifndef BLOQUE_H_
#define BLOQUE_H_
 
#define OK 				0
#define NO_ENCONTRADO 	1
#define SOBREFLUJO		2

#include <cstdio>
#include <cstdlib>
#include <string>
#include <list>

#include "../../Common/Tamanios.h"
#include "../../Common/TipoDatos.h"
#include "../RegisterInfo/RegisterInfo.h"
#include "../Common/Clave.h"

using namespace std;

class Bloque
{
private:	
	////////////////////////////////////////////////////////////////
	// Atributos
	////////////////////////////////////////////////////////////////
	unsigned int numero;
	unsigned int tamanio;
	char *datos;
	
	//Métodos privados
	
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
	int modificarRegistro(const ListaNodos *listaParam, unsigned short longReg,Clave &clavePrimaria, char* registro);
	
	
	
	/*
	 * Devuelve el offset al primer registro del bloque.
	 **/
	virtual unsigned short getOffsetToRegs();
	
	/*
	 * Verifica que el registro a insertar tiene espacio dentro del bloque
	 * Retorna "true" en caso de que haya espacio y "false" en caso contrario
	 **/
	bool verificarEspacioDisponible(int longReg,int offsetEspLibre);
	
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
	
};

#endif /*BLOQUE_H_*/
