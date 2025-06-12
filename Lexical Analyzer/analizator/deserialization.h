#ifndef DESERIALIZATION_H
#define DESERIALIZATION_H

#include <set>
#include <map>
#include <tuple>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void deserializeBinary(const string& filename,
                       vector<string>& stanja,
                       map<string, set<int>>& prihvatljivaStanja,
                       map<string, map<int, tuple<string, string, string, int>>>& akcijeZaIzraz,
                       map<string, map<int, set<int>>>& epsilonPrijelazi,
                       map<string, map<int, pair<int, char>>>& prijelazi);

#endif