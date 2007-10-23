#ifndef BLOQUE_H_
#define BLOQUE_H_
 
#define OK 0;
#define FAIL 1;

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>

using namespace std;

class Bloque
{
private:
	int numero;
	int tamanio;
	char *datos;
	
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
	void insertarRegistro(char *registro,int nuevoOffsetEspLibre);
	
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
	Bloque(int num,int tam);
	virtual ~Bloque();
	
	/*
	 * Inserta un nuevo registro dentro del bloque
	 **/
	int altaRegistro(char *registro);
	
	/*
	 * Elimina un registro del bloque, reorganizando el espacio libre
	 **/
	int bajaRegistro(list <string>listaParam,void *clavePrimaria);
	
	/*
	 * Modifica el contenido de un registro 
	 **/
	int modificarRegistro(list <string>listaParam,int longReg);
	
	
};

#endif /*BLOQUE_H_*/
