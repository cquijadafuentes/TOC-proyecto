#include MINIBITMAP
#define MINIBITMAP

#include <iostream>

using namespace std;

const int bits_int = 32;

class MiniBitmap{
	public:
		int cant;			// Cantidad de unos en el bitmap
		int cap;			// Capacidad del bitmap
		MiniBitmap(int n);
		~MiniBitmap();
		void setBit(int x);
		void unsetBit(int x);
		int access(int x);
		void printInfo();
		void printBitmap();
		bool esSubconjunto(MiniBitmap* supConj);
		int count(int pInicial, int pFinal);
	private:
		int* bitmap;		// Arreglo de enteros para almacenar los bits
		int ints;			// Cantidad de enteros para el bitmap
};

#endif