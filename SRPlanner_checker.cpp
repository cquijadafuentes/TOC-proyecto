#include <iostream>
#include "SRPlanner.hpp"

using namespace std;

int main(int argc, char const *argv[]){
	if(argc < 3){
		cout << "Error! Faltan argumentos." << endl;
		cout << "Ejemplo: \n\t" << argv[0] << " instance_input.txt instance_output.txt" << endl;
		return -1;
	}

	string fnameIn(argv[1]);
	string fnameOut(argv[2]);
	InstanceInput ii_srp(fnameIn);
	InstanceOutput io_srp(fnameOut,ii_srp.n, ii_srp.m, ii_srp.k, ii_srp.l);

	cout << "Viajes por Vehículos ---> ";
	int erroresViajesVehiculo = io_srp.validaViajesPorVehiculos();
	if(erroresViajesVehiculo == 0){
		cout << "OK" << endl;
	}else{		
		cout << "Error! " << erroresViajesVehiculo << " encontrados." << endl;
	}

	return 0;
}