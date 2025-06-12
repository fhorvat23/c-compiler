#include "deserialization.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

void deserializeBinary(const std::string& filename,
                       std::vector<std::vector<std::string>>& produkcije_po_redu,
                       std::vector<std::vector<std::string>>& akcija_novostanje,
                       std::vector<std::string>& sinkronizacijskiZnakovi,
                       std::map<std::string, int>& znakoviInt) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        cerr << "Error opening file for reading." << endl;
        return;
    }

    // Deserialize produkcije_po_redu
    size_t outerSize;
    ifs.read(reinterpret_cast<char*>(&outerSize), sizeof(outerSize));
    produkcije_po_redu.resize(outerSize);
    for (auto& innerVec : produkcije_po_redu) {
        size_t innerSize;
        ifs.read(reinterpret_cast<char*>(&innerSize), sizeof(innerSize));
        innerVec.resize(innerSize);
        for (auto& str : innerVec) {
            size_t strSize;
            ifs.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
            str.resize(strSize);
            ifs.read(&str[0], strSize);
        }
    }

    // Deserialize akcija_novostanje
    ifs.read(reinterpret_cast<char*>(&outerSize), sizeof(outerSize));
    akcija_novostanje.resize(outerSize);
    for (auto& innerVec : akcija_novostanje) {
        size_t innerSize;
        ifs.read(reinterpret_cast<char*>(&innerSize), sizeof(innerSize));
        innerVec.resize(innerSize);
        for (auto& str : innerVec) {
            size_t strSize;
            ifs.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
            str.resize(strSize);
            ifs.read(&str[0], strSize);
        }
    }

    size_t vecSize;
    ifs.read(reinterpret_cast<char*>(&vecSize), sizeof(vecSize));
    sinkronizacijskiZnakovi.resize(vecSize);
    for (auto& str : sinkronizacijskiZnakovi) {
        size_t strSize;
        ifs.read(reinterpret_cast<char*>(&strSize), sizeof(strSize));
        str.resize(strSize);
        ifs.read(&str[0], strSize);
    }

    // Deserialize znakoviInt
    size_t mapSize;
    ifs.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
    for (size_t i = 0; i < mapSize; ++i) {
        size_t keySize;
        ifs.read(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        std::string key(keySize, '\0');
        ifs.read(&key[0], keySize);
        int value;
        ifs.read(reinterpret_cast<char*>(&value), sizeof(value));
        znakoviInt[key] = value;
    }

    ifs.close();
}
