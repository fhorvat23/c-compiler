#include "serialization.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

void serializeBinary(const std::vector<std::vector<std::string>>& produkcije_po_redu,
                     const std::vector<std::vector<std::string>>& akcija_novostanje,
                     const std::vector<std::string>& sinkronizacijskiZnakovi,
                     const std::map<std::string, int>& znakoviInt,
                     const std::string& filename) {
   ofstream ofs(filename, ios::binary);
   if (!ofs) {
      cerr << "Error opening file for writing." << endl;
      return;
   }

   // Serialize produkcije_po_redu
   size_t outerSize = produkcije_po_redu.size();
   ofs.write(reinterpret_cast<const char*>(&outerSize), sizeof(outerSize));
   for (const auto& innerVec : produkcije_po_redu) {
      size_t innerSize = innerVec.size();
      ofs.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));
      for (const auto& str : innerVec) {
            size_t strSize = str.size();
            ofs.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
            ofs.write(str.c_str(), strSize);
      }
   }

   // Serialize akcija_novostanje
   outerSize = akcija_novostanje.size();
   ofs.write(reinterpret_cast<const char*>(&outerSize), sizeof(outerSize));
   for (const auto& innerVec : akcija_novostanje) {
      size_t innerSize = innerVec.size();
      ofs.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));
      for (const auto& str : innerVec) {
            size_t strSize = str.size();
            ofs.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
            ofs.write(str.c_str(), strSize);
      }
   }
   
   // Serialize sinkronizacijskiZnakovi
   size_t vecSize = sinkronizacijskiZnakovi.size();
   ofs.write(reinterpret_cast<const char*>(&vecSize), sizeof(vecSize));
   for (const auto& str : sinkronizacijskiZnakovi) {
      size_t strSize = str.size();
      ofs.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
      ofs.write(str.c_str(), strSize);
   }

   // Serialize znakoviInt
   size_t mapSize = znakoviInt.size();
   ofs.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));
   for (const auto& [key, value] : znakoviInt) {
      size_t keySize = key.size();
      ofs.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize));
      ofs.write(key.c_str(), keySize);
      ofs.write(reinterpret_cast<const char*>(&value), sizeof(value));
   }

   ofs.close();
}
