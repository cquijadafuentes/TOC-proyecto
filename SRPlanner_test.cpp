#include "SRPlanner.hpp"

int main(int argc, char const *argv[]){
	if(argc < 3){
		cout << "Error! Falta argunmento con nombre del archivo de entrada." << endl;
		cout << "Ejemplo: " << endl;
		cout << argv[0] << " input.txt output.txt" << endl;
		return -1;
	}
	cout << "Funcionando..." << endl;
	string fnameIn(argv[1]);
	string fnameOut(argv[2]);
	InstanceInput ii_srp(fnameIn);
	InstanceOutput io_srp(fnameOut,ii_srp.n, ii_srp.m, ii_srp.k, ii_srp.l);
	return 0;
}