#ifndef DEFINICIONESMANAGER_H_
#define DEFINICIONESMANAGER_H_

//#include "../RegisterInfo/RegisterInfo.cpp"
#include <map>

class DefinicionesManager {
	
	public:
		typedef ListaNodos ListaTiposAtributos;
		
		typedef std::map<string, ListaTiposAtributos*> MapaDefiniciones;
		
		DefinicionesManager();
		
		virtual ~DefinicionesManager();
		
	private:
		//typedef struct _
		MapaDefiniciones mapaTipos;
		 
		void cargarDefiniciones();
		
};

#endif /*DEFINICIONESMANAGER_H_*/
