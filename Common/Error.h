#ifndef __ERROR_H__
#define __ERROR_H__

#include <string>

class Error {

    public:
        //Se define el tipo de dato del codigo de error
        typedef unsigned short tError;
        //Se definen los distintos codigos de error
        static const tError ERR_COMANDO_INVALIDO    = 0;
        static const tError ERR_ENTERO_NO_VALIDO    = 1;
        static const tError ERR_DIVISION_POR_CERO   = 2;
        static const tError ERR_FORMATO_ARCHIVO     = 3;
        static const tError _NO_ERROR_              = 4;
        static const tError ERR_ABRIR_ARCHIVO       = 5;
        static const tError ERR_SINTAXIS            = 6;
        static const tError ERR_SOCKET              = 7;
        static const tError ERR_THREAD              = 8;
        static const tError ERR_IP                  = 9;
        static const tError ERR_PUERTO              = 10;
        static const tError ERR_CANT_CLIENTES       = 11;
        static const tError ERR_ENTERO_NO_PRIMO     = 12;
        static const tError ERR_OVERFLOW            = 13;
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
        //Metodo que convierte el cedigo de error en un string
        std::string toString() const {
            std::string s(1, (char)(48+error));
            return s;
        }
        //Metodo que obtiene el codigo de error desde un string
        void fromString(const std::string& s) { error = ((tError)s.at(0) - 48);}

    private:
        tError error;

};

#endif
