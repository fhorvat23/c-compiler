#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <fstream>
#include "deserialization.h"

using namespace std;

map<string, set<int>> prihvatljivaStanja;
map<string, map<int, tuple<string, string, string, int>>> akcijeZaIzraz;
map<string, map<int, set<int>>> epsilonPrijelazi;
map<string, map<int, pair<int,char>>> prijelazi;
vector<string> stanja;
string trenutnoStanje;
int redak = 1;

set<int> epsilon_okruzenje(int stanje, string automat) {
   set<int> sva_stanja = {stanje};
   set<int> nova_stanja = epsilonPrijelazi[automat][stanje];
   while (!nova_stanja.empty()) {
      auto it = nova_stanja.begin();
      if (sva_stanja.find(*it) == sva_stanja.end()) {
         sva_stanja.insert(*it);
         set<int> pomocni = epsilonPrijelazi[automat][*it];
         for (auto it2 = pomocni.begin(); it2 != pomocni.end(); it2++) {
            nova_stanja.insert(*it2);
         }
      }  
      nova_stanja.erase(it);
   }
   return sva_stanja;
}

set<int> epsilon_okruzenje(set<int> stanja, char znak, string automat) {
   set<int> stanja_nakon_prijelaza;
   for (auto it = stanja.begin(); it != stanja.end(); it++) {
      if (prijelazi[automat].find(*it) != prijelazi[automat].end()) {
         if (prijelazi[automat][*it].second == znak) 
            stanja_nakon_prijelaza.insert(prijelazi[automat][*it].first);
      }
   }
   set<int> sva_stanja;
   for (auto it = stanja_nakon_prijelaza.begin(); it != stanja_nakon_prijelaza.end(); it++) {
      set<int> temp = epsilon_okruzenje(*it, automat);
      sva_stanja.insert(temp.begin(), temp.end());
   }

   return sva_stanja;
}

int presjek(set<int> R, string automat) {
   for (auto it = R.begin(); it != R.end(); it++) {
      if (prihvatljivaStanja[automat].find(*it) != prihvatljivaStanja[automat].end())
         return *it;
   }
   return -1;
}

void analiziraj(string ulazniNiz) {
   size_t pocetak = 0;
   size_t zavrsetak = -1;
   int posljednji = 0;
   string izraz = "/";
   char procitaniZnak;
   tuple<string, string, string, int> akcije;
   int vratiZa = -1;

   set<int> R = epsilon_okruzenje(0, trenutnoStanje);
   set<int> Q;
   while (pocetak < ulazniNiz.length()) {
      //gotov
      if (R.empty()) {
         //leksicka pogreska
         if (izraz == "/") {
            cerr << ulazniNiz[pocetak];
            zavrsetak = pocetak;
            pocetak++;
            R = epsilon_okruzenje(0, trenutnoStanje);
         }
         //naso je leksicku jedinku
         else {
            if (get<2>(akcije) != "-")
               trenutnoStanje = get<2>(akcije);
            if (get<3>(akcije) != -1)
               vratiZa = get<3>(akcije);
            if (vratiZa == -1) {
               if (get<0>(akcije) != "-")
                  cout << izraz << " " << redak << " " << ulazniNiz.substr(pocetak, posljednji-pocetak+1) << endl;
               pocetak = posljednji + 1;
               zavrsetak = posljednji;
            }
            else {
               if (get<0>(akcije) != "-")
                  cout << izraz << " " << redak << " " << ulazniNiz.substr(pocetak, vratiZa) << endl;
               pocetak += vratiZa;
               zavrsetak = pocetak-1;
               vratiZa = -1;
            }
            if (get<1>(akcije) != "-")
               redak++;
            izraz = "/";
            R = epsilon_okruzenje(0, trenutnoStanje);
         }
      }
      else {
         int presjekStanje = presjek(R, trenutnoStanje);
         //prazan skup - nije naso jedinku
         if (presjekStanje < 0) {
            procitaniZnak = ulazniNiz[++zavrsetak];
            Q = R;
            R = epsilon_okruzenje(Q, procitaniZnak, trenutnoStanje);
         }
         //naso je jedinku, ali treba provjerit jel ima jos iza
         else {
            akcije = akcijeZaIzraz[trenutnoStanje][presjekStanje];
            izraz = get<0>(akcijeZaIzraz[trenutnoStanje][presjekStanje]);
            posljednji = zavrsetak;
            zavrsetak++;
            Q = R;
            procitaniZnak = ulazniNiz[zavrsetak];
            if (zavrsetak == ulazniNiz.length())
               R = {};
            else {
               R = epsilon_okruzenje(Q, procitaniZnak, trenutnoStanje);
            }
         }
      }
   }
}

int main() {
   deserializeBinary("tablica.bin", stanja, prihvatljivaStanja, akcijeZaIzraz, epsilonPrijelazi, prijelazi);

   string s, ulazniNiz = "";
   trenutnoStanje = stanja[0];
   while(getline(cin, s)) {
      s = s + '\n';
      ulazniNiz += s;
   }
   analiziraj(ulazniNiz);

   return 0;
}