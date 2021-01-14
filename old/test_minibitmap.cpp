#include <iostream>
#include "../MiniBitmap.hpp"

using namespace std;

int main(int argc, char const *argv[]){
	cout << "Hola mundo " << endl;
	int bitsrequeridos = 100;
	MiniBitmap* x = new MiniBitmap(bitsrequeridos);
	for(int i=0; i < bitsrequeridos; i++){
		cout << x->access(i);
	}
	cout << endl;

	cout << "Insertando: " << endl;
	for(int j=0; j<5; j++){
	int aux;
		cin >> aux;
		x->setBit(aux);
		x->printBitmap();
		cout << endl;
	}
	cout << "Eliminando: " << endl;
	for(int j=0; j<3; j++){
	int aux;
		cin >> aux;
		x->unsetBit(aux);
		x->printBitmap();
		cout << endl;
	}
	x->printInfo();
	delete(x);
	return 0;
}