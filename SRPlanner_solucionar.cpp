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

	// Se crea el solucionador que genera:
	//	- Un InputInstance con los datos de entrada
	//	- Un InstanceSolution con la solución inicial
	Solver solucionador(fnameIn);
	// Ejecutar búsqueda local para sacar salida a fichero
	solucionador.SolucionPorBusquedaLocal(argv[2]);
	
	return 0;
}