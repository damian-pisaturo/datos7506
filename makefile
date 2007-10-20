# Makefile de ejemplo para C++
# 
# Creado: jue abr 15 15:34:19 ART 2004
#
# Copyleft 2004 - Leandro Lucarella, Bajo licencia GPL [http://www.gnu.org/]
#

# CONFIGURACION
################

# Nombre del ejecutable.
target = datos

# Extensi�n de los archivos a compilar (c para C, cpp o cc o cxx para C++).
extension = cpp

# Archivos con el c�digo fuente que componen el ejecutable. Si no se especifica,
# toma todos los archivos con la extensi�n mencionada. Para especificar hay que
# descomentar la l�nea (quitarle el '#' del principio).
# NOTA: No poner cabeceras (.h). 
fuentes = CapaIndices\Common\Clave.cpp

# Si es un programa GTK+, descoment� (quitale el '#' a) la siguiente l�nea.
#gtk = si 

# Si usa funciones de math.h, descoment� (quitale el '#' a) la siguiente l�nea.
#math = si 


# CONFIGURACION "AVANZADA"
###########################

# Opciones para el compilador C/C++ en modo ansi.
CFLAGS = -Wall -ansi -pedantic-errors

# Para que explote lo mas posible
#CFLAGS += -O3 -DNDEBUG

# Para valgrind o debug
CFLAGS += -ggdb -DDEBUG

# Opciones para el compilador C++.
CXXFLAGS = $(CFLAGS) -fno-inline

# Opciones del enlazador.
#LDFLAGS=


# VARIABLES CALCULADAS A PARTIR DE LA CONFIGURACION
####################################################

# Agrego flags y libs de GTK+ de ser necesario.
ifdef gtk
CFLAGS += $(shell pkg-config --cflags gtk+-2.0)
CXXFLAGS += $(shell pkg-config --cflags gtk+-2.0)
LDFLAGS += $(shell pkg-config --libs gtk+-2.0)
endif

# Linkeo con libm de ser necesario.
ifdef math
LDFLAGS += -lm
endif

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
#	@if [ -z "$(o_files)" ]; \
#	then \
#		echo "No hay archivos de entrada, recuerde que la extensi�n debe ser '.$(extension)'."; \
#		false; \
#	fi >&2
	$(enlazador) $(LDFLAGS) $(o_files) $(LOADLIBES) $(LDLIBS) -o $(target)

clean:
	@$(RM) -fv *.o $(target)

