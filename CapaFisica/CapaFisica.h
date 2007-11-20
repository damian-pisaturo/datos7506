#ifndef CAPAFISICA_H_
#define CAPAFISICA_H_

#include "ArchivoIndice/ArchivoIndice.h"
#include "ArchivoDatos/ArchivoDatos.h"
#include "../Common/ResultadosFisica.h"
#include "../Common/OperacionesCapas.h"
#include "../ComuDatos/ComuDatos.h"
#include "../CapaIndices/Common/IndiceManagerFactory.h"
#include "../CapaIndices/BPlus/NodoBPlus.h"
#include <string>

using namespace std;

char ejecutarOperacion(char** args);
int main(int argc, char**argv);

#endif /*CAPAFISICA_H_*/
