#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>
#include "ResultadosFisica.h"
#include "ResultadosIndices.h"
#include "CodigosPipe.h"

class Error {

    public:
    	
        typedef int tError;
        
    	static const int _NO_ERROR_ = 0;
        //Se define el tipo de dato del codigo de error
        
        //Constructor por default
        Error() { error = _NO_ERROR_; }
        //Constructor de copia
        Error (const Error &e) { error = e.getValue(); }
        //Destructor
        virtual ~Error() {}
        
        //Devuelve el codigo de error
        tError getValue() const { return error; }
        //Operador utilzado para verificar la existencia de error
        operator bool () const { return (error != _NO_ERROR_); }
        //Operador utilizado para asignar un codigo de error
        void operator << (const tError e) { error = e; }
        //Operador de asignacien estandar
        Error& operator = (const Error &e) {
            error = e.getValue();
            return *this;
        }
        
        //Metodo que convierte el codigo de error en un string
        std::string toString() const {
            std::string s(1, (char)(48 + error));
            return s;
        }
        //Metodo que obtiene el codigo de error desde un string
        void fromString(const std::string& s) { error = ((tError)s.at(0) - 48);}

    private:
        tError error;

};

#endif
