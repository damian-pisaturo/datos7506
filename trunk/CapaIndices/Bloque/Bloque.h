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

using namespace std;

class Bloque
{
private:
	int numero;
	int tamanio;
	char *datos;
	
public:
	Bloque();
	Bloque(int num,int tam);
	virtual ~Bloque();
	
private:
	//Métodos privados
	
	/*
	 * Organiza el bloque a partir de una modificación dentro del mismo
	 **/
	
	void organizarBloque(int offsetToReg,int longReg);
	/*
	 * Verifica que el registro a insertar tiene espacio dentro del bloque
	 * Retorna "true" en caso de que haya espacio y "false" en caso contrario
	 **/
	bool verificarEspacioDisponible(int longReg,int offsetEspLibre);
	
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
	
	bool buscarRegistro(const list<string>& listaParam, void *clavePrimaria, unsigned short* offsetReg);

public:
	/*
	 * Inserta un nuevo registro dentro del bloque
	 **/
	bool altaRegistro(char *registro);
	
	/*
	 * Elimina un registro del bloque, reorganizando el espacio libre
	 **/
	int bajaRegistro(list <string>listaParam,void *clavePrimaria);
	/*
	 * Modifica el contenido de un registro 
	 **/
	int modificarRegistro(const list<string>& listaParam, unsigned short longReg, char* registro);
	
};

#endif /*BLOQUE_H_*/
