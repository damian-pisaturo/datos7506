#ifndef CAPAINDICES_H_
#define CAPAINDICES_H_

#define ERROR		1
#define SEPARADOR	'='

#include <iostream>
#include "Indices/IndiceArbol.h"
#include "Indices/IndiceHash.h"
#include "../Common/DefManager/DefinitionsManager.h"
#include "../Common/Registro/Registro.h"
#include "../Common/CodigosPipe.h"
#include "../Common/OperacionesCapas.h"
#include "../Common/ClaveFactory/ClaveFactory.h"
#include "Common/BloqueListaPrimaria/BloqueListaPrimaria.h"
 
using namespace std;

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

int main(int argc, char* argv[]);


#endif /*CAPAINDICES_H_*/
