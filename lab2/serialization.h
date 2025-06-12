#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <vector>
#include <string>
#include <map>

void serializeBinary(const std::vector<std::vector<std::string>>& produkcije_po_redu,
                     const std::vector<std::vector<std::string>>& akcija_novostanje,
                     const std::vector<std::string>& sinkronizacijskiZnakovi,
                     const std::map<std::string, int>& znakoviInt,
                     const std::string& filename);

#endif