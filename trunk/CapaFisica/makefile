# CONFIGURACION
################

# Nombre del ejecutable.
target = capaFisica

# Extensi�n de los archivos a compilar (c para C, cpp o cc o cxx para C++).
extension = cpp

# Archivos con el c�digo fuente que componen el ejecutable. Si no se especifica,
# toma todos los archivos con la extensi�n mencionada. Para especificar hay que
# descomentar la l�nea (quitarle el '#' del principio).
# NOTA: No poner cabeceras (.h). 

fuentes = ../CapaIndices/Bloque/Bloque.cpp 
fuentes += ../CapaIndices/Common/Nodo.cpp
fuentes += ../CapaIndices/Common/Clave.cpp
fuentes += ../CapaIndices/Common/SetClaves.cpp 
fuentes += ../CapaIndices/BStar/NodoBStar.cpp
fuentes += ../CapaIndices/BStar/BStarTree.cpp 
fuentes += ../CapaIndices/BPlus/NodoBPlus.cpp
fuentes += ../CapaIndices/BPlus/BPlusTree.cpp 
fuentes += ../CapaIndices/BTree/BTree.cpp 
fuentes += ../ComuDatos/ComuDatos.cpp 
fuentes += ../CapaIndices/Hash/Hash.cpp 
fuentes += ../CapaIndices/Hash/Bucket.cpp 
fuentes += ../CapaIndices/Hash/Tabla.cpp 
fuentes += ../CapaIndices/RegisterInfo/RegisterInfo.cpp
fuentes += ../CapaMetadata/DataManager/DataManager.cpp 
fuentes += ../CapaIndices/Indices/Indice.cpp 
fuentes += ../CapaIndices/Indices/IndiceHash.cpp 
fuentes += ../CapaIndices/Indices/IndiceArbol.cpp 
fuentes += ../CapaIndices/Manager/IndiceManager.cpp 
fuentes += ../CapaIndices/Common/ClaveFactory.cpp 
fuentes += ../CapaIndices/Common/IndiceManagerFactory.cpp 
fuentes += CapaFisica.cpp 
fuentes += ArchivoBase/ArchivoBase.cpp
fuentes += ArchivoDatos/ArchivoDatos.cpp 
fuentes += ArchivoEL/ArchivoEL.cpp
fuentes += ArchivoIndice/ArchivoIndice.cpp

# CONFIGURACION "AVANZADA"
###########################

# Opciones para el compilador C/C++ en modo ansi.
CFLAGS = -Wall -ansi -pedantic-errors

# Para valgrind o debug
CFLAGS += -ggdb -DDEBUG

# Opciones para el compilador C++.
CXXFLAGS = $(CFLAGS) -fno-inline

# Opciones del enlazador.
#LDFLAGS=


# VARIABLES CALCULADAS A PARTIR DE LA CONFIGURACION
####################################################

# Uso enlazador de c++ si es c�digo no C.
ifeq ($(extension), c)
enlazador = $(CC)
else
enlazador = $(CXX)
endif

# Si no especifica archivos, tomo todos.
fuentes ?= $(wildcard *.$(extension))


# REGLAS
#########

.PHONY: all clean

all: $(target)

o_files = $(patsubst %.$(extension),%.o,$(fuentes))

$(target): $(o_files)
	$(enlazador) $(LDFLAGS) $(o_files) $(LOADLIBES) $(LDLIBS) -o $(target)

clean:
	@$(RM) -fv *.o $(target)
