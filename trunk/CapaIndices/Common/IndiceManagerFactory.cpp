#include "IndiceManagerFactory.h"

IndiceManagerFactory IndiceManagerFactory::instance;

IndiceManager* IndiceManagerFactory::getIndiceManager(unsigned char tipoIndice,
													  int tipoDato, ListaTipos* listaTipos,
													  unsigned char tipoEstructura, unsigned short tamNodo,
													  unsigned int tamBucket, const string& nombreArchivo) const {
	
	if ((tipoDato != TipoDatos::TIPO_COMPUESTO) && (listaTipos))
		delete listaTipos;
	
	if ( (tipoIndice == TipoIndices::GRIEGO) || (tipoIndice == TipoIndices::ROMANO) ){
		
		switch (tipoDato) {
			
			case TipoDatos::TIPO_ENTERO:
				return new IndiceEnteroManager(tamNodo, nombreArchivo, tipoEstructura);
			case TipoDatos::TIPO_SHORT:
				return new IndiceShortManager(tamNodo, nombreArchivo, tipoEstructura);
			case TipoDatos::TIPO_CHAR:
				return new IndiceCharManager(tamNodo, nombreArchivo, tipoEstructura);
			case TipoDatos::TIPO_BOOL:
				return new IndiceBooleanManager(tamNodo, nombreArchivo, tipoEstructura);
			case TipoDatos::TIPO_FLOAT:
				return new IndiceRealManager(tamNodo, nombreArchivo, tipoEstructura);
			case TipoDatos::TIPO_FECHA:
				return new IndiceFechaManager(tamNodo, nombreArchivo, tipoEstructura);
			case TipoDatos::TIPO_STRING:
				return new IndiceVariableManager(tamNodo, nombreArchivo, tipoEstructura);
			case TipoDatos::TIPO_COMPUESTO:
				return new IndiceCompuestoManager(tamNodo, nombreArchivo, tipoEstructura, listaTipos);
			default: return NULL;
		
		}
	}else if (tipoIndice == TipoIndices::HASH)
		return new IndiceHashManager(tamBucket, nombreArchivo);
	else 
		return NULL;
	
}

