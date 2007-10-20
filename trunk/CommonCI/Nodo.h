#ifndef NODO_H_
#define NODO_H_

#ifndef NULL
#define NULL 0
#endif

#include "Lista.h"
#include "ArchivoIndicePadre.h"
#include "BpTree.h"

/*Pre-Definition*/
class ArchivoIndice;
class Clave;
class ListaClaves;
class Codigo;

class Nodo{
      private:

              /*Header del Nodo - Info que no son claves
               * pero que se almacena en el archivo*/
              int hijoIzq;
              int nivel;
              int espacioLibre;
                     
              int posicionEnArchivo;
              ListaClaves* claves;
             
              
      public:
              
             /*Constructores*/
             
             /*Crea el nodo sin setear ningun atributo*/
             Nodo (int hijoIzq,int nivel,ArchivoIndice* archivo);
             /*Crea un nuevo nodo para insertarle una Clave*/
             Nodo(int hijoIzq,int nivel,Clave* clave,ArchivoIndice* archivo);
             /*Lee el archivo y crea ese nodo*/
             Nodo(ArchivoIndice* archivoIndice,int referencia);
             
             /*Insercion*/
		     void insertarEnHoja(ArchivoIndice* archivoIndice,Clave* &clave,Codigo* codigo);
             void insertarEnNodo(ArchivoIndice* archivoIndice,Clave* &clave,Codigo* codigo);
        
             
			 void actualizarEspacioLibre(ListaClaves* claves, bool insercion);          
			 void actualizarEspacioLibre(Clave* clave, bool insercion);
			 void actualizarEspacioLibre(ArchivoIndice* archivo);
             /*Siguiente*/		
             Nodo* siguiente(ArchivoIndice* archivo,Clave* clave);
             
             /*Eliminar*/
             /*tiene que quitar la clave o la mas cercana ARREGLAR*/
             void quitarClave(ArchivoIndice* archivo,Clave* clave,Codigo* codigo);
             bool soportaSplit(Nodo* nodoConSubflow);
             int condicionMinima(ArchivoIndice* archivo);
             Clave* claveSiguiente(Clave* clave);
             Clave* reemplazarClave(Clave* claveVieja,Clave* claveNueva,Codigo* codigo);
             
             /*inserta la lista al final de  la existente en el nodo, graba el nodo en el archivo , actualiza el espcio libre*/
             void insertarFinal(ListaClaves* lista,ArchivoIndice* archivoIndice);
             
             /*Chequea si puede donar*/
             bool puedeDonar(ArchivoIndice* archivo);
             Clave* buscar(Clave* claveBuscada);
             /*Setters*/
             void setHijoIzq(int hijoIzq);
             void setNivel(int nivel);
             void setEspacioLibre(int cant);
             
             /*----------*/
             void setPosicionEnArchivo(int posicion);
             void setClaves(ListaClaves* lista);
			
             /*Getters*/
             int getHijoIzq();
             int getNivel();
             int obtenerPosicionEnArchivo();
             ListaClaves* obtenerClaves();
             int getEspacioLibre();
             bool vacio(ArchivoIndice* archivo);
             void primero();
             Clave* obtenerClaveActual();
             bool siguiente();
             
             /*Devuelven referencias a archivos de las claves anterior y posterior a la clave dada
              * devuelven -1 si la clave no tiene referencia posterior o anterior*/
             int refAnterior(Clave* clave);
             int refPosterior(Clave* clave);
             
             /*Destructor*/             
             Nodo::~Nodo();          
};            

#endif /*NODO_H_*/
