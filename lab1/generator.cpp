#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <map>
#include <fstream>
#include "serialization.h"

using namespace std;

struct automat {
   int br_stanja = 0;
   string ime;
   int poz;

   bool operator <(const automat& other) const{
      return poz < other.poz;
   }
};

string s;
map<string, string> regDef;
vector<string> jedinke;
map<automat*, map<int, tuple<string, string, string, int>>> akcijeZaIzraz;
map<automat*, map<int, set<int>>> epsilonPrijelazi;
map<automat*, map<int, pair<int,char>>> prijelazi;
vector<string> stanja;
map<automat*, set<int>> prihvatljivaStanja;
map<string, map<int, tuple<string, string, string, int>>> akcijeZaIzraz_serialized;
map<string, map<int, set<int>>> epsilonPrijelazi_serialized;
map<string, map<int, pair<int,char>>> prijelazi_serialized;
map<string, set<int>> prihvatljivaStanja_serialized;

int novo_stanje(automat &automat) {
   automat.br_stanja++;
   return (automat.br_stanja - 1);
}

bool je_operator(string izraz, int i) {
   int brBackSlash = 0;
   while((i-1) >= 0 && izraz[i-1] == '\\') {
      brBackSlash++;
      i--;
   }
   return (brBackSlash%2 == 0);
}

void dodaj_epsilon_prijelaz(automat *automat, int lijevoStanje, int desnoStanje) {
   if (epsilonPrijelazi[automat].find(lijevoStanje) == epsilonPrijelazi[automat].end()) {
      set<int> stanja;
      stanja.insert(desnoStanje);
      epsilonPrijelazi[automat][lijevoStanje] = stanja;
   }
   else 
      epsilonPrijelazi[automat][lijevoStanje].insert(desnoStanje);
}

void dodaj_prijelaz(automat *automat, int prvoStanje, int drugoStanje, char znak) {
      pair<int, char> parovi = make_pair(drugoStanje, znak);
      prijelazi[automat][prvoStanje] = parovi;
}

pair<int, int> pretvori(string izraz, automat &automat) {
   vector<string> izbori;
   int brZagrada = 0;
   int zadnjiIzbor = 0;
   for (size_t i = 0; i < izraz.length(); i++) {
      if (izraz[i] == '(' && je_operator(izraz, i))
         brZagrada++;   
      else if (izraz[i] == ')' && je_operator(izraz, i))
         brZagrada--;
      else if(izraz[i] == '|' && brZagrada == 0 && je_operator(izraz, i)) {
         string s = izraz.substr(zadnjiIzbor, i-zadnjiIzbor);
         zadnjiIzbor = i+1;
         izbori.push_back(s);
      }
   }
   if (zadnjiIzbor != 0)
      izbori.push_back(izraz.substr(zadnjiIzbor));
   else
      izbori.push_back(izraz);

   int lijevoStanje = novo_stanje(automat);
   int desnoStanje = novo_stanje(automat);
   if (zadnjiIzbor != 0) {
      for (size_t i = 0; i < izbori.size(); i++) {
         pair<int, int> tempStanje = pretvori(izbori[i], automat);
         dodaj_epsilon_prijelaz(&automat, lijevoStanje, tempStanje.first);
         dodaj_epsilon_prijelaz(&automat, tempStanje.second, desnoStanje);
      }
   }
   else {
      bool prefiksirano = false;
      int zadnjeStanje = lijevoStanje;
      for (size_t i = 0; i < izraz.length(); i++) {
         int a, b;
         //slucaj 1 - znak je prefiksiran s \;
         if (prefiksirano) {
            prefiksirano = false;
            char prijelazniZnak;
            if (izraz[i] == 't') 
               prijelazniZnak = '\t';
            else if (izraz[i] == 'n')
               prijelazniZnak = '\n';
            else if (izraz[i] == '_')
               prijelazniZnak = ' ';
            else 
               prijelazniZnak = izraz[i];

            a = novo_stanje(automat);
            b = novo_stanje(automat);
            dodaj_prijelaz(&automat, a, b, prijelazniZnak);
         }
         //slucaj 2 - znak nije prefiksiran s \;
         else {
            if (izraz[i] == '\\') {
               prefiksirano = true;
               continue;
            }
            if (izraz[i] != '(') {
               a = novo_stanje(automat);
               b = novo_stanje(automat);
               if (izraz[i] == '$')
                  dodaj_epsilon_prijelaz(&automat, a, b);
               else 
                  dodaj_prijelaz(&automat, a, b, izraz[i]);
            }
            else if(izraz[i] == '(') {
               //pronadji pripadnu ) zagradu
               int pozZagrade = i+1;
               int brZagrada = 1;
               size_t j;
               for (j = i+1; j < izraz.length(); j++) {
                  if (izraz[j] == '(' && je_operator(izraz, j))
                     brZagrada++;
                  else if(izraz[j] == ')' && je_operator(izraz, j))
                     brZagrada--;
                  if (brZagrada == 0) {
                     pozZagrade = j;
                     break;
                  }
               }

               pair<int, int> tempStanje = pretvori(izraz.substr(i+1, pozZagrade-1-i), automat);
               a = tempStanje.first;
               b = tempStanje.second;
               i = j;
            }
         }
         
         //provjeri ponavljanje - kleeneov operator *
         if (i+1 < izraz.length() && izraz[i+1] == '*') {
            int x = a;
            int y = b;
            a = novo_stanje(automat);
            b = novo_stanje(automat);
            dodaj_epsilon_prijelaz(&automat, a, x);
            dodaj_epsilon_prijelaz(&automat, a, b);
            dodaj_epsilon_prijelaz(&automat, y, b);
            dodaj_epsilon_prijelaz(&automat, y, x);
            i++;
         }

         //povezi s ostatkom automata
         dodaj_epsilon_prijelaz(&automat, zadnjeStanje, a);
         zadnjeStanje = b;
      }
      //cout << lijevoStanje << " " << zadnjeStanje << " ";
      dodaj_epsilon_prijelaz(&automat, zadnjeStanje, desnoStanje);
      set<int> temp = epsilonPrijelazi[&automat][zadnjeStanje];
      // for (auto it = temp.begin(); it != temp.end(); it++) {
      //    cout << *it << " ";
      // }
   }
   return make_pair(lijevoStanje, desnoStanje);
}

vector<string> split(string s, string delim) {
   vector<string> izlaz;
   int start = 0;
   int end = s.find(delim);
   while(end != -1) {
      izlaz.push_back(s.substr(start, end - start));
      start = end + delim.length();
      end = s.find(delim, start);
   }
   izlaz.push_back(s.substr(start, end));
   return izlaz;
}

void dereferencirajPocetne() {
   for (auto &it : regDef) {
      string def = it.second;
      size_t pozRef = def.find("{");
      while(pozRef != string::npos) {
         if (je_operator(def, pozRef)) {
            string imeRef = def.substr(pozRef + 1, def.find("}", pozRef)-1-pozRef);
            def.replace(pozRef, imeRef.length()+2, "(" + regDef[imeRef] + ")");
         }
         pozRef = def.find("{", pozRef+1);
      }
      it.second = def;
   }
}

string dereferencirajIzraz(string izraz) {
   size_t pozRef = izraz.find_first_of("{");
   while (pozRef != string::npos) {
      if (je_operator(izraz, pozRef)) {
         string imeRef = izraz.substr(pozRef+1, izraz.find('}', pozRef)-pozRef-1);
         izraz.replace(pozRef, imeRef.length()+2, '(' + regDef[imeRef] + ')');
      }
      pozRef = izraz.find('{', pozRef+1);
   }
   return izraz;
}

int main(void) {

   getline(cin, s);
   while(s[0] == '{') {
      string ime = s.substr(1, s.find_first_of("}")-1);
      regDef[ime] = s.substr(s.find_first_of("}") + 2);
      getline(cin, s);
   }
   dereferencirajPocetne();

   s = s.substr(3);
   stanja = split(s, " ");
   getline(cin, s);
   s = s.substr(3);
   jedinke = split(s, " ");

   vector<automat> automatiZaStanja;
   map<string, int> redniBrojAutomata;
   for (size_t i = 0; i < stanja.size(); i++) {
      automat a;
      a.ime = stanja[i];
      a.poz = i;
      novo_stanje(a);
      automatiZaStanja.push_back(a);
      redniBrojAutomata[a.ime] = i;
   }

   while(getline(cin, s)) {
      string stanje = s.substr(1, s.find_first_of('>')-1);
      string izraz = s.substr(s.find_first_of('>') + 1);
      pair<int, int> lijevoDesno;
      
      string dereferenciran = dereferencirajIzraz(izraz);
      automat* trenutniAutomat = &automatiZaStanja[redniBrojAutomata[stanje]];
      lijevoDesno = pretvori(dereferenciran, *trenutniAutomat);
      //cout << lijevoDesno.first << " " << lijevoDesno.second << " " << dereferenciran << endl;
      prihvatljivaStanja[trenutniAutomat].insert(lijevoDesno.second);
      dodaj_epsilon_prijelaz(trenutniAutomat, 0, lijevoDesno.first);
      akcijeZaIzraz[trenutniAutomat][lijevoDesno.second] = make_tuple("-", "-", "-", -1);
      
      getline(cin, s);
      while (s[0] != '}') {
         if (s != "{") {
            vector<string> temp_stanje = split(s, " "); 
            if (temp_stanje[0] == "NOVI_REDAK")
               get<1>(akcijeZaIzraz[trenutniAutomat][lijevoDesno.second]) = "+";
            else if (temp_stanje[0] == "UDJI_U_STANJE") {
               vector<string> temp_stanje = split(s, " ");
               get<2>(akcijeZaIzraz[trenutniAutomat][lijevoDesno.second]) = temp_stanje[1];
            }
            else if (temp_stanje[0] == "VRATI_SE") {
               vector<string> temp_stanje = split(s, " ");
               get<3>(akcijeZaIzraz[trenutniAutomat][lijevoDesno.second]) = stoi(temp_stanje[1]);
            }
            else
               get<0>(akcijeZaIzraz[trenutniAutomat][lijevoDesno.second]) = s;
         }
         getline(cin, s);
      }
   }

   for (auto &it : prijelazi) {
      prijelazi_serialized[it.first->ime] = it.second;
   } 
   for (auto &it : epsilonPrijelazi) {
      epsilonPrijelazi_serialized[it.first->ime] = it.second;
   }
   for (auto &it : akcijeZaIzraz) {
      akcijeZaIzraz_serialized[it.first->ime] = it.second;
   }
   for (auto &it : prihvatljivaStanja) {
      prihvatljivaStanja_serialized[it.first->ime] = it.second;
   }
   serializeBinary(stanja, prihvatljivaStanja_serialized, akcijeZaIzraz_serialized, epsilonPrijelazi_serialized, prijelazi_serialized, "analizator/tablica.bin");

   return 0;
}