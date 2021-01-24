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
		delete bitmap;
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


// Veradero si THIS es subconjunto de subConj
bool MiniBitmap::esSubconjunto(MiniBitmap* supConj){
	// Si es nulo el parámetro o si tienen diferente capacidad
	if(supConj == NULL || cap != supConj->cap){
		return false;
	}
	int mascara, opA, opSC;
	for(int i=0; i<ints; i++){
		// Si el supConj no tiene elementos en 1 en ese entero, no es subconjunto
		if(bitmap[i] != 0 && supConj == 0){
			return false;
		}else{
			mascara = 1;
			for(int j=0; j<bits_int; j++){
				// Esta operación debería dar 0 o un número
				opA = bitmap[i] & mascara;
				opSC = supConj->bitmap[i] & mascara;
				if(opA > opSC){
					return false;
				}
				mascara = mascara << 1;
			}
		}
	}
	return true;
}


// Cuenta la cantidad de 1s (unos) que hay entre dos posiciones (incluyéndolas)
int MiniBitmap::count(int pInicial, int pFinal){
	if(pInicial > pFinal){
		int aux = pFinal;
		pFinal = pInicial;
		pInicial = aux;
	}
	if(pInicial < 0){
		pInicial = 0;
	}
	if(pFinal > cap){
		pFinal = cap - 1;
	}
	if(pInicial == 0 && pFinal == cap - 1){
		return cant;
	}
	int posInt = pInicial / bits_int;
	int posBit = pInicial % bits_int;
	int mascara = 1 << posBit;
	int count = 0;
	for(int pos = pInicial ; pos <= pFinal; pos++){
		if((bitmap[posInt] & mascara) != 0){
			count++;
		}
		posBit++;
		if(posBit == bits_int){
			mascara = 1;
			posBit = 0; 
			posInt++;
		}else{
			mascara = mascara << 1;
		}
	}
	return count;
}