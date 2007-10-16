#ifndef HASH_H_
#define HASH_H_

class Hash
{
public:
	Hash();
	virtual ~Hash();
	int aplicarHash(char* clave);
	
private:
	int hash(char* clave);

};

#endif /*HASH_H_*/
