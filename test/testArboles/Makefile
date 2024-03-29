# CONFIGURACION
################

# Nombre del ejecutable.
target = testArboles

# Extensión de los archivos a compilar (c para C, cpp o cc o cxx para C++).
extension = cpp

# Archivos con el código fuente que componen el ejecutable. Si no se especifica,
# toma todos los archivos con la extensión mencionada. Para especificar hay que
# descomentar la línea (quitarle el '#' del principio).
# NOTA: No poner cabeceras (.h). 

fuentes  = testArboles.cpp
fuentes += ../../CapaIndices/Manager/IndiceManager/IndiceManager.cpp
fuentes += ../../CapaIndices/BStar/BStarTree.cpp
fuentes += ../../CapaIndices/BPlus/BPlusTree.cpp
fuentes += ../../CapaIndices/BStar/NodoBStar.cpp
fuentes += ../../CapaIndices/BPlus/NodoBPlus.cpp
fuentes += ../../CapaIndices/Common/IndiceManagerFactory.cpp
fuentes += ../../Common/Bloque/Bloque.cpp
fuentes += ../../CapaIndices/Hash/Bucket.cpp
fuentes += ../../Common/Clave/Clave.cpp
fuentes += ../../ComuDatos/ComuDatos.cpp
fuentes += ../../CapaIndices/Manager/Manager.cpp
fuentes += ../../CapaIndices/Common/Nodo.cpp
fuentes += ../../CapaIndices/Common/SetClaves.cpp

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

# Uso enlazador de c++ si es código no C.
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
	@$(RM) -fv $(o_files) $(target)
