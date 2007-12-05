#ifndef CAPAMETADATA_H_
#define CAPAMETADATA_H_

#include "../Common/CodigosPipe.h"
#include "../Common/ResultadosMetadata.h"
#include "../Common/ResultadosIndices.h"
#include "../Common/ImpresorResultado.h"
#include "../Common/NombresCapas.h"
#include "../Common/ExpresionesLogicas.h"
#include "../ComuDatos/ComuDatos.h"
#include "../Common/DefManager/DefinitionsManager.h"
#include "Parser/ParserOperaciones.h"
#include "DataManager/DataManager.h"
#include "Vista/Vista.h"
#include <string>

using namespace std;

ComuDatos* instanciarPipe();
void serializarListaClaves(string& s, DefinitionsManager::ListaClaves* listaClaves);
bool cumpleRestricciones(char* registro, DefinitionsManager::ListaOperaciones &listaOperaciones, 
						 char operacion, DefinitionsManager::ListaNombresAtributos &listaNombresAtributos, 
						 DefinitionsManager::ListaTiposAtributos &listaTiposAtributos);
int main(int argc, char* argv[]);

#endif /*CAPAMETADATA_H_*/
