#include "SRP_Utils.hpp"

time_t obtenerTime(string ts){
	time_t t;
	//return t;
	//time (&t);
	char * cts = new char [ts.length()+1];
	strcpy (cts, ts.c_str());
	struct tm tm;
	strptime(cts, "%Y-%m-%dT%H:%M:%S%Z", &tm);
	free(cts);
	t = mktime(&tm);  // t is now your desired time_t
	return t;
}


string stringTime(time_t t){
	string aux = ctime(&t);
	return aux.substr(0,aux.length()-1);
}


time_t sumaMinutos(time_t t, int m){
	struct tm tm = *localtime(&t);
	tm.tm_min += m;
	time_t nt = mktime(&tm);
	return nt;
}