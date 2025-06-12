#ifndef DESERIALIZATION_H
#define DESERIALIZATION_H

#include <vector>
#include <string>
#include <map>

void deserializeBinary(const std::string& filename,
                       std::vector<std::vector<std::string>>& produkcije_po_redu,
                       std::vector<std::vector<std::string>>& akcija_novostanje,
                       std::vector<std::string>& sinkronizacijskiZnakovi,
                       std::map<std::string, int>& znakoviInt);

#endif
