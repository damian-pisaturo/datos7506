# CONFIGURACION
################

# Nombre del ejecutable.
target = capaIndices

# Extensión de los archivos a compilar (c para C, cpp o cc o cxx para C++).
extension = cpp

# Archivos con el código fuente que componen el ejecutable. Si no se especifica,
# toma todos los archivos con la extensión mencionada. Para especificar hay que
# descomentar la línea (quitarle el '#' del principio).
# NOTA: No poner cabeceras (.h).

fuentes = Manager/Manager.cpp
fuentes += Manager/BloqueManager/BloqueDatosManager/BloqueDatosManager.cpp 
fuentes += Manager/BloqueManager/BloqueListaManager/BloqueListaManager.cpp
fuentes += Manager/IndiceManager/IndiceManager.cpp
fuentes += Indices/IndiceArbol.cpp
fuentes += Indices/IndiceHash.cpp
fuentes += Common/Nodo.cpp
fuentes += Common/SetClaves.cpp
fuentes += Common/IndiceManagerFactory.cpp
fuentes += Common/BloqueListaPrimaria/BloqueListaPrimaria.cpp
fuentes += BPlus/BPlusTree.cpp
fuentes += BPlus/NodoBPlus.cpp
fuentes += BStar/BStarTree.cpp
fuentes += BStar/NodoBStar.cpp
fuentes += Hash/Hash.cpp
fuentes += Hash/Bucket.cpp
fuentes += Hash/Tabla.cpp
fuentes += ../Common/RegisterInfo/RegisterInfo.cpp
fuentes += ../Common/Bloque/Bloque.cpp
fuentes += ../Common/Clave/Clave.cpp
fuentes += ../Common/ClaveFactory/ClaveFactory.cpp
fuentes += ../Common/DefManager/DefinitionsManager.cpp
fuentes += ../Common/Registro/Registro.cpp
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
