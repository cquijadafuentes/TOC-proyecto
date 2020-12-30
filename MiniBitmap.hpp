#include <iostream>

using namespace std;

const int bits_int = 32;

class MiniBitmap{
	public:
		MiniBitmap(int n);
		~MiniBitmap();
		void setBit(int x);
		void unsetBit(int x);
		int access(int x);
		void printInfo();
		void printBitmap();
	private:
		int* bitmap;		// Arreglo de enteros para almacenar los bits
		int cant;			// Cantidad de unos en el bitmap
		int cap;			// Capacidad del bitmap
		int ints;			// Cantidad de enteros para el bitmap
};