# CONFIGURACION
################

CAPA_CONSULTAS_DIR	= CapaConsultas
CAPA_FISICA_DIR		= CapaFisica
CAPA_INDICES_DIR	= CapaIndices
CAPA_METADATA_DIR	= CapaMetadata

SUBDIRS = $(CAPA_FISICA_DIR) $(CAPA_INDICES_DIR) $(CAPA_METADATA_DIR)

# REGLAS
#########

all:
	@for i in $(SUBDIRS); do \
	echo "make all in $$i..."; \
	(cd $$i; $(MAKE) $(MFLAGS) all); done

consultas:
	@echo "make all in CapaConsultas..."; \
	(cd $(CAPA_CONSULTAS_DIR); $(MAKE) $(MFLAGS) all);

fisica:
	@echo "make all in CapaFisica..."; \
	(cd $(CAPA_FISICA_DIR); $(MAKE) $(MFLAGS) all);

indices:
	@echo "make all in CapaIndices..."; \
	(cd $(CAPA_INDICES_DIR); $(MAKE) $(MFLAGS) all);

metadata:
	@echo "make all in CapaMetadata..."; \
	(cd $(CAPA_METADATA_DIR); $(MAKE) $(MFLAGS) all);

clean:
	@for i in $(SUBDIRS); do \
	echo "Clearing in $$i..."; \
	(cd $$i; $(MAKE) $(MFLAGS) clean); done

