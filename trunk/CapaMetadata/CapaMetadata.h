#ifndef CAPAMETADATA_H_
#define CAPAMETADATA_H_

#include "../Common/CodigosPipe.h"
#include "../Common/ResultadosMetadata.h"
#include "../Common/ResultadosIndices.h"
#include "../Common/ImpresorResultado.h"
#include "../Common/NombresCapas.h"
#include "../Common/OperacionesCapas.h"
#include "../Common/ExpresionesLogicas.h"
#include "../ComuDatos/ComuDatos.h"
#include "../Common/DefManager/DefinitionsManager.h"
#include "DataManager/DataManager.h"
#include "FileManager/FileManager.h"
#include <string>

using namespace std;

#define EXTENSION_TEMPORAL_INICIAL	"tmp0"
#define EXTENSION_ESPACIO_LIBRE		"nfo"

ComuDatos* instanciarPipe();
void serializarListaClaves(string& s, ListaInfoClave* listaClaves);
bool cumpleRestricciones(char* registro, ListaOperaciones &listaOperaciones, 
						 char operacion, ListaNombresAtributos &listaNombresAtributos, 
						 ListaTiposAtributos &listaTiposAtributos);
int main(int argc, char* argv[]);

#endif /*CAPAMETADATA_H_*/
