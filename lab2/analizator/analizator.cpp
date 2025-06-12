#include <iostream>
#include <stack>
#include <map>
#include <vector>
#include <fstream>
#include <algorithm>
#include "deserialization.h"

using namespace std;

struct Cvor {
   string znak;
   int stanje;
   vector<Cvor*> djeca;
   Cvor() {
      znak = "";
      stanje = -1;
      djeca = {};
   }
   Cvor(const string& z) : znak(z), stanje(-1), djeca({}) {}
};

vector<vector<string>> produkcije_po_redu;
vector<vector<string>> akcija_novostanje;
vector<string> sinkronizacijski_znakovi;
map<string, int> znakoviInt;
vector<Cvor> dostupniCvorovi;
ofstream myFile;

vector<string> split(string s, string delim, int koliko) {
   vector<string> izlaz;
   int start = 0, end;
   for (int i = 0; i < koliko; i++) {
      end = s.find(delim, start);
      izlaz.push_back(s.substr(start, end - start));
      start = end + delim.length();
   }
   izlaz.push_back(s.substr(start));
   return izlaz;
}

void ispisi_stablo(Cvor cvor, int dubina) {
   for (int i = 0; i < dubina; i++) {
      //myFile << ' ';
      cout << ' ';
   }
   //myFile << cvor.znak << '\n';
   cout << cvor.znak << endl;
   reverse(cvor.djeca.begin(), cvor.djeca.end());
   for (Cvor* dijete : cvor.djeca) {
      ispisi_stablo(*dijete, dubina+1);
   }
}

void analiziraj(vector<string> uniformni_znakovi) {
   //myFile.open("izlaz.txt");
   //myFile.open("izlaz.txt", std::ios::binary);
   stack<Cvor> stog;
   Cvor pocetni_znak_stoga, pocetno_stanje;
   pocetni_znak_stoga.znak = "¬";
   pocetno_stanje.stanje = 0;
   stog.push(pocetni_znak_stoga);
   stog.push(pocetno_stanje);
   int glava = 0;
   bool prihvati = false;
   Cvor korijen;
   while (!prihvati) {
      int trenutno_stanje = stog.top().stanje;
      vector<string> uniformni_znak = split(uniformni_znakovi[glava], " ", 2);
      string akcija = akcija_novostanje[trenutno_stanje][znakoviInt[uniformni_znak[0]]];
      if (akcija[0] == 'p') {
         Cvor list;
         list.znak = uniformni_znak[0] + " " + uniformni_znak[1] + " " + uniformni_znak[2];
         stog.push(list);
         glava++;
         Cvor stanje;
         stanje.stanje = stoi(akcija.substr(1));
         stog.push(stanje);
      }
      else if (akcija[0] == 'r') {
         vector<string> produkcija = produkcije_po_redu[stoi(akcija.substr(1))];
         int duljina;
         if (produkcija[1] == "$") 
            duljina = 0;
         else
            duljina = (produkcija.size()-1)*2;
         Cvor cvor;
         cvor.znak = produkcija[0];
         if (duljina == 0)
            cvor.djeca.push_back(new Cvor("$"));
         for (int i = 0; i < duljina; i++) {
            if (i%2 == 1) {
               cvor.djeca.push_back(new Cvor(stog.top()));
            }
            stog.pop();
         }
         trenutno_stanje = stog.top().stanje;
         stog.push(cvor);
         akcija = akcija_novostanje[trenutno_stanje][znakoviInt[cvor.znak]];
         Cvor stanje;
         stanje.stanje = stoi(akcija.substr(1));
         stog.push(stanje);
      }
      else if (akcija == "") {
         cerr << uniformni_znak[1] << " " << uniformni_znak[0] << " " << uniformni_znak[2];
         // do {
         //    glava++;
         //    vector<string> uniformni_znak = split(uniformni_znakovi[glava], " ", 2);
         // } while(find(sinkronizacijski_znakovi.begin(), sinkronizacijski_znakovi.end(), uniformni_znak[2]) == sinkronizacijski_znakovi.end());
         while(find(sinkronizacijski_znakovi.begin(), sinkronizacijski_znakovi.end(), uniformni_znak[0]) == sinkronizacijski_znakovi.end()) {
            glava++;
            uniformni_znak = split(uniformni_znakovi[glava], " ", 2);
         }
         Cvor list; 
         //uniformni_znak = split(uniformni_znakovi[glava], " ", 2);
         list.znak = uniformni_znak[0] + " " + uniformni_znak[1] + " " + uniformni_znak[2];
         bool naso_stanje = false;
         while (!naso_stanje) {
            Cvor cvor_na_stogu = stog.top();
            if (cvor_na_stogu.stanje >= 0) {
               string akcija = akcija_novostanje[cvor_na_stogu.stanje][znakoviInt[uniformni_znak[0]]];
               if (akcija != "") {
                  if (akcija[0] == 'p') {
                     stog.push(list);
                     Cvor stanje;
                     stanje.stanje = stoi(akcija_novostanje[cvor_na_stogu.stanje][znakoviInt[uniformni_znak[0]]].substr(1));
                     stog.push(stanje);
                     glava++;
                     naso_stanje = true;
                  }
                  else if (akcija[0] == 'r') {
                     vector<string> produkcija = produkcije_po_redu[stoi(akcija.substr(1))];
                     int duljina;
                     if (produkcija[1] == "$") 
                        duljina = 0;
                     else
                        duljina = (produkcija.size()-1)*2;
                     Cvor cvor;
                     cvor.znak = produkcija[0];
                     if (duljina == 0)
                        cvor.djeca.push_back(new Cvor("$"));
                     for (int i = 0; i < duljina; i++) {
                        if (i%2 == 1) {
                           cvor.djeca.push_back(new Cvor(stog.top()));
                        }
                        stog.pop();
                     }
                     trenutno_stanje = stog.top().stanje;
                     stog.push(cvor);
                     akcija = akcija_novostanje[trenutno_stanje][znakoviInt[cvor.znak]];
                     Cvor stanje;
                     stanje.stanje = stoi(akcija.substr(1));
                     stog.push(stanje);
                     naso_stanje = true;
                  }
               }
               else
                  stog.pop();
            }
            else {
               for (Cvor* dijete : cvor_na_stogu.djeca) {
                  delete dijete;
               }
               cvor_na_stogu.djeca.clear();
               stog.pop();
            }
         }
      }
      else if (akcija == "Prihvati") {
         stog.pop();
         korijen = stog.top();
         prihvati = true;
      }
   }

   ispisi_stablo(korijen, 0);
   //myFile.close();
}

int main() {
   deserializeBinary("tablica.bin", produkcije_po_redu, akcija_novostanje, sinkronizacijski_znakovi, znakoviInt);

   vector<string> uniformni_znakovi;
   string s;
   while (getline(cin, s)) {
      uniformni_znakovi.push_back(s);
   }
   uniformni_znakovi.push_back("# # #");
   analiziraj(uniformni_znakovi);

   return 0;
}