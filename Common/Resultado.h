#ifndef RESULTADO_H_
#define RESULTADO_H_

#include <string>
#include "ResultadosFisica.h"
#include "ResultadosIndices.h"
#include "CodigosPipe.h"

class Resultado {

    public:
    	
        //Se define el tipo de dato del codigo de resultado
        typedef int tResultado;
        
    	static const tResultado _NO_ERROR_ = 0;
        
        //Constructor por default
        Resultado() { resultado = _NO_ERROR_; }
        
        //Constructor de copia
        Resultado (const Resultado &e) { resultado = e.getValue(); }
        
        //Destructor
        virtual ~Resultado() {}
        
        //Devuelve el codigo de resultado
        tResultado getValue() const { return resultado; }
        
        //Operador utilizado para asignar un codigo de resultado
        void operator << (const tResultado &e) { resultado = e; }
        
        //Operador de asignación estandar
        Resultado& operator = (const Resultado &e) {
            resultado = e.getValue();
            return *this;
        }

    private:
        tResultado resultado;

};

#endif
