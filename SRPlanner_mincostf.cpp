#include "SRPlanner.hpp"

int main(int argc, char const *argv[]){
	if(argc < 2){
		cout << "Error! Falta argunmento con nombre del archivo de entrada." << endl;
		cout << "Ejemplo: " << endl;
		cout << argv[0] << " input.txt" << endl;
		return -1;
	}
	cout << "Funcionando..." << endl;
	string fnameIn(argv[1]);
	InstanceInput ii_srp(fnameIn);
	vector<pair<string,string>> vPV = ii_srp.MinCostFlow();

	cout << "Visitas a ralizar idPersona -> idVisita: " << endl;
	for(int i=0; i<vPV.size(); i++){
		cout << vPV[i].first << " -> " << vPV[i].second << endl;
	}
	return 0;
}