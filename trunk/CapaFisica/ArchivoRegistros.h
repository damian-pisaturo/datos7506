#ifndef ARCHIVOREGISTROS_H_
#define ARCHIVOREGISTROS_H_

#include <iostream>
#include <fstream>
using namespace std;

class ArchivoRegistros{
	
public:
	fstream  archivo;  // referencia al archivo
    unsigned int tamanioRegistro;    // tamaño en bytes del registro


public:

unsigned int getTamanioRegistro();	
	
/*
  pre : ninguna.
  post: abre el archivo, si no existe lo crea.
  
  nombre : nombre físico del archivo, path incluido.
  tamanioRegistro : tamanio en bytes de los registros del archivo.
*/
ArchivoRegistros(string nombre,unsigned int tamanioRegistro);

ArchivoRegistros(string nombre);



/*----------------------------------------------------------------------------*/
/*
  post: cierra el archivo 
*/
~ArchivoRegistros();

/*----------------------------------------------------------------------------*/
/*
  pre : Registro es un puntero a un struct del tamaño correspondiente 
  al de los registros del archivo.
  post: escribe el registro en el archivo en la posición actual.
  
  registro : puntero hacia el registro a ser escrito en el archivo.
*/
void escribir(const void* registro);

/*----------------------------------------------------------------------------*/
/*
  pre : registro es un puntero a un struct del tamaño correspondiente al de 
  		los registros del archivo.
  post: lee del archivo el registro en la posición actual.
  
  registro : puntero hacia el registro donde colocar la lectura.
*/
void leer(void* registro);
  
/*----------------------------------------------------------------------------*/
/*
  post: devuelve true si se ha llegado al fin del archivo, es decir no se puede
        leer más registros. false en caso contrario.
*/
bool fin();

/*----------------------------------------------------------------------------*/
/*
  post: devuelve la posición relativa del registro actual.
*/
long int posicion();

/*----------------------------------------------------------------------------*/
/*
  post: cambia la posición actual.
  
  posicion : número de registro (0..n)
*/
void posicionarse(unsigned int posicion);
/*----------------------------------------------------------------------------*/
/*
  post: Se posiciona al final del archivo.
*/
void posicionarseFin();
};


#endif /*ARCHIVOREGISTROS_H_*/
