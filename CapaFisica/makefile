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

fuentes = CapaFisica/CapaFisica.cpp
fuentes += CapaFisica/ArchivoBase/ArchivoBase.cpp
fuentes += CapaFisica/ArchivoDatos/ArchivoDatos.cpp
fuentes += CapaFisica/ArchivoEL/ArchivoEL.cpp
fuentes += CapaFisica/ArchivoIndice/ArchivoIndice.cpp
fuentes +=  CapaIndices/Manager/IndiceManager.cpp
fuentes += CapaIndices/Bloque/Bloque.cpp
fuentes += CapaIndices/Common/Nodo.cpp CapaIndices/Common/Clave.cpp CapaIndices/Common/SetClaves.cpp
fuentes += CapaIndices/BPlus/NodoBPlus.cpp
fuentes += CapaIndices/BStar/NodoBStar.cpp
fuentes += CapaIndices/Hash/Hash.cpp CapaIndices/Hash/Bucket.cpp CapaIndices/Hash/Tabla.cpp CapaIndices/Hash/RegistroV.cpp
fuentes += ComuDatos/ComuDatos.cpp

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
