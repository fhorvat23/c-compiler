#include "serialization.h"
#include <fstream>
#include <cstring>
#include <iostream>

using namespace std;

void serializeBinary(const vector<string>& stanja,
                     const map<string, set<int>>& prihvatljivaStanja,
                     const map<string, map<int, tuple<string, string, string, int>>>& akcijeZaIzraz,
                     const map<string, map<int, set<int>>>& epsilonPrijelazi,
                     const map<string, map<int, pair<int, char>>>& prijelazi,
                     const string& filename) {
    ofstream ofs(filename, ios::binary);
    if (!ofs) {
        cerr << "Error opening file for writing." << endl;
        return;
    }

    // Serialize stanja
    size_t stanjaSize = stanja.size();
    ofs.write(reinterpret_cast<const char*>(&stanjaSize), sizeof(stanjaSize));
    for (const auto& stanje : stanja) {
        size_t length = stanje.size();
        ofs.write(reinterpret_cast<const char*>(&length), sizeof(length));
        ofs.write(stanje.c_str(), length);
    }

    // Serialize prihvatljivaStanja
    size_t prihvatljivaSize = prihvatljivaStanja.size();
    ofs.write(reinterpret_cast<const char*>(&prihvatljivaSize), sizeof(prihvatljivaSize));
    for (const auto& [key, intSet] : prihvatljivaStanja) {
        size_t keyLength = key.size();
        ofs.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
        ofs.write(key.c_str(), keyLength); // Write the string key

        size_t setSize = intSet.size();
        ofs.write(reinterpret_cast<const char*>(&setSize), sizeof(setSize));
        for (int value : intSet) {
            ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
        }
    }

    // Serialize akcijeZaIzraz
    size_t akcijeSize = akcijeZaIzraz.size();
    ofs.write(reinterpret_cast<const char*>(&akcijeSize), sizeof(akcijeSize));
    for (const auto& [key, innerMap] : akcijeZaIzraz) {
        size_t keyLength = key.size();
        ofs.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
        ofs.write(key.c_str(), keyLength);

        size_t innerSize = innerMap.size();
        ofs.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));
        for (const auto& [index, tupleValue] : innerMap) {
            ofs.write(reinterpret_cast<const char*>(&index), sizeof(index));
            const auto& [str1, str2, str3, intValue] = tupleValue;

            size_t length1 = str1.size();
            size_t length2 = str2.size();
            size_t length3 = str3.size();
            ofs.write(reinterpret_cast<const char*>(&length1), sizeof(length1));
            ofs.write(str1.c_str(), length1);
            ofs.write(reinterpret_cast<const char*>(&length2), sizeof(length2));
            ofs.write(str2.c_str(), length2);
            ofs.write(reinterpret_cast<const char*>(&length3), sizeof(length3));
            ofs.write(str3.c_str(), length3);
            ofs.write(reinterpret_cast<const char*>(&intValue), sizeof(intValue));
        }
    }

    // Serialize epsilonPrijelazi
    size_t epsilonSize = epsilonPrijelazi.size();
    ofs.write(reinterpret_cast<const char*>(&epsilonSize), sizeof(epsilonSize));
    for (const auto& [key, innerMap] : epsilonPrijelazi) {
        size_t keyLength = key.size();
        ofs.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
        ofs.write(key.c_str(), keyLength);

        size_t innerSize = innerMap.size();
        ofs.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));
        for (const auto& [index, values] : innerMap) {
            ofs.write(reinterpret_cast<const char*>(&index), sizeof(index));
            size_t setSize = values.size();
            ofs.write(reinterpret_cast<const char*>(&setSize), sizeof(setSize));
            for (const auto& val : values) {
                ofs.write(reinterpret_cast<const char*>(&val), sizeof(val));
            }
        }
    }

    // Serialize prijelazi
    size_t prijelaziSize = prijelazi.size();
    ofs.write(reinterpret_cast<const char*>(&prijelaziSize), sizeof(prijelaziSize));
    for (const auto& [key, innerMap] : prijelazi) {
        size_t keyLength = key.size();
        ofs.write(reinterpret_cast<const char*>(&keyLength), sizeof(keyLength));
        ofs.write(key.c_str(), keyLength);

        size_t innerSize = innerMap.size();
        ofs.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));
        for (const auto& [index, pairValue] : innerMap) {
            ofs.write(reinterpret_cast<const char*>(&index), sizeof(index));
            ofs.write(reinterpret_cast<const char*>(&pairValue.first), sizeof(pairValue.first));
            ofs.write(reinterpret_cast<const char*>(&pairValue.second), sizeof(pairValue.second));
        }
    }

    ofs.close();
}
