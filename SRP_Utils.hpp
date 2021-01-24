#ifndef SRP_UTILS
#define SRP_UTILS

#include <cstring>
#include <string>

using namespace std;

time_t obtenerTime(string ts);

string stringTime(time_t t);

time_t sumaMinutos(time_t t, int m);

#endif