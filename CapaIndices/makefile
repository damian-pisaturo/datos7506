# CONFIGURACION
################

# Nombre del ejecutable.
target = capaIndices

# Extensi�n de los archivos a compilar (c para C, cpp o cc o cxx para C++).
extension = cpp

# Archivos con el c�digo fuente que componen el ejecutable. Si no se especifica,
# toma todos los archivos con la extensi�n mencionada. Para especificar hay que
# descomentar la l�nea (quitarle el '#' del principio).
# NOTA: No poner cabeceras (.h). 

fuentes = RegisterInfo/RegisterInfo.cpp
fuentes += Manager/IndiceManager.cpp
fuentes += Indices/Indice.cpp
fuentes += Indices/IndiceArbol.cpp
fuentes += Indices/IndiceHash.cpp
fuentes += Bloque/Bloque.cpp
fuentes += Common/Nodo.cpp
fuentes += Common/Clave.cpp
fuentes += Common/SetClaves.cpp
fuentes += ../Common/DefinitionsManager.cpp
fuentes += Common/IndiceManagerFactory.cpp
fuentes += BTree/BTree.cpp
fuentes += BPlus/BPlusTree.cpp
fuentes += BPlus/NodoBPlus.cpp
fuentes += BStar/BStarTree.cpp
fuentes += BStar/NodoBStar.cpp
fuentes += Hash/Hash.cpp
fuentes += Hash/Bucket.cpp
fuentes += Hash/Tabla.cpp
fuentes += ../ComuDatos/ComuDatos.cpp
fuentes += CapaIndices.cpp

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
