#ifndef BLOQUE_H_
#define BLOQUE_H_
 
#define OK 				0
#define NO_ENCONTRADO 	1
#define OVERFLOW		2

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>

#include "../Common/Tamanios.h"
#include "../Common/TipoDatos.h"
#include "../RegisterInfo/RegisterInfo.h"

using namespace std;

class Bloque
{
private:	
	////////////////////////////////////////////////////////////////
	// Atributos
	////////////////////////////////////////////////////////////////
	int numero;
	unsigned int tamanio;
	char *datos;
	
	//Métodos privados
	
	/*CapaIndices/Bloque/Bloque.cpp 
	 * Organiza el bloque a partir de una modificación dentro del mismo
	 **/
	
	void organizarBloque(int settamDispersionToReg,int longReg);
	/*
	 * Inserta un registro dentro del bloque, actualizando el offset a espacio libre
	 **/
	void insertarRegistro(char *registro,unsigned short nuevoOffsetEspLibre,unsigned short longitudRegistro);
	
	/*
	 * Retorna un registro a partir del offset del mismo dentro del bloque
	 * y la longitud del registro
	 * */
	char* getRegistro(int longReg, int offsetToReg);	
	
	/**
	 * Retorna un campo específico del registro
	 * */
	char* getRegisterAtribute(string registro,int offsetCampo,int longCampo);

	
public:
	///////////////////////////////////////////////////////////////////////
	// Constructor/Destructor
	///////////////////////////////////////////////////////////////////////
	Bloque();
	Bloque(int num,int tam);
	virtual ~Bloque();
	
	///////////////////////////////////////////////////////////////////////
	// Métodos públicos
	///////////////////////////////////////////////////////////////////////
	bool buscarRegistro(const list<nodoLista>& listaParam, void *clavePrimaria, unsigned short* offsetReg);
	/*
	 * Inserta un nuevo registro dentro del bloque
	 **/
	bool altaRegistro(const list<nodoLista>& listaParam, char *registro);
	
	/*
	 * Elimina un registro del bloque, reorganizando el espacio libre
	 **/
	int bajaRegistro(const list <nodoLista>& listaParam,void *clavePrimaria);
	/*
	 * Modifica el contenido de un registro.
	 **/
	int modificarRegistro(const list<nodoLista>& listaParam, unsigned short longReg,void* clavePrimaria, char* registro);
	
	virtual unsigned short getOffsetToRegs();
	/*
	 * Verifica que el registro a insertar tiene espacio dentro del bloque
	 * Retorna "true" en caso de que haya espacio y "false" en caso contrario
	 **/
	bool verificarEspacioDisponible(int longReg,int offsetEspLibre);
	
	/*
	 * Devuelve la longitud del registro, ya sea de longitud fija o varaible.
	 **/
	unsigned short getTamanioRegistros(const list<nodoLista>& listaParam,char *registro);
	
	unsigned int getTamanioBloque();
	
	void setTamanioBloque(unsigned int tam);
	
	char * getDatos();
	
	void setDatos(char* d);
	
	int getNroBloque();
	
	void setNroBloque(int num);
	
};

#endif /*BLOQUE_H_*/