#include "SRPlanner.hpp"

int main(int argc, char const *argv[]){
	if(argc < 2){
		cout << "Error! Falta argunmento con nombre del archivo de entrada." << endl;
		cout << "Ejemplo: " << endl;
		cout << argv[0] << " datos.txt" << endl;
		return -1;
	}
	cout << "Funcionando..." << endl;
	string fname(argv[1]);
	InstanceInput srp(fname);
	return 0;
}