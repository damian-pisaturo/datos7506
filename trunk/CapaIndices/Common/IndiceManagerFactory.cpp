#include "IndiceManagerFactory.h"

IndiceManagerFactory IndiceManagerFactory::instance;

IndiceManager* IndiceManagerFactory::getIndiceManager(unsigned char tipoIndice, unsigned short tamBloqueLista,
													  int tipoDato, ListaTipos* listaTipos,
													  unsigned char tipoEstructura, unsigned short tamNodo,
													  unsigned int tamBucket, const string& nombreArchivo) const {
	
	switch (tipoIndice) {
	
		case TipoIndices::GRIEGO:
			
			switch (tipoDato) {
				
				case TipoDatos::TIPO_ENTERO:
					return new IndiceEnteroGriegoManager(tamNodo, nombreArchivo, tipoEstructura);
				case TipoDatos::TIPO_SHORT:
					return new IndiceShortGriegoManager(tamNodo, nombreArchivo, tipoEstructura);
				case TipoDatos::TIPO_CHAR:
					return new IndiceCharGriegoManager(tamNodo, nombreArchivo, tipoEstructura);
				case TipoDatos::TIPO_BOOL:
					return NULL;
				case TipoDatos::TIPO_FLOAT:
					return NULL;
				case TipoDatos::TIPO_FECHA:
					return new IndiceFechaGriegoManager(tamNodo, nombreArchivo, tipoEstructura);
				case TipoDatos::TIPO_STRING:
					return new IndiceVariableGriegoManager(tamNodo, nombreArchivo, tipoEstructura);
				case TipoDatos::TIPO_COMPUESTO:
					return new IndiceCompuestoGriegoManager(tamNodo, nombreArchivo, tipoEstructura, listaTipos);
			
			}
			
		case TipoIndices::ROMANO:
					
			switch (tipoDato) {
				
				case TipoDatos::TIPO_ENTERO:
					return new IndiceEnteroRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura);
				case TipoDatos::TIPO_SHORT:
					return new IndiceShortRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura);
				case TipoDatos::TIPO_CHAR:
					return new IndiceCharRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura);
				case TipoDatos::TIPO_BOOL:
					//TODO Implementar IndiceBooleanRomanoManager
					//return new IndiceBooleanRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura);
				case TipoDatos::TIPO_FLOAT:
					return new IndiceRealRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura);
				case TipoDatos::TIPO_FECHA:
					return new IndiceFechaRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura);
				case TipoDatos::TIPO_STRING:
					return new IndiceVariableRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura);
				case TipoDatos::TIPO_COMPUESTO:
					//TODO Implementar IndiceCompuestoRomanoManager
					//return new IndiceCompuestoRomanoManager(tamNodo, nombreArchivo, tamBloqueLista, tipoEstructura, listaTipos);
					return NULL;
			}
			
		case TipoIndices::HASH:
			
			return new IndiceHashManager(tamBucket, nombreArchivo);
			
		default: return NULL;
	
	}
	
}

