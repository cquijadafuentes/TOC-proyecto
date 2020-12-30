#include "MiniBitmap.hpp"


MiniBitmap::MiniBitmap(int n){
	if(n <= 0){
		delete(this);
	}
	cap = n;						// Cantidad de bits requeridos
	ints = n/bits_int;				// Se calcula
	if(ints * bits_int < n){		// Se actualiza 
		ints++;						// (por el truncado podría faltar 1 entero)
	}
	bitmap = new int[ints]();		// Se inicializa todo en 0
	cant = 0;						// No hay unos en el bitmap
}


MiniBitmap::~MiniBitmap(){
	if(cap > 0 && bitmap != NULL){
		delete(bitmap);
	}
}


void MiniBitmap::setBit(int x){
	if(x < 0 || x >= cap || access(x) == 1){
		return;
	}
	int bitpos = x % bits_int;			// Se calcula la posición del bit
	int mascara = 1 << bitpos;	// máscara para añadir 1 en el bitmap
	int pos = x / bits_int;			// Posición del entero donde se encuentra el bit
	bitmap[pos] = bitmap[pos] | mascara;	// Se cambia a 1 el bit necesario.
	cant++;						// Se incrementa el contador de unos del bitmap
}


void MiniBitmap::unsetBit(int x){
	if(x < 0 || x >= cap || access(x) == 0){
		return;
	}
	int bitpos = x % bits_int;			// Se calcula la posición del bit
	int mascara = ~(1 << bitpos);	// máscara para quitar 1 en el bitmap
	int pos = x / bits_int;			// Posición del entero donde se encuentra el bit
	bitmap[pos] = bitmap[pos] & mascara;	// Se cambia a 1 el bit necesario.
	cant--;						// Se incrementa el contador de unos del bitmap
}

int MiniBitmap::access(int x){
	if(x < 0 || x >= cap){
		return 0;
	}
	int bitpos = x % bits_int;			// Se calcula la posición del bit
	int mascara = 1 << bitpos;	// máscara para quitar 1 en el bitmap
	int pos = x / bits_int;			// Posición del entero donde se encuentra el bit
	if(bitmap[pos] & mascara){
		return 1;
	}else{
		return 0;
	}
	
}


void MiniBitmap::printInfo(){
	cout << "Capacidad: " << cap << endl;
	cout << "Cantidad de unos: " << cant << endl;
	cout << "Bitmap:"<< endl;
	printBitmap();
	cout << endl;
}


void MiniBitmap::printBitmap(){
	int impresos = 0;
	int aux;
	for(int i=0; i<ints; i++){
		aux = 1;
		for(int j=0; j<bits_int && impresos < cap; j++){
			if(bitmap[i] & aux){
				cout << 1;
			}else{
				cout << 0;
			}
			aux = aux << 1;
			impresos++;
		}
	}
}