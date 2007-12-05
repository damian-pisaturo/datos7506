#include "BTree.h"

BTree::BTree(IndiceManager& indiceManager, unsigned short tamanioNodo) : indiceManager(indiceManager) {
	this->tamanioNodo = tamanioNodo;
}

BTree::~BTree() {
	
}
