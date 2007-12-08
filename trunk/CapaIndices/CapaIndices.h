#ifndef CAPAINDICES_H_
#define CAPAINDICES_H_

#include <iostream>
#include "Indices/IndiceArbol.h"
#include "Indices/IndiceHash.h"
#include "Common/BloqueListaPrimaria/BloqueListaPrimaria.h"
#include "../Common/DefManager/DefinitionsManager.h"
#include "../Common/Registro/Registro.h"
#include "../Common/CodigosPipe.h"
#include "../Common/OperacionesCapas.h"
#include "../Common/ClaveFactory/ClaveFactory.h"
#include "../Common/ExpresionesLogicas.h"
 
using namespace std;

// Declaración global de la lista de los nombres de los campos
// que componen la clave por la cual se realiza la operación.
ListaNombresClaves listaNombresClaves;


/*
 * Crea todos los índices correspondientes a un tipo, y carga su mapa de índices.
 **/
void crearIndices(const string &nombreTipo, MapaIndices &mapaIndices,
				  DefinitionsManager &defManager);

/*
 * Destruye los índices que se crearon para llevar a cabo una operación
 */
void destruirIndices(MapaIndices &mapaIndices);

/*
 * Procesa la operación que le llega desde la Capa de Metadata
 */
int procesarOperacion(unsigned char codOp, const string &nombreTipo, ComuDatos &pipe);

void eliminar(const string &nombreTipo, MapaIndices &mapaIndices,
	   	  	  Indice *indice, Clave *clave,
	   	  	  DefinitionsManager &defManager, ComuDatos &pipe);

void modificar(const string &nombreTipo, MapaIndices &mapaIndices,
 	  	  	   Indice *indice, Clave *clave,
 	  	  	   DefinitionsManager &defManager, ComuDatos &pipe);

void insertar(const string &nombreTipo, MapaIndices &mapaIndices,
		   	  Indice *indice, Clave *clave,
		   	  DefinitionsManager &defManager, ComuDatos &pipe);

int main(int argc, char* argv[]);


#endif /*CAPAINDICES_H_*/
