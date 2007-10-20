#ifndef CLAVE_H_
#define CLAVE_H_

class Clave {
	
	private:
		unsigned char tamanioClave;
		void* clave;
		
	public:
		Clave();
		virtual ~Clave();
		
		unsigned char getTamanioClave();
		void* getClave();
		void setTamanioClave(unsigned char tamanioClave);
		void setClave(void* clave);
};

#endif /*CLAVE_H_*/
