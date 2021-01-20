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

	int errores;

	errores = 0;
	for(int i=0; i<ii_srp.pers_id.size(); i++){
		cout << i << "\t" << ii_srp.pers_id[i] << " en el mapa es: " << ii_srp.mapa_personas[ii_srp.pers_id[i]] << endl;
		if(i != ii_srp.mapa_personas[ii_srp.pers_id[i]]){
			errores++;
		}
	}
	cout << errores << " errores en pers_id" << endl;

	errores = 0;
	for(int i=0; i<ii_srp.vehi_id.size(); i++){
		cout << i << "\t" << ii_srp.vehi_id[i] << " en el mapa es: " << ii_srp.mapa_vehiculos[ii_srp.vehi_id[i]] << endl;
		if(i != ii_srp.mapa_vehiculos[ii_srp.vehi_id[i]]){
			errores++;
		}
	}
	cout << errores << " errores en pers_id" << endl;

	errores = 0;
	for(int i=0; i<ii_srp.ubica_id.size(); i++){
		cout << i << "\t" << ii_srp.ubica_id[i] << " en el mapa es: " << ii_srp.mapa_ubicaciones[ii_srp.ubica_id[i]] << endl;
		if(i != ii_srp.mapa_ubicaciones[ii_srp.ubica_id[i]]){
			errores++;
		}
	}
	cout << errores << " errores en pers_id" << endl;

	errores = 0;
	for(int i=0; i<ii_srp.bloque_id.size(); i++){
		cout << i << "\t" << ii_srp.bloque_id[i] << " en el mapa es: " << ii_srp.mapa_bloquesh[ii_srp.bloque_id[i]] << endl;
		if(i != ii_srp.mapa_bloquesh[ii_srp.bloque_id[i]]){
			errores++;
		}
	}
	cout << errores << " errores en pers_id" << endl;

	errores = 0;
	for(int i=0; i<ii_srp.visita_id.size(); i++){
		cout << i << "\t" << ii_srp.visita_id[i] << " en el mapa es: " << ii_srp.mapa_visitas[ii_srp.visita_id[i]] << endl;
		if(i != ii_srp.mapa_visitas[ii_srp.visita_id[i]]){
			errores++;
		}
	}
	cout << errores << " errores en pers_id" << endl;


	return 0;
}