#include "deserialization.h"
#include <fstream>
#include <cstring>
#include <iostream>

using namespace std;

void deserializeBinary(const string& filename,
                       vector<string>& stanja,
                       map<string, set<int>>& prihvatljivaStanja,
                       map<string, map<int, tuple<string, string, string, int>>>& akcijeZaIzraz,
                       map<string, map<int, set<int>>>& epsilonPrijelazi,
                       map<string, map<int, pair<int, char>>>& prijelazi) {
    ifstream ifs(filename, ios::binary);
    if (!ifs) {
        cerr << "Error opening file for reading." << endl;
        return;
    }

    // Deserialize stanja
    size_t stanjaSize;
    ifs.read(reinterpret_cast<char*>(&stanjaSize), sizeof(stanjaSize));
    stanja.resize(stanjaSize);
    for (size_t i = 0; i < stanjaSize; ++i) {
        size_t length;
        ifs.read(reinterpret_cast<char*>(&length), sizeof(length));
        stanja[i].resize(length);
        ifs.read(&stanja[i][0], length);
    }

    // Deserialize prihvatljivaStanja
    size_t prihvatljivaSize;
    ifs.read(reinterpret_cast<char*>(&prihvatljivaSize), sizeof(prihvatljivaSize));
    for (size_t i = 0; i < prihvatljivaSize; ++i) {
        size_t keyLength;
        ifs.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));

        string key(keyLength, '\0');
        ifs.read(&key[0], keyLength); // Read the string key

        size_t setSize;
        ifs.read(reinterpret_cast<char*>(&setSize), sizeof(setSize));
        set<int> intSet;
        for (size_t j = 0; j < setSize; ++j) {
            int value;
            ifs.read(reinterpret_cast<char*>(&value), sizeof(value));
            intSet.insert(value);
        }
        prihvatljivaStanja[key] = intSet;
    }

    // Deserialize akcijeZaIzraz
    size_t akcijeSize;
    ifs.read(reinterpret_cast<char*>(&akcijeSize), sizeof(akcijeSize));
    for (size_t i = 0; i < akcijeSize; ++i) {
        size_t keyLength;
        string key;
        ifs.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
        key.resize(keyLength);
        ifs.read(&key[0], keyLength);

        size_t innerSize;
        ifs.read(reinterpret_cast<char*>(&innerSize), sizeof(innerSize));
        for (size_t j = 0; j < innerSize; ++j) {
            int index;
            ifs.read(reinterpret_cast<char*>(&index), sizeof(index));

            string str1, str2, str3;
            int intValue;

            size_t length1, length2, length3;
            ifs.read(reinterpret_cast<char*>(&length1), sizeof(length1));
            str1.resize(length1);
            ifs.read(&str1[0], length1);

            ifs.read(reinterpret_cast<char*>(&length2), sizeof(length2));
            str2.resize(length2);
            ifs.read(&str2[0], length2);

            ifs.read(reinterpret_cast<char*>(&length3), sizeof(length3));
            str3.resize(length3);
            ifs.read(&str3[0], length3);

            ifs.read(reinterpret_cast<char*>(&intValue), sizeof(intValue));

            akcijeZaIzraz[key][index] = make_tuple(str1, str2, str3, intValue);
        }
    }

    // Deserialize epsilonPrijelazi
    size_t epsilonSize;
    ifs.read(reinterpret_cast<char*>(&epsilonSize), sizeof(epsilonSize));
    for (size_t i = 0; i < epsilonSize; ++i) {
        size_t keyLength;
        string key;
        ifs.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
        key.resize(keyLength);
        ifs.read(&key[0], keyLength);

        size_t innerSize;
        ifs.read(reinterpret_cast<char*>(&innerSize), sizeof(innerSize));
        for (size_t j = 0; j < innerSize; ++j) {
            int index;
            ifs.read(reinterpret_cast<char*>(&index), sizeof(index));

            size_t setSize;
            ifs.read(reinterpret_cast<char*>(&setSize), sizeof(setSize));
            set<int> values;
            for (size_t k = 0; k < setSize; ++k) {
                int value;
                ifs.read(reinterpret_cast<char*>(&value), sizeof(value));
                values.insert(value);
            }
            epsilonPrijelazi[key][index] = values;
        }
    }

    // Deserialize prijelazi
    size_t prijelaziSize;
    ifs.read(reinterpret_cast<char*>(&prijelaziSize), sizeof(prijelaziSize));
    for (size_t i = 0; i < prijelaziSize; ++i) {
        size_t keyLength;
        string key;
        ifs.read(reinterpret_cast<char*>(&keyLength), sizeof(keyLength));
        key.resize(keyLength);
        ifs.read(&key[0], keyLength);

        size_t innerSize;
        ifs.read(reinterpret_cast<char*>(&innerSize), sizeof(innerSize));
        for (size_t j = 0; j < innerSize; ++j) {
            int index;
            pair<int, char> pairValue;
            ifs.read(reinterpret_cast<char*>(&index), sizeof(index));
            ifs.read(reinterpret_cast<char*>(&pairValue.first), sizeof(pairValue.first));
            ifs.read(reinterpret_cast<char*>(&pairValue.second), sizeof(pairValue.second));
            prijelazi[key][index] = pairValue;
        }
    }

    ifs.close();
}