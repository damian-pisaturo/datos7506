#ifndef ELEMENTO_H_
#define ELEMENTO_H_

class Elemento {
	
	private:
		unsigned int offsetArchivo; //Si es primario apunta al dato, si es secundario a un archivo con una lista de claves.
		unsigned int hijoDerecho;
	
	public:
		Elemento();
		virtual ~Elemento();
		
		//Getters y Setters
		unsigned int getOffsetArchivo();
		unsigned int getHijoDerecho();
		void setOffsetArchivo(unsigned int offsetArchivo);
		void setHijoDerecho(unsigned int hijoDerecho);
		
};

#endif /*ELEMENTO_H_*/
