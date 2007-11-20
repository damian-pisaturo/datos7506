#ifndef LISTAPRIMARIAMANAGER_H_
#define LISTAPRIMARIAMANAGER_H_

class ListaPrimariaManager {
	
	private:
		unsigned short tamLista;
		
	public:
		ListaPrimariaManager(unsigned short tamLista) : tamLista(tamLista) {}
		
		virtual ~ListaPrimariaManager() {}
		
		void crearLista(char* &lista) const;
		
		void crearLista(char* &lista, const Clave* clave, const ListaTipos* listaTipos) const;
		
		bool insertarClave(char* lista, const Clave* clave, const ListaTipos* listaTipos) const;
		
		bool eliminarClave(char* lista, const Clave* clave, const ListaTipos* listaTipos) const;
		
		unsigned short getTamanioLista() const { return this->tamLista; }
		
		void setTamanioLista(unsigned short tamLista) { this->tamLista = tamLista; }
		
};

#endif /*LISTAPRIMARIAMANAGER_H_*/
