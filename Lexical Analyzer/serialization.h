#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <set>
#include <map>
#include <tuple>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void serializeBinary(const vector<string>& stanja,
                     const map<string, set<int>>& prihvatljivaStanja,
                     const map<string, map<int, tuple<string, string, string, int>>>& akcijeZaIzraz,
                     const map<string, map<int, set<int>>>& epsilonPrijelazi,
                     const map<string, map<int, pair<int, char>>>& prijelazi,
                     const string& filename);

#endif