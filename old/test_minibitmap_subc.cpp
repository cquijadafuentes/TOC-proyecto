#include <iostream>
#include "../MiniBitmap.hpp"

using namespace std;

int main(int argc, char const *argv[]){
	cout << "Hola mundo " << endl;
	int bitsrequeridos = 100;
	MiniBitmap* a = new MiniBitmap(bitsrequeridos);
	MiniBitmap* x = new MiniBitmap(bitsrequeridos);
	MiniBitmap* c = new MiniBitmap(bitsrequeridos);
	for(int i=0; i < bitsrequeridos; i++){
		cout << x->access(i);
	}
	cout << endl;

	cout << "Insertando: " << endl;
	for(int j=0; j<5; j++){
	int aux;
		cin >> aux;
		a->setBit(aux);
		x->setBit(aux);
		c->setBit(aux);
		x->printBitmap();
		cout << endl;
	}
	cout << "Eliminando del sub_conjunto_valido: " << endl;
	for(int j=0; j<3; j++){
	int aux;
		cin >> aux;
		a->unsetBit(aux);
		a->printBitmap();
		cout << endl;
	}
	cout << "Insertando en el sub_conjunto_erróneo: " << endl;
	for(int j=0; j<3; j++){
	int aux;
		cin >> aux;
		c->setBit(aux);
		c->printBitmap();
		cout << endl;
	}

	cout << "a: " ;
	a->printInfo();
	cout << "x: " ;
	x->printInfo();
	cout << "c: " ;
	c->printInfo();
	if(a->esSubconjunto(x)){
		cout << "A es subconjunto de X" << endl;
	}else{

		cout << "A NO es subconjunto de X" << endl;
	}
	if(c->esSubconjunto(x)){
		cout << "C es subconjunto de X" << endl;
	}else{

		cout << "C NO es subconjunto de X" << endl;
	}
	delete(x);
	return 0;
}