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
	int aux;
	for(int j=0; j<5; j++){
		cin >> aux;
		x->setBit(aux);
		x->printBitmap();
		cout << endl;
	}
	cout << "Eliminando: " << endl;
	for(int j=0; j<3; j++){
		cin >> aux;
		x->unsetBit(aux);
		x->printBitmap();
		cout << endl;
	}
	x->printInfo();
	cout << "Count: posición inicial: ";
	cin >> aux;
	int aux2;
	cout << "Count: posición final: ";
	cin >> aux2;
	int count = x->count(aux, aux2);
	cout << "Count(" << aux << "," << aux2 << "): " << count << endl;
	delete(x);
	return 0;
}