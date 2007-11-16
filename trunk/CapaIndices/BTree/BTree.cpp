#include "BTree.h"

BTree::BTree(IndiceManager& indiceManager, unsigned short tamanioNodo) : indiceManager( (IndiceArbolManager&) indiceManager) {
	this->tamanioNodo = tamanioNodo;
}

BTree::~BTree() {
	
}
