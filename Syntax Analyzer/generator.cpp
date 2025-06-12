#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <fstream>
#include "serialization.h"

using namespace std;

struct lrStavka {
   string lijevaStrana;
   vector<string> desnaStrana;
   set<string> znakovi;

   bool operator <(const lrStavka& other) const {
      if (lijevaStrana != other.lijevaStrana) {
         return lijevaStrana < other.lijevaStrana;
      }
      if (desnaStrana != other.desnaStrana) {
         return desnaStrana < other.desnaStrana;
      }
      return znakovi < other.znakovi; 
   }
   bool operator ==(const lrStavka& other) const {
      return lijevaStrana == other.lijevaStrana &&
             desnaStrana == other.desnaStrana &&
             znakovi == other.znakovi;
    }
};

string s;
vector<string> nezavrsniZnakovi;
vector <string> zavrsniZnakovi;
vector<string> sviZnakovi;
vector<string> sinkronizacijskiZnakovi;
string pocetniNezavrsni;
map<string, vector<vector<string>>> produkcije;
vector<vector<string>> produkcije_po_redu;
set<string> prazniZnakovi;
vector<vector<int>> zapocinjeZnakom;
map<string, int> znakoviInt;
map<string, vector<lrStavka>> lrStavke;
map<lrStavka, vector<pair<lrStavka, string>>> enka;
map<int, vector<pair<int, string>>> dka;
map<vector<lrStavka>, int> stanjaDka_u_int;
map<int, vector<lrStavka>> stanjaDka;
vector<vector<string>> akcija_novostanje;
int broj_stanja_dka;

void izradi_tablicu_parsera() {
   for (int i = 0; i < broj_stanja_dka; i++) {
      if (dka.find(i) != dka.end()) {
         for (pair<int, string> prijelaz : dka[i]) {
            if (find(zavrsniZnakovi.begin(), zavrsniZnakovi.end(), prijelaz.second) != zavrsniZnakovi.end())
               akcija_novostanje[i][znakoviInt[prijelaz.second]] = "p" + to_string(prijelaz.first);
            if (find(nezavrsniZnakovi.begin(), nezavrsniZnakovi.end(), prijelaz.second) != nezavrsniZnakovi.end())
               akcija_novostanje[i][znakoviInt[prijelaz.second]] = "s" + to_string(prijelaz.first);
         }
      }
      vector<pair<vector<string>, set<string>>> potpune;
      for (lrStavka stavka : stanjaDka[i]) {
         if (stavka.desnaStrana.back() == ".") {
            pair<vector<string>, set<string>> potp = make_pair(stavka.desnaStrana, stavka.znakovi);
            potp.first.pop_back();
            if (stavka.desnaStrana[0] == ".")
               potp.first.push_back("$");
            potp.first.insert(potp.first.begin(), stavka.lijevaStrana);
            potpune.push_back(potp);
         }
      }
      vector<tuple<vector<string>, set<string>, int>> poredane_potpune;
      for (size_t j = 0; j < produkcije_po_redu.size(); j++) {
         for (auto prod : potpune) {
            if (prod.first == produkcije_po_redu[j]) {
               poredane_potpune.push_back(make_tuple(prod.first, prod.second, j));
            }
         }
      }

      for (size_t j = 0; j < poredane_potpune.size(); j++) {
         for (string znak : get<1>(poredane_potpune[j])) {
               if (akcija_novostanje[i][znakoviInt[znak]] == "")
                  akcija_novostanje[i][znakoviInt[znak]] = "r" + to_string(get<2>(poredane_potpune[j]));
               if ((get<0>(poredane_potpune[j]))[0] == "<S'>" && (get<0>(poredane_potpune[j]))[1] == pocetniNezavrsni)
                  akcija_novostanje[i][znakoviInt[znak]] = "Prihvati";  
         }
      }
   }
}

vector<lrStavka> epsilon_okruzenje(vector<lrStavka> stanje) {
   queue<lrStavka> queue;
   for (lrStavka it : stanje) 
      queue.push(it);
   while (!queue.empty()) {
      lrStavka trenutna_stavka = queue.front();
      queue.pop();
      if (enka.find(trenutna_stavka) == enka.end())
            continue;
      for (pair<lrStavka, string> stavka_znak : enka[trenutna_stavka]) {
         if (stavka_znak.second == "$" && find(stanje.begin(), stanje.end(), stavka_znak.first) == stanje.end()) {
            stanje.push_back(stavka_znak.first);
            queue.push(stavka_znak.first);
         }
      }     
   }
   return stanje;
}

void pretvori_u_dka() {
   int rbr_stanja = 0;
   vector<lrStavka> stanje;
   stanje.push_back(lrStavke["<S'>"][0]);
   stanje = epsilon_okruzenje(stanje);
   stanjaDka_u_int[stanje] = rbr_stanja;
   stanjaDka[rbr_stanja] = stanje;

   stack<pair<vector<lrStavka>, int>> nova_stanja_dka;
   nova_stanja_dka.push(make_pair(stanje, rbr_stanja));
   while (!nova_stanja_dka.empty()) {
      pair<vector<lrStavka>, int> trenutno_stanje = nova_stanja_dka.top();
      nova_stanja_dka.pop();
      for (string znak : sviZnakovi) {
         vector<lrStavka> novo_stanje{};
         novo_stanje.clear();
         for (size_t i = 0; i < trenutno_stanje.first.size(); i++) {
            if (enka.find(trenutno_stanje.first[i]) == enka.end())
               continue;
            vector<pair<lrStavka, string>> prlz = enka[trenutno_stanje.first[i]];
            for (size_t i = 0; i < prlz.size(); i++) {
               if (prlz[i].second == znak) {
                  novo_stanje.push_back(prlz[i].first);
               }
            }
         }
         if (!novo_stanje.empty()) {
            novo_stanje = epsilon_okruzenje(novo_stanje);
            int redni_broj;
            sort(novo_stanje.begin(), novo_stanje.end());
            if (stanjaDka_u_int.find(novo_stanje) == stanjaDka_u_int.end()) {
               rbr_stanja++;
               redni_broj = rbr_stanja;
               stanjaDka_u_int[novo_stanje] = redni_broj;
               stanjaDka[redni_broj] = novo_stanje;
               dka[trenutno_stanje.second].push_back(make_pair(redni_broj, znak));
               nova_stanja_dka.push(make_pair(novo_stanje, redni_broj));    
            }
            else {
               redni_broj = stanjaDka_u_int[novo_stanje];
               dka[trenutno_stanje.second].push_back(make_pair(redni_broj, znak));
            }
         }      
      }
   }
}

bool jeli_prazan(vector<string> izraz) {
   for (string s : izraz) {
      if (prazniZnakovi.find(s) == prazniZnakovi.end())
         return false;
   }
   return true;
}

set<string> zapocinje(vector<string> produkcija) {
   set<string> znakovi{};
   if (produkcija.empty() || produkcija[0] == "$")
      return znakovi;
   for (size_t i = 0; i < produkcija.size(); i++) {
      for (size_t j = nezavrsniZnakovi.size(); j < nezavrsniZnakovi.size() + zavrsniZnakovi.size(); j++) {
         if (zapocinjeZnakom[znakoviInt[produkcija[i]]][j] == 1)
            znakovi.insert(zavrsniZnakovi[j - nezavrsniZnakovi.size()]);
      }
      if (prazniZnakovi.find(produkcija[i]) == prazniZnakovi.end())
         break;
   }
   return znakovi;
}

void stvori_prijelaze(lrStavka stavka) {
   vector<string> produkcija = stavka.desnaStrana;
   for (size_t i = 0; i < produkcija.size(); i++) {
      if (produkcija[0] == "$") {
         vector<string> desna{"."};
         lrStavka temp;
         temp.desnaStrana = desna;
         temp.lijevaStrana = stavka.lijevaStrana;
         temp.znakovi = stavka.znakovi;
         lrStavke[temp.lijevaStrana].push_back(temp);
         break;
      }
      // else if (i == 0)
      //    continue;
      vector<string> lijeva = stavka.desnaStrana;
      lijeva.insert(lijeva.begin()+i, ".");
      vector<string> desna = stavka.desnaStrana;
      if (i == produkcija.size() - 1)
         desna.push_back(".");
      else
         desna.insert(desna.begin()+i+1, ".");
      //stvori stavku u lrStavke
      lrStavka temp;
      temp.desnaStrana = desna;
      temp.lijevaStrana = stavka.lijevaStrana;
      temp.znakovi = stavka.znakovi;
      lrStavke[stavka.lijevaStrana].push_back(temp);

      //dodaj prijelaz u enka
      lrStavka enka_lijevo;
      enka_lijevo.lijevaStrana = stavka.lijevaStrana;
      vector<string> lijeva_enka = stavka.desnaStrana;
      lijeva_enka.insert(lijeva_enka.begin()+i, ".");
      enka_lijevo.desnaStrana = lijeva_enka;
      enka_lijevo.znakovi = stavka.znakovi;
      enka[enka_lijevo].push_back(make_pair(temp, produkcija[i]));

      if (find(nezavrsniZnakovi.begin(), nezavrsniZnakovi.end(), produkcija[i]) != nezavrsniZnakovi.end()) {
         vector<lrStavka> sve_prod;
         vector<string> beta{};
         for (size_t j = i+1; j < stavka.desnaStrana.size(); j++) {
            beta.push_back(stavka.desnaStrana[j]);
         }
         set<string> novi_znakovi = zapocinje(beta);
         if (jeli_prazan(beta))
            novi_znakovi.insert(stavka.znakovi.begin(), stavka.znakovi.end());
         
         for (vector<string> prod : produkcije[produkcija[i]]) {
            vector<string> vec_temp = prod;
            lrStavka temp_stavka;
            if (vec_temp[0] == "$")
               vec_temp[0] = ".";
            else
               vec_temp.insert(vec_temp.begin(), ".");
            temp_stavka.desnaStrana = vec_temp;
            temp_stavka.lijevaStrana = produkcija[i];
            temp_stavka.znakovi = novi_znakovi;
            sve_prod.push_back(temp_stavka);
         }
         for (lrStavka epsiloni : sve_prod) {
            enka[enka_lijevo].push_back(make_pair(epsiloni, "$"));
         }

         if (find(lrStavke[produkcija[i]].begin(), lrStavke[produkcija[i]].end(), /*enka_lijevo*/sve_prod[0]) == lrStavke[produkcija[i]].end()) {
            for (lrStavka svaka : sve_prod) {
               lrStavke[produkcija[i]].push_back(svaka);
            }
            for (lrStavka svaka : sve_prod) {
               svaka.desnaStrana.erase(svaka.desnaStrana.begin());
               stvori_prijelaze(svaka);
            }
         }
      }
   }
}

void stvori_nka() {
   lrStavka pocetna; 
   pocetna.lijevaStrana = "<S'>";
   pocetna.desnaStrana = produkcije["<S'>"][0];
   pocetna.desnaStrana.insert(pocetna.desnaStrana.begin(), ".");
   set<string> zn{"#"};
   pocetna.znakovi = zn;
   lrStavke["<S'>"].push_back(pocetna);
   pocetna.desnaStrana.erase(pocetna.desnaStrana.begin());
   stvori_prijelaze(pocetna);
}

void tablica_zapocinje_znakom() {
   vector<int> posjeceni(nezavrsniZnakovi.size() + zavrsniZnakovi.size(), 0);
   stack<int> stog;
   for (size_t i = 0; i < nezavrsniZnakovi.size(); i++) {
      fill(posjeceni.begin(), posjeceni.end(), 0);
      stog = stack<int>();
      stog.push(i);
      while (!stog.empty()) {
         int vrh = stog.top();
         posjeceni[vrh] = 1;
         zapocinjeZnakom[i][vrh] = 1;
         stog.pop();
         for (size_t j = 0; j < nezavrsniZnakovi.size() + zavrsniZnakovi.size(); j++) {
            if (zapocinjeZnakom[vrh][j] == 1 && !posjeceni[j]){
               if (j < nezavrsniZnakovi.size())
                  stog.push(j);
               else 
                  zapocinjeZnakom[i][j] = 1;
            }
         }
      }
   }
}

void izravno_zapocinje_znakom() {
   for (auto &it : produkcije) {
      for (vector<string> prod : it.second) {
         if (prod[0] != "$") {
            for (size_t i = 0; i < prod.size(); i++) {
               int redak = znakoviInt[it.first];
               int stupac = znakoviInt[prod[i]];
               zapocinjeZnakom[redak][stupac] = 1;
               if (prazniZnakovi.find(prod[i]) == prazniZnakovi.end())
                  break;
            }
         }
      }
   }
   for (size_t i = nezavrsniZnakovi.size(); i < nezavrsniZnakovi.size() + zavrsniZnakovi.size(); i++) {
      for (size_t j = nezavrsniZnakovi.size(); j < nezavrsniZnakovi.size() + zavrsniZnakovi.size(); j++) {
         if (i == j)
            zapocinjeZnakom[i][j] = 1;
      }
   }
}

void izracunaj_prazne() {
   bool novi = false, je_prazan;
   do {
      novi = false;
      je_prazan = true;
      for (auto &it : produkcije) {
         for (vector<string> prod : it.second) {
            je_prazan = true;
            for (string znak : prod) {
               if (prazniZnakovi.find(znak) == prazniZnakovi.end()) {
                  je_prazan = false;
                  break;
               }
            }
            if (je_prazan && (prazniZnakovi.find(it.first) == prazniZnakovi.end())) {
               prazniZnakovi.insert(it.first);
               novi = true;
            }
         }
      }
   } while(novi);
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

int main() {

   getline(cin, s);
   nezavrsniZnakovi = split(s.substr(3), " ");
   pocetniNezavrsni = nezavrsniZnakovi[0];

   getline(cin, s);
   zavrsniZnakovi = split(s.substr(3), " ");
   getline(cin, s);
   sinkronizacijskiZnakovi = split(s.substr(5), " ");
   sviZnakovi.insert(sviZnakovi.begin(), nezavrsniZnakovi.begin(), nezavrsniZnakovi.end());
   sviZnakovi.insert(sviZnakovi.end(), zavrsniZnakovi.begin(), zavrsniZnakovi.end());

   zapocinjeZnakom.resize(nezavrsniZnakovi.size() + zavrsniZnakovi.size(), vector<int>(nezavrsniZnakovi.size() + zavrsniZnakovi.size(), 0));

   for(size_t i = 0; i < nezavrsniZnakovi.size(); i++) {
      znakoviInt[nezavrsniZnakovi[i]] = i;
   }
   for(size_t i = nezavrsniZnakovi.size(); i < nezavrsniZnakovi.size() + zavrsniZnakovi.size(); i++) {
      znakoviInt[zavrsniZnakovi[i-nezavrsniZnakovi.size()]] = i;
   }
   vector<string> temp1{"<S'>", pocetniNezavrsni};
   produkcije_po_redu.push_back(temp1);
   
   string lijevaStrana;
   vector<string> desnaStrana;
   bool nastavi = false;
   getline(cin, s);
   do {
      nastavi = false;
      lijevaStrana = s;
      getline(cin, s);
      while(s[0] == ' ') {
         desnaStrana = split(s.substr(1), " ");
         if (desnaStrana[0] == "$")
            prazniZnakovi.insert(lijevaStrana);
         produkcije[lijevaStrana].push_back(desnaStrana);
         vector<string> cijela_produkcija = desnaStrana;
         cijela_produkcija.insert(cijela_produkcija.begin(), lijevaStrana);
         produkcije_po_redu.push_back(cijela_produkcija);
         getline(cin, s);
      }
      if (s[0] == '<')
         nastavi = true;
   } while (nastavi);

   izracunaj_prazne();
   izravno_zapocinje_znakom();
   tablica_zapocinje_znakom();

   vector<vector<string>> temp(1, vector<string>(1));
   temp[0][0] = pocetniNezavrsni;
   produkcije["<S'>"] = temp;
   znakoviInt["<S'>"] = zavrsniZnakovi.size() + nezavrsniZnakovi.size();
   znakoviInt["#"] = zavrsniZnakovi.size() + nezavrsniZnakovi.size() + 1;

   stvori_nka();
   pretvori_u_dka();
   broj_stanja_dka = stanjaDka.size();
   akcija_novostanje.resize(broj_stanja_dka, vector<string>(zavrsniZnakovi.size() + nezavrsniZnakovi.size() + 2, ""));
   izradi_tablicu_parsera();

   serializeBinary(produkcije_po_redu, akcija_novostanje, sinkronizacijskiZnakovi, znakoviInt, "./analizator/tablica.bin");

  // cout << "kraj";
   return 0;
}