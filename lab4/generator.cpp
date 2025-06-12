#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

ofstream file("a.frisc");

struct citac {
   string izraz;
   string klasa;
   string vrijednost;
   string redak;
   int glava;
   int duljina;
   int dubina;
   citac() {
      izraz = "";
      klasa = "";
      vrijednost = "";
      redak = "";
      glava = 0;
      duljina = 0;
      dubina = 0;
   }
};

struct tablica_znakova {
   map<string, pair<string, string>> tablica;
   tablica_znakova* ugnijezdjeni_blok;
   string return_vrijednost, parametri;
   vector<string> parametri_funkcije;
   vector<string> varijable;
   bool u_petlji;
   bool returno;
   int na_stogu;
   tablica_znakova() {
      ugnijezdjeni_blok = nullptr;
      return_vrijednost = "";
      parametri = "";
      u_petlji = false;
      returno = false;
      na_stogu = 0;
   }
};

citac ulaz;
vector<string> ulazni_niz;
vector<string> ulazni_niz_procitano;
bool zadnja_linija = false;
tablica_znakova* trenutna_tablica = new tablica_znakova();
bool pozvala_petlja = false;
map<string, string> definirane_funkcije;
map<string, set<string>> deklarirane_funkcije;
//int trenutna_tablica->na_stogu = 0;
vector<string> globalne_varijable;
vector<string> char_nizovi, int_nizovi;
bool stavi_minus = false;
bool unutar_ifa = false;
bool niz_char, niz_int = false;
bool pozvo_main = false;
int redaka = 0;


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

void iduca_linija() {
   string linija_ulaza = ulazni_niz[ulaz.glava];
   ulaz.glava++;
   int i = 0;
   ulaz.dubina = 0;
   ulazni_niz_procitano.push_back(linija_ulaza);
   while(linija_ulaza[i] == ' ') {
      linija_ulaza = linija_ulaza.substr(1);
      ulaz.dubina++;
   }
   ulaz.izraz = linija_ulaza;
   if (linija_ulaza[0] == '<') {
      ulaz.klasa = "";
      ulaz.vrijednost = "";
   }
   else {
      vector<string> linija_ulaza_vec = split(linija_ulaza, " ");
      ulaz.klasa = linija_ulaza_vec[0];
      ulaz.redak = linija_ulaza_vec[1];
      ulaz.vrijednost = linija_ulaza_vec[2];
   }
}

int izracunaj_dubinu(string linija) {
   int i = 0, dubina = 0;
   while (linija[i] == ' ') {
      dubina++;
      i++;
   }
   return dubina;
}

void greska(int dubina_produkcije) {
   if (dubina_produkcije == -1) {
      cout << "main" << endl;
      exit(0);
   }
   if (dubina_produkcije == -2) {
      cout << "funkcija" << endl;
      exit(0);
   }
   vector<string> produkcija;
   int i = 0;
   int len = ulazni_niz_procitano.size();
   int dubina_linije = izracunaj_dubinu(ulazni_niz_procitano[len-i-1]);
   while(dubina_linije != dubina_produkcije) {
      if (dubina_linije == dubina_produkcije+1)
         produkcija.emplace(produkcija.begin(), ulazni_niz_procitano[len-1-i].substr(dubina_linije));
      i++;
      dubina_linije = izracunaj_dubinu(ulazni_niz_procitano[len-i-1]);
   }
   produkcija.emplace(produkcija.begin(), ulazni_niz_procitano[len-1-i].substr(dubina_linije));
   iduca_linija();
   while(!(ulaz.dubina <= dubina_produkcije) && (size_t)ulaz.glava < ulazni_niz.size()) {
      if (ulaz.dubina == dubina_produkcije+1)
         produkcija.push_back(ulaz.izraz);
      iduca_linija();
   }
   cout << produkcija[0] << " ::=";
   for (size_t i = 1; i < produkcija.size(); i++) {
      if (produkcija[i][0] != '<') {
         vector<string> jedinka = split(produkcija[i], " ");
         cout << " " << jedinka[0] << "(" << jedinka[1] << "," << jedinka[2] << ")";
      }
      else {
         cout << " " << produkcija[i];
      }
   }
   cout << endl;
   exit(0);
}

void vrati_se_liniju_nazad() {
   ulaz.glava--;
   ulazni_niz_procitano.erase(ulazni_niz_procitano.end());
   ulaz.glava--;
   ulazni_niz_procitano.erase(ulazni_niz_procitano.end());
   iduca_linija();
}

string dohvati_iz_tablice(string ime) {
   tablica_znakova* tablica_zn = trenutna_tablica;
   while(tablica_zn != nullptr) {
      if (tablica_zn->tablica.find(ime) != tablica_zn->tablica.end()) {
         if (tablica_zn->tablica[ime].first == "funkcija") {
            return "funkcija " + tablica_zn->tablica[ime].second;
         }
         return tablica_zn->tablica[ime].second;
      }
      tablica_zn = tablica_zn->ugnijezdjeni_blok;
   }
   return "";
}

bool moze_li_se_pretvoriti_u_int(string tip) {
   if (tip == "int" || tip == "const int" || tip == "char")
      return true;
   return false;
}

bool moze_li_se_pretvoriti_u_vrijednost(string tip, string vrijednost) {
   if (vrijednost == "int") {
      if (tip == "int" || tip == "const_int" || tip == "char")
         return true;
   }
   else if (vrijednost == "char") {
      if (tip == "char" || tip == "const_char")
         return true;
   }
   else if (vrijednost == "niz_const_int") {
      if (tip == "niz_int" || tip == "niz_const_int")
         return true;
   }
   else if (vrijednost == "niz_const_char") {
      if (tip == "niz_char" || tip == "niz_const_char")
         return true;
   }
   else if (vrijednost == "niz_int") {
      if (tip == "niz_int" || tip == "niz_const_int")
         return true;
   }
   else if (vrijednost == "niz_char") {
      if (tip == "niz_char" || tip == "niz_const_char")
         return true;
   }
   return false;
}

bool moze_li_se_pretvoriti_jedan_u_drugi(string tip1, string tip2) {
   if ((tip1 == "niz_int" && tip2 == "niz_int") || (tip1 == "niz_char" && tip2 == "niz_char") ||
       (tip1 == "char" && tip2 == "char") || (tip1 == "const_char" && tip2 == "const_char") ||
       (tip1 == "int" && tip2 == "int") || (tip1 == "const_int" && tip2 == "const_int") ||
       (tip1 == "int" && tip2 == "const_int") || (tip1 == "const_int" && tip2 == "int") ||
       (tip1 == "char" && tip2 == "const_char") || (tip1 == "char" && tip2 == "int") ||
       (tip1 == "const_char" && tip2 == "char") || (tip1 == "niz_int" && tip2 == "niz_const_int") ||
       (tip1 == "niz_char" && tip2 == "niz_const_char"))
      return true;
   return false;
}

void stvori_novi_djelokrug() {
   tablica_znakova* nova_tablica = new tablica_znakova();
   bool u_petlji = trenutna_tablica->u_petlji;
   string return_vr = trenutna_tablica->return_vrijednost;
   string parametri = trenutna_tablica->parametri;
   nova_tablica->ugnijezdjeni_blok = trenutna_tablica;
   trenutna_tablica = nova_tablica;
   if (pozvala_petlja || u_petlji) {
      trenutna_tablica->u_petlji = true;
   }
   trenutna_tablica->return_vrijednost = return_vr;
   trenutna_tablica->parametri = parametri;
   pozvala_petlja = false;
}

void unisti_trenutni_djelokrug() {
   tablica_znakova* roditelj_djelokrug = trenutna_tablica->ugnijezdjeni_blok;
   delete trenutna_tablica;
   trenutna_tablica = roditelj_djelokrug;
}

void konacne_provjere() {
   if (definirane_funkcije.find("main") == definirane_funkcije.end())
      greska(-1);
   if (definirane_funkcije["main"] != "void int")
      greska(-1);
   for (auto it = deklarirane_funkcije.begin(); it != deklarirane_funkcije.end(); it++) {
      if (it->second.size() > 1)
         greska(-2);
      else if (definirane_funkcije.find(it->first) == definirane_funkcije.end())
         greska(-2);
      else if (definirane_funkcije[it->first] != *deklarirane_funkcije[it->first].begin())
         greska(-2);
   }
}

string to_upper(string s) {
   for (auto it = s.begin(); it != s.end(); it++) {
      *it = toupper(*it);
   }
   return s;
}

bool is_number(string s) {
   auto it = s.begin();
   if (s[0] == '-')
      it++;
   while(it != s.end() && isdigit(*it)) {
      it++;
      if (*it == 'c')
         it++;
   }
   return !s.empty() && it == s.end();
}


void greska(int dubina_produkcije);
void primarni_izraz(string &tip, bool &l_izraz);
void izraz(string &tip, bool &l_izraz);
void postfiks_izraz(string &tip, bool &l_izraz);
void lista_argumenata(string &tipovi);
void izraz_pridruzivanja(string &tip, bool &l_izraz);
void unarni_izraz(string &tip, bool &l_izraz);
void cast_izraz(string &tip, bool &l_izraz);
void unarni_izraz(string &tip, bool &l_izraz);
void ime_tipa(string &tip);
void specifikator_tipa(string &tip);
void multiplikativni_izraz(string &tip, bool &l_izraz);
void aditivni_izraz(string &tip, bool &l_izraz);
void odnosni_izraz(string &tip, bool &l_izraz);
void jednakosni_izraz(string &tip, bool &l_izraz);
void bin_i_izraz(string &tip, bool &l_izraz);
void bin_xili_izraz(string &tip, bool &l_izraz);
void bin_ili_izraz(string &tip, bool &l_izraz);
void log_i_izraz(string &tip, bool &l_izraz);
void log_ili_izraz(string &tip, bool &l_izraz);
void izraz_pridruzivanja(string &tip, bool &l_izraz);
void izraz(string &tip, bool &l_izraz);
void slozena_naredba();
void lista_naredbi();
void lista_deklaracija();
void naredba();
void izraz_naredba(string &tip);
void naredba_grananja();
void naredba_petlje();
void naredba_skoka();
void prijevodna_jedinica();
void vanjska_deklaracija();
void definicija_funkcije();
void deklaracija();
void definicija_funkcije();
void lista_parametara(string &tipovi, string &imena);
void deklaracija_parametra(string &tip, string &ime);
void lista_deklaracija();
void lista_init_deklaratora(string ntip);
void init_deklarator(string ntip);
void izravni_deklarator(string ntip, string &tip, int &br_elem); 
void inicijalizator(int &br_elem, string &tipovi, string &tip);
void lista_izraza_pridruzivanja(int &br_elem, string &tipovi);


void lista_izraza_pridruzivanja(int &br_elem, string &tipovi) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<izraz_pridruzivanja>") {
      string tip;
      bool l_izraz;
      izraz_pridruzivanja(tip, l_izraz);
      tipovi = tip;
      br_elem = 1;
      if (niz_char) {
         file << "\tSTOREB R0, (R1+0)\n";
         redaka++;
      }
      else if (niz_int) {
         file << "\tSTORE R0, (R1+0)\n";
         redaka++;
      }
   }
   else if (ulaz.izraz == "<lista_izraza_pridruzivanja>") {
      lista_izraza_pridruzivanja(br_elem, tipovi);
      iduca_linija();
      if (ulaz.klasa != "ZAREZ")
         greska(dubina);
      iduca_linija();
      string tip;
      bool l_izraz;
      izraz_pridruzivanja(tip, l_izraz);
      tipovi = tipovi + "-" + tip;
      br_elem++;
      vector<string> tipovi_vec = split(tipovi, "-");
      if (niz_char) {
         file << "\t`BASE D\n";
         file << "\tSTOREB R0, (R1+" + to_string(tipovi_vec.size()-1) + ")\n";
         file << "\t`BASE H\n";
         redaka++;
      }
      else if (niz_int) {
         file << "\t`BASE D\n";
         file << "\tSTOREB R0, (R1+" + to_string((tipovi_vec.size()-1)*4) + ")\n";
         file << "\t`BASE H\n";
         redaka++;
      }
   }
}

void inicijalizator(int &br_elem, string &tipovi, string &tip) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<izraz_pridruzivanja>") {
      string tip_prid;
      bool l_izraz;
      izraz_pridruzivanja(tip_prid, l_izraz);
      if (tip_prid == "niz_const_char") {
         br_elem = (ulaz.vrijednost.length() - 2) + 1;
         string s = "char";
         for (int i = 0; i < br_elem-1; i++) {
            s += "-char";
         }
         tipovi = s;
      }
      else
         tip = tip_prid;
   }
   else if (ulaz.klasa == "L_VIT_ZAGRADA") {
      iduca_linija();
      lista_izraza_pridruzivanja(br_elem, tipovi);
      iduca_linija();
      if (ulaz.klasa != "D_VIT_ZAGRADA")
         greska(dubina);
   }
   else
      greska(dubina);
}

void izravni_deklarator(string ntip, string &tip, int &br_elem) {
   int dubina = ulaz.dubina;
   iduca_linija();
   int dubina_prod = ulaz.dubina;
   if (ulaz.klasa != "IDN")
      greska(dubina);
   string ime = ulaz.vrijednost;
   iduca_linija();
   if (ulaz.dubina != dubina_prod) {
      vrati_se_liniju_nazad();
      if (!(ntip != "void"))
         greska(dubina);
      if (trenutna_tablica->tablica.find(ime) != trenutna_tablica->tablica.end())
         greska(dubina);
      trenutna_tablica->tablica[ime] = make_pair("varijabla", ntip + " " + "true");
      tip = ntip;
   }
   else {
      if (ulaz.klasa == "L_UGL_ZAGRADA") {
         file << "\tMOVE G_" + to_upper(ime) + ", R1\n";
         redaka++;
         
         iduca_linija();
         if (ulaz.klasa != "BROJ")
            greska(dubina);
         int broj = stoi(ulaz.vrijednost);
         if (!(ntip != "void"))
            greska(dubina);
         if (trenutna_tablica->tablica.find(ime) != trenutna_tablica->tablica.end())
            greska(dubina);
         if (!(broj > 0 && broj <= 1024))
            greska(dubina);
         trenutna_tablica->tablica[ime] = make_pair("varijabla", "niz_" + ntip + " " + "true");
         iduca_linija();
         if (ulaz.klasa != "D_UGL_ZAGRADA")
            greska(dubina);
         tip = "niz_" + ntip;
         br_elem = broj;
         if (ntip == "int") {
            int_nizovi.push_back(ime);
            int_nizovi.push_back(to_string(broj));
            niz_int = true;
         }
         else if (ntip == "char") {
            char_nizovi.push_back(ime);
            char_nizovi.push_back(to_string(broj));
            niz_char = true;
         }
      }
      else if (ulaz.klasa == "L_ZAGRADA") {
         iduca_linija();
         if (ulaz.klasa == "KR_VOID") {
            if (trenutna_tablica->tablica.find(ime) != trenutna_tablica->tablica.end() && trenutna_tablica->tablica[ime].first != "funkcija") {
               if (trenutna_tablica->tablica[ime].second != "void " + ntip)
                  greska(dubina);
            }
            else
               trenutna_tablica->tablica[ime] = make_pair("funkcija", "void " + ntip);
            iduca_linija();
            if (ulaz.klasa != "D_ZAGRADA")
               greska(dubina);
            tip = "funkcija";
            deklarirane_funkcije[ime].insert("void " + ntip);
         }
         else if (ulaz.izraz == "<lista_parametara>") {
            string tipovi;
            string imena;
            lista_parametara(tipovi, imena);
            if (trenutna_tablica->tablica.find(ime) != trenutna_tablica->tablica.end() && trenutna_tablica->tablica[ime].first != "funkcija") {
                  greska(dubina);
            }
            else
               trenutna_tablica->tablica[ime] = make_pair("funkcija", tipovi + " " + ntip);
            iduca_linija();
            if (ulaz.klasa != "D_ZAGRADA")
               greska(dubina);
            tip = "funkcija";
            deklarirane_funkcije[ime].insert(tipovi + " " + ntip);
         }
      }
   }
}

void init_deklarator(string ntip) {
   int dubina = ulaz.dubina;
   iduca_linija();
   int dubina_prod = ulaz.dubina;
   string tip_dekl;
   int br_elem_dekl;
   izravni_deklarator(ntip, tip_dekl, br_elem_dekl);
   string ime = ulaz.vrijednost;
   trenutna_tablica->varijable.push_back(ime);
   if (trenutna_tablica->ugnijezdjeni_blok == nullptr && tip_dekl.find("niz") == string::npos)
      globalne_varijable.push_back(ulaz.vrijednost);

   iduca_linija();
   if (ulaz.dubina != dubina_prod) {
      vrati_se_liniju_nazad();
      if (tip_dekl.find("const") != string::npos)
         greska(dubina);

      if (tip_dekl.find("niz") == string::npos) {
         file << "\tMOVE %D 0, R0\n";
         file << "\tPUSH R0\n";
         redaka += 2;
         trenutna_tablica->na_stogu++;
      }
   }
   else {
      if (ulaz.klasa != "OP_PRIDRUZI") 
         greska(dubina);
      iduca_linija();
      int br_elem_ini;
      string tipovi, tip_ini;

      inicijalizator(br_elem_ini, tipovi, tip_ini);
      if (tip_dekl.find("niz") != string::npos) {
         //vector<string> tipovi_vec = split(split(tipovi, " ")[0], "-");
         vector<string> tipovi_vec = split(tipovi, "-");
         if (!(br_elem_ini <= br_elem_dekl))
            greska(dubina);
         for (string s : tipovi_vec) {
            if (s.find("int") != string::npos)
               moze_li_se_pretvoriti_u_vrijednost(s, "int") ? (void)0 : greska(dubina);
            else if (s.find("char") != string::npos)
               moze_li_se_pretvoriti_u_vrijednost(s, "char") ? (void)0 : greska(dubina);
         }
      }
      else if (tip_dekl.find("int") != string::npos) {
         moze_li_se_pretvoriti_u_vrijednost(tip_ini, "int") ? (void)0 : greska(dubina);
      }
      else if (tip_dekl.find("char") != string::npos)
         moze_li_se_pretvoriti_u_vrijednost(tip_ini, "char") ? (void)0 : greska(dubina);
      else
         greska(dubina);

      niz_char = false;
      niz_int = false;
      // if (ulaz.klasa == "IDN") {
      //    for (int i = 0; i < trenutna_tablica->varijable.size(); i++) {
      //       if (ulaz.vrijednost == trenutna_tablica->varijable[i]) {
      //          file << "\tLOAD R0, (R7+" + to_string((trenutna_tablica->na_stogu-1-i)*4) + ")\n";
      //          file << "\tPUSH R0\n";
      //          trenutna_tablica->na_stogu++;
      //       }
      //    }
         // for (int i = 0; i < trenutna_tablica->varijable.size(); i++) {
         //    if (ime == trenutna_tablica->varijable[i]) {
         //       file << "\tSTORE R0, (R7+" + to_string((trenutna_tablica->na_stogu-1-i)*4) + ")\n";
         //    }
         // }
      // }
   }
}

void lista_init_deklaratora(string ntip) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<init_deklarator>")
      init_deklarator(ntip);
   else if (ulaz.izraz == "<lista_init_deklaratora>") {
      lista_init_deklaratora(ntip);
      iduca_linija();
      if (ulaz.klasa != "ZAREZ")
         greska(dubina);
      iduca_linija();
      init_deklarator(ntip);
   }
}

void deklaracija() {
   int dubina = ulaz.dubina;
   iduca_linija();
   string tip;
   ime_tipa(tip); 
   iduca_linija();
   lista_init_deklaratora(tip);
   iduca_linija();
   if (ulaz.klasa != "TOCKAZAREZ")  
      greska(dubina);
}

void lista_deklaracija() {
   iduca_linija();
   if (ulaz.izraz == "<deklaracija>")
      deklaracija();
   else if (ulaz.izraz == "<lista_deklaracija>") {
      lista_deklaracija();
      iduca_linija();
      deklaracija();
   }
}

void deklaracija_parametra(string &tip, string &ime) {
   int dubina = ulaz.dubina;
   iduca_linija();
   int dubina_prod = ulaz.dubina;
   ime_tipa(tip);
   if (tip == "void")
      greska(dubina);
   iduca_linija();
   if (ulaz.klasa != "IDN")
      greska(dubina);
   string idn_ime = ulaz.vrijednost;
   iduca_linija();
   if (ulaz.dubina != dubina_prod) {
      vrati_se_liniju_nazad();
      ime = idn_ime;
   }
   else {
      iduca_linija();
      if (ulaz.klasa != "D_UGL_ZAGRADA") 
         greska(dubina);
      tip = "niz_" + tip;
      ime = idn_ime; 
   }
}

void lista_parametara(string &tipovi, string &imena) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<deklaracija_parametra>") {
      string tip;
      string ime;
      deklaracija_parametra(tip, ime);
      tipovi = tip;
      imena = ime;
   }
   else if (ulaz.izraz == "<lista_parametara>") {
      lista_parametara(tipovi, imena);
      iduca_linija();
      if (ulaz.klasa != "ZAREZ")
         greska(dubina);
      iduca_linija();
      string tip;
      string ime;
      deklaracija_parametra(tip, ime);
      if (imena.find(ime) != string::npos)
         greska(dubina);
      tipovi = tipovi + "-" + tip;
      imena = imena + "-" + ime;
   }
}

void definicija_funkcije() {
   int dubina = ulaz.dubina;
   trenutna_tablica->na_stogu = 0;
   iduca_linija();
   string tip;
   ime_tipa(tip);
   if (!(tip != "const_int" || tip != "const_char"))
      greska(dubina);
   iduca_linija();
   if (ulaz.klasa != "IDN")
      greska(dubina);
   if (!pozvo_main) {
      file << "\tCALL F_MAIN\n";
      file << "\tHALT\n";
      redaka += 2;
      pozvo_main = true;
   }
   file << "F_" + to_upper(ulaz.vrijednost) + "\n";
   tablica_znakova* iduca_tablica = trenutna_tablica;
   string ime = ulaz.vrijednost;
   if (definirane_funkcije.find(ime) != definirane_funkcije.end())
      greska(dubina);
   iduca_linija();
   if (ulaz.klasa != "L_ZAGRADA")
      greska(dubina);
   iduca_linija();
   if (ulaz.klasa == "KR_VOID") {
      //jel postoji deklarirana u globalnom - sve iduca_tablica varijable mogu sam zamijenit s trenutna_tablica jer se nalazim u globalnom djelokrugu
      if (iduca_tablica->tablica.find(ime) != iduca_tablica->tablica.end() && iduca_tablica->tablica[ime].first == "funkcija") {
         if (iduca_tablica->tablica[ime].second != "void " + tip)
            greska(dubina);
      }  
      iduca_linija();
      if (ulaz.klasa != "D_ZAGRADA")   
         greska(dubina);
      trenutna_tablica->tablica[ime] = make_pair("funkcija", "void " + tip);
      stvori_novi_djelokrug();
      deklarirane_funkcije[ime].insert("void " + tip);
      definirane_funkcije[ime] = "void " + tip;
      trenutna_tablica->return_vrijednost = tip;
      trenutna_tablica->parametri = "void";
   }
   else if (ulaz.izraz == "<lista_parametara>") {
      string tipovi;
      string imena;
      lista_parametara(tipovi, imena);
      //jel postoji u globalnom
      if (iduca_tablica->tablica.find(ime) != iduca_tablica->tablica.end() && iduca_tablica->tablica[ime].first == "funkcija") {
         if (iduca_tablica->tablica[ime].second != tipovi + "_" + tip)
            greska(dubina);
      }
      iduca_linija();
      if (ulaz.klasa != "D_ZAGRADA")   
      greska(dubina);
      trenutna_tablica->tablica[ime] = make_pair("funkcija",  tipovi + " " + tip);
      stvori_novi_djelokrug();
      definirane_funkcije[ime] = tipovi + " " + tip;
      deklarirane_funkcije[ime].insert(tipovi + " " + tip);
      trenutna_tablica->return_vrijednost = tip;
      trenutna_tablica->parametri = tipovi;
      
      vector<string> tipovi_vec = split(tipovi, "-");
      vector<string> imena_vec = split(imena, "-");
      trenutna_tablica->parametri_funkcije = imena_vec;
      for (size_t i = 0; i < imena_vec.size(); i++) {
         if (tipovi_vec[i].find("const") != string::npos) 
            trenutna_tablica->tablica[imena_vec[i]] = make_pair("varijabla", tipovi_vec[i] + " false");
         else
            trenutna_tablica->tablica[imena_vec[i]] = make_pair("varijabla", tipovi_vec[i] + " true");
      }
   }
   iduca_linija();
   slozena_naredba();
   if (!trenutna_tablica->returno) {
      file << "\tRET\n";
      redaka++;
   }
}

void vanjska_deklaracija() {
   iduca_linija();
   if (ulaz.izraz == "<definicija_funkcije>") {
      definicija_funkcije();
   }
   else if (ulaz.izraz == "<deklaracija>") {
      deklaracija();
   }
}

void prijevodna_jedinica() {
   iduca_linija();
   if (ulaz.izraz == "<vanjska_deklaracija>")
      vanjska_deklaracija();
   else if (ulaz.izraz == "<prijevodna_jedinica>") {
      prijevodna_jedinica();
      iduca_linija();
      vanjska_deklaracija();
   }
}

void naredba_skoka() {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.klasa == "KR_CONTINUE" || ulaz.klasa == "KR_BREAK") {
      //if (u_petlji < 1)
      if (!(trenutna_tablica->u_petlji || (!trenutna_tablica->u_petlji && pozvala_petlja))) {
         greska(dubina);
      }
      pozvala_petlja = false;
      iduca_linija();
      if (ulaz.klasa != "TOCKAZAREZ")
         greska(dubina);
   }
   else if (ulaz.klasa == "KR_RETURN") {
      iduca_linija();
      tablica_znakova* tb_zn = trenutna_tablica;
      while (tb_zn->ugnijezdjeni_blok != nullptr)
         tb_zn = tb_zn->ugnijezdjeni_blok;
      tb_zn->returno = true;
      if (ulaz.klasa == "TOCKAZAREZ") {
         if (!(trenutna_tablica->return_vrijednost == "void"))
            greska(dubina);
         // file << "\tADD R7, " + to_string(trenutna_tablica->na_stogu*4) + ", R7\n";
         // trenutna_tablica->na_stogu -= 
      }
      else if (ulaz.izraz == "<izraz>") {
         string tip;
         bool l_izraz;
         izraz(tip, l_izraz);
         if (!(trenutna_tablica->return_vrijednost != "void"
               && moze_li_se_pretvoriti_u_vrijednost(tip, trenutna_tablica->return_vrijednost)))
                  greska(dubina);
         iduca_linija();
         if (ulaz.klasa != "TOCKAZAREZ")
            greska(dubina);

         file << "\tPOP R6\n";
         trenutna_tablica->na_stogu--;
         tablica_znakova* tb_zn = trenutna_tablica;
         // while (tb_zn->ugnijezdjeni_blok != nullptr) {
         //    if (tb_zn->na_stogu > 0) {
         //       file << "\t`BASE D\n";
         //       file << "\tADD R7, " + to_string((tb_zn->na_stogu)*4) + ", R7\n";
         //       file << "\t`BASE H\n";
         //    }
         //    tb_zn = tb_zn->ugnijezdjeni_blok;
         // }
         int ukupno_na_stogu = 0;
         while (tb_zn->ugnijezdjeni_blok != nullptr) {
            ukupno_na_stogu += tb_zn->na_stogu;
            tb_zn = tb_zn->ugnijezdjeni_blok;
         }
         ukupno_na_stogu += tb_zn->na_stogu;
         if (ukupno_na_stogu > 0) {
            file << "\t`BASE D\n";
            file << "\tADD R7, " + to_string((ukupno_na_stogu)*4) + ", R7\n";
            file << "\t`BASE H\n";
            redaka += 1;
         }
         file << "\tRET\n";
         redaka += 1;
      }
      else
         greska(dubina);
   }
}

void naredba_petlje() {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.klasa == "KR_WHILE") {
      iduca_linija();
      if (ulaz.klasa != "L_ZAGRADA")
         greska(dubina);
      iduca_linija();
      string tip;
      bool l_izraz;
      izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "D_ZAGRADA")
         greska(dubina);
      iduca_linija();
      pozvala_petlja = true;
      naredba();
   }
   else if (ulaz.klasa == "KR_FOR") {
      iduca_linija();
      if (ulaz.klasa != "L_ZAGRADA")
         greska(dubina);
      iduca_linija();
      string tip1;
      izraz_naredba(tip1);
      iduca_linija();
      string tip2;
      izraz_naredba(tip2);
      moze_li_se_pretvoriti_u_int(tip2) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa == "D_ZAGRADA") {
         iduca_linija();
         pozvala_petlja = true;
         naredba();
      }
      else if (ulaz.izraz == "<izraz>") {
         string tip;
         bool l_izraz;
         izraz(tip, l_izraz);
         iduca_linija();
         if (ulaz.klasa != "D_ZAGRADA")
            greska(dubina);
         iduca_linija();
         pozvala_petlja = true;
         naredba();
      }
   }
}

void naredba_grananja() {
   int dubina = ulaz.dubina;
   iduca_linija();
   int dubina_prod = ulaz.dubina;
   if (ulaz.klasa == "KR_IF") {
      unutar_ifa = true;
      int stari_retci = redaka;
      iduca_linija();
      if (ulaz.klasa != "L_ZAGRADA") 
         greska(dubina);
      iduca_linija();
      string tip;
      bool l_izraz;
      izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "D_ZAGRADA")
         greska(dubina);
      
      if (redaka - stari_retci == 2) {
         file << "\tPOP R0\n";
         trenutna_tablica->na_stogu--;
         file << "\tCMP R0, 0\n";
         file << "\tJR_Z ELSE\n";
         redaka += 3;
      }

      iduca_linija();
      naredba();
      iduca_linija();
      if (ulaz.dubina != dubina_prod) {
         vrati_se_liniju_nazad();
         file << "ELSE\n";
      }
      else {
         file << "\tJR NASTAVAK\n";
         redaka += 1;
         file << "ELSE\n";
         if (ulaz.klasa != "KR_ELSE")
            greska(dubina);
         iduca_linija();
         naredba();
         file << "NASTAVAK\n";
         unutar_ifa = false;
      }
   }
}

void izraz_naredba(string &tip) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.klasa == "TOCKAZAREZ")
      tip = "int";
   else if (ulaz.izraz == "<izraz>") {
      bool l_izraz;
      izraz(tip, l_izraz);
      iduca_linija();
      if (ulaz.klasa != "TOCKAZAREZ")
         greska(dubina);
   }
}

void naredba() {
   iduca_linija();
   if (ulaz.izraz == "<slozena_naredba>") {
      stvori_novi_djelokrug();
      slozena_naredba();
   }
   else if (ulaz.izraz == "<izraz_naredba>") {
      string tip;
      izraz_naredba(tip);
   }
   else if (ulaz.izraz == "<naredba_grananja>")
      naredba_grananja();
   else if (ulaz.izraz == "<naredba_petlje>") 
      naredba_petlje();
   else if (ulaz.izraz == "<naredba_skoka>")
      naredba_skoka();
}

void lista_naredbi() {
   iduca_linija();
   if (ulaz.izraz == "<naredba>") {
      naredba();
   }
   else if (ulaz.izraz == "<lista_naredbi>") {
      lista_naredbi();
      iduca_linija();
      naredba();
   }
}

void slozena_naredba() {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.klasa == "L_VIT_ZAGRADA") {
      iduca_linija();
      if (ulaz.izraz == "<lista_naredbi>") {
         lista_naredbi();
         iduca_linija();
         if (ulaz.klasa != "D_VIT_ZAGRADA")
            greska(dubina);
      }
      else if (ulaz.izraz == "<lista_deklaracija>") {
         lista_deklaracija();
         iduca_linija();
         lista_naredbi();
         iduca_linija();
         if (ulaz.klasa != "D_VIT_ZAGRADA")
            greska(dubina);
      }
      if (!trenutna_tablica->returno && trenutna_tablica->na_stogu > 0) {
         file << "\t`BASE D\n";
         file << "\tADD R7, " + to_string((trenutna_tablica->na_stogu)*4) + ", R7\n";
         file << "\t`BASE H\n";
         redaka += 1;
      }
      unisti_trenutni_djelokrug();
   }
}

void izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<izraz_pridruzivanja>") {
      izraz_pridruzivanja(tip, l_izraz);
   }
   else if (ulaz.izraz == "<izraz>") {
      izraz(tip, l_izraz);
      iduca_linija();
      if (ulaz.klasa != "ZAREZ")
         greska(dubina);
      iduca_linija();
      izraz_pridruzivanja(tip, l_izraz);
      l_izraz = false;
   }
}

void izraz_pridruzivanja(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<log_ili_izraz>") {
      log_ili_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<postfiks_izraz>") {
      string tip_post;
      bool l_izraz_post;
      postfiks_izraz(tip_post, l_izraz_post);
      string ime = ulaz.vrijednost;
      string tip_izraz;
      if (!l_izraz_post)
         greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_PRIDRUZI")
         greska(dubina);
      iduca_linija();

      file << "\tPOP R0\n";
      redaka += 1;
      trenutna_tablica->na_stogu--;

      izraz_pridruzivanja(tip_izraz, l_izraz);
      moze_li_se_pretvoriti_jedan_u_drugi(tip_izraz, tip_post) ? (void)0 : greska(dubina);
      tip = tip_post;
      l_izraz = false;
      
      if (ulaz.klasa == "BROJ") {
         if (!(niz_int || niz_char)) {
            file << "\tPOP R0\n";
            redaka += 1;
            trenutna_tablica->na_stogu--;
            for (size_t i = 0; i < trenutna_tablica->varijable.size(); i++) {
               if (ime == trenutna_tablica->varijable[i]) {
                  file << "\t`BASE D\n";
                  file << "\tSTORE R0, (R7+" + to_string((trenutna_tablica->na_stogu-1-i)*4) + ")\n";
                  file << "\t`BASE H\n";
                  redaka += 1;
               }
            }
         }
         else {
            file << "\tPOP R0\n";
            redaka += 1;
            trenutna_tablica->na_stogu--;
            if (niz_int)
               file << "\tSTORE R0, (R1)\n";
            else if (niz_char)
               file << "\tSTOREB R0, (R1)\n";
            redaka++;
         }
      }
      else if (ulaz.klasa == "IDN") {
         for (size_t i = 0; i < trenutna_tablica->varijable.size(); i++) {
            if (ulaz.vrijednost == trenutna_tablica->varijable[i]) {
               file << "\t`BASE D\n";
               file << "\tLOAD R0, (R7+" + to_string((trenutna_tablica->na_stogu-1-i)*4) + ")\n";
               file << "\t`BASE H\n";
               redaka += 1;
            }
         }
         for (size_t i = 0; i < trenutna_tablica->varijable.size(); i++) {
            if (ime == trenutna_tablica->varijable[i]) {
               file << "\t`BASE D\n";
               file << "\tSTORE R0, (R7+" + to_string((trenutna_tablica->na_stogu-1-i)*4) + ")\n";
               file << "\t`BASE H\n";
               redaka += 1;
            }
         }
      }
   }
}

void log_ili_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<log_i_izraz>") {
      log_i_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<log_ili_izraz>") {
      log_ili_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_ILI")
         greska(dubina);
      iduca_linija();
      log_i_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
   }
}

void log_i_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<bin_ili_izraz>") {
      bin_ili_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<log_i_izraz>") {
      log_i_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_I")
         greska(dubina);
      iduca_linija();
      bin_ili_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
   }
}

void bin_ili_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<bin_xili_izraz>") {
      bin_xili_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<bin_ili_izraz>") {
      bin_ili_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_BIN_ILI")
         greska(dubina);
      iduca_linija();
      bin_xili_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      tablica_znakova* tb_zn = trenutna_tablica;
      while (tb_zn->ugnijezdjeni_blok != nullptr)
         tb_zn = tb_zn->ugnijezdjeni_blok;
      if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
         file << "\tPOP R1\n";
         file << "\tPOP R0\n";
         trenutna_tablica->na_stogu-=2;
         file << "\tOR R0, R1, R0\n";
         file << "\tPUSH R0\n";
         redaka += 4;
         trenutna_tablica->na_stogu++;
      }
   }
}

void bin_xili_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<bin_i_izraz>") {
      bin_i_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<bin_xili_izraz>") {
      bin_xili_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_BIN_XILI")
         greska(dubina);
      iduca_linija();
      bin_i_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      tablica_znakova* tb_zn = trenutna_tablica;
      while (tb_zn->ugnijezdjeni_blok != nullptr)
         tb_zn = tb_zn->ugnijezdjeni_blok;
      if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
         file << "\tPOP R1\n";
         file << "\tPOP R0\n";
         trenutna_tablica->na_stogu-=2;
         file << "\tXOR R0, R1, R0\n";
         file << "\tPUSH R0\n";
         redaka += 4;
         trenutna_tablica->na_stogu++;
      }
   }
}

void bin_i_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<jednakosni_izraz>") {
      jednakosni_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<bin_i_izraz>") {
      bin_i_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_BIN_I")
         greska(dubina);
      iduca_linija();
      jednakosni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      tablica_znakova* tb_zn = trenutna_tablica;
      while (tb_zn->ugnijezdjeni_blok != nullptr)
         tb_zn = tb_zn->ugnijezdjeni_blok;
      if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
         file << "\tPOP R1\n";
         file << "\tPOP R0\n";
         trenutna_tablica->na_stogu-=2;
         file << "\tAND R0, R1, R0\n";
         file << "\tPUSH R0\n";
         redaka += 4;
         trenutna_tablica->na_stogu++;
      }
   }
}

void jednakosni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<odnosni_izraz>") {
      odnosni_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<jednakosni_izraz>") {
      jednakosni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_EQ" && ulaz.klasa != "OP_NEQ")
         greska(dubina);
      string znak_operatora = ulaz.klasa;
      iduca_linija();
      odnosni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);

      if (unutar_ifa) {
         file << "\tPOP R1\n";
         file << "\tPOP R0\n";
         trenutna_tablica->na_stogu -= 2;
         file << "\tCMP R0, R1\n";
         if (znak_operatora == "OP_EQ")
            file << "\tJR_NE ELSE\n";
         else if (znak_operatora == "OP_NEQ")
            file << "\tJR_EQ ELSE\n";
         redaka += 4;
      }
   }
}

void odnosni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<aditivni_izraz>") {
      aditivni_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<odnosni_izraz>") {
      odnosni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_LT" && ulaz.klasa != "OP_GT" && ulaz.klasa != "OP_LTE" && ulaz.klasa != "OP_GTE")
         greska(dubina);
      string znak_operatora = ulaz.klasa;
      iduca_linija();
      aditivni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);

      if (unutar_ifa) {
         file << "\tPOP R1\n";
         file << "\tPOP R0\n";
         trenutna_tablica->na_stogu -= 2;
         file << "\tCMP R0, R1\n";
         if (znak_operatora == "OP_LT")
            file << "\tJR_UGE ELSE\n";
         else if (znak_operatora == "OP_GT")
            file << "\tJR_ULE ELSE\n";
         else if (znak_operatora == "OP_LTE")
            file << "\tJR_UGT ELSE\n";
         else if (znak_operatora == "OP_GTE")
            file << "\tJR_ULT ELSE\n";
         redaka += 4;
      }
   }
}

void aditivni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<multiplikativni_izraz>") {
      multiplikativni_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<aditivni_izraz>") {
      aditivni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);

      // tablica_znakova* tb_zn = trenutna_tablica;
      // while (tb_zn->ugnijezdjeni_blok != nullptr)
      //    tb_zn = tb_zn->ugnijezdjeni_blok;
      // if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
      //    if (ulaz.klasa == "BROJ") {
         
      // }

      iduca_linija();
      if (ulaz.klasa != "PLUS" && ulaz.klasa != "MINUS") 
         greska(dubina);
      string znak = ulaz.klasa;
      iduca_linija();
      multiplikativni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      tip = "int";
      l_izraz = 0;

      tablica_znakova* tb_zn = trenutna_tablica;
      while (tb_zn->ugnijezdjeni_blok != nullptr)
         tb_zn = tb_zn->ugnijezdjeni_blok;
      if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
         
      }
      file << "\tPOP R1\n";
      file << "\tPOP R0\n";
      redaka += 2;
      trenutna_tablica->na_stogu-=2;
      if (znak == "PLUS")
         file << "\tADD R0, R1, R0\n";
      else if (znak == "MINUS")
         file << "\tSUB R0, R1, R0\n";
      file << "\tPUSH R0\n";
      redaka += 2;
      trenutna_tablica->na_stogu++;
   }
}

void multiplikativni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<cast_izraz>") {
      cast_izraz(tip, l_izraz);
   }
   else if (ulaz.izraz == "<multiplikativni_izraz") {
      multiplikativni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_PUTA" && ulaz.klasa != "OP_DIJELI" && ulaz.klasa != "OP_MOD")
         greska(dubina);
      cast_izraz(tip, l_izraz);
      if (moze_li_se_pretvoriti_u_int(tip)) {
         tip = "int";
         l_izraz = 0;
      }
      else
         greska(dubina);
   }
}

void specifikator_tipa(string &tip) {
   iduca_linija();
   if (ulaz.klasa == "KR_VOID")
      tip = "void";
   else if (ulaz.klasa == "KR_CHAR")
      tip = "char";
   else if (ulaz.klasa == "KR_INT")
      tip = "int";
}

void ime_tipa(string &tip) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<specifikator_tipa>") {
      specifikator_tipa(tip);
   }
   else if (ulaz.klasa == "KR_CONST") {
      iduca_linija();
      specifikator_tipa(tip);
      if (tip != "void") {
         tip = "const_" + tip;
      }
      else 
         greska(dubina);
   }
}

void cast_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<unarni_izraz>") {
      unarni_izraz(tip, l_izraz);
   }
   else if (ulaz.klasa == "L_ZAGRADA") {
      iduca_linija();
      ime_tipa(tip);
      string ime_tip = tip;
      iduca_linija();
      if (ulaz.klasa != "D_ZAGRADA")
         greska(dubina);
      iduca_linija();
      cast_izraz(tip, l_izraz);
      if ((tip == "int" || tip == "const_int" || tip == "char" || tip == "const_char")
         && (ime_tip == "int" || ime_tip == "const_int" || ime_tip == "char" || ime_tip == "const_char")) {
            tip = ime_tip;
            l_izraz = 0;
         }
      else
         greska(dubina);
   }
}

void unarni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<postfiks_izraz>") {
      postfiks_izraz(tip, l_izraz);
   }
   else if (ulaz.klasa == "OP_INC" || ulaz.klasa == "OP_DEC") {
      iduca_linija();
      unarni_izraz(tip, l_izraz);
      tablica_znakova* tb_zn = trenutna_tablica;
      int ukupno_na_stogu = 0;
      while (tb_zn->ugnijezdjeni_blok != nullptr) {
         ukupno_na_stogu += tb_zn->na_stogu;
         if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
            for (size_t i = 0; i < tb_zn->varijable.size(); i++) {
               if (ulaz.vrijednost == tb_zn->varijable[i]) {
                  file << "\t`BASE D\n";
                  file << "\tPOP R0\n";
                  file << "\tADD R0, 1, R0\n";
                  file << "\tSTORE R0, ((R7+" + to_string((ukupno_na_stogu-1-i)*4) + ")\n";
                  file << "\tPUSH R0\n";
                  file << "\t`BASE H\n";
                  redaka += 4;
                  trenutna_tablica->na_stogu++;
               }
            }
         }
         tb_zn = tb_zn->ugnijezdjeni_blok;
      }
      if (l_izraz == 1 && ((moze_li_se_pretvoriti_u_int(tip)))) {
         tip = "int";
         l_izraz = false;
      }
      else
         greska(dubina);
   }
   else if (ulaz.izraz == "<unarni_operator>") {
      iduca_linija();
      if (ulaz.klasa == "MINUS")
         stavi_minus = true;
      iduca_linija();
      cast_izraz(tip, l_izraz);
      if ((moze_li_se_pretvoriti_u_int(tip))) {
         tip = "int";
         l_izraz = false;
      }
      else
         greska(dubina);
   }
}

void lista_argumenata(string &tipovi) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<izraz_pridruzivanja>") {
      string tip;
      bool l_izraz;
      izraz_pridruzivanja(tip, l_izraz);
      tipovi = tip;
      if (ulaz.klasa == "IDN") {
         if (find(int_nizovi.begin(), int_nizovi.end(), ulaz.vrijednost) != int_nizovi.end()
               || find(char_nizovi.begin(), char_nizovi.end(), ulaz.vrijednost) != char_nizovi.end()) {
               file << "\tMOVE G_" + to_upper(ulaz.vrijednost) + ", R1\n";
               file << "\tPUSH R1\n";
               trenutna_tablica->na_stogu++;
            }
      }
   }
   else if (ulaz.izraz == "<lista_argumenata>") {
      lista_argumenata(tipovi);
      iduca_linija();
      if (ulaz.klasa != "ZAREZ")
         greska(dubina);
      iduca_linija();
      string tip;
      bool l_izraz;
      izraz_pridruzivanja(tip, l_izraz);
      tipovi = tipovi + "-" + tip;
      if (ulaz.klasa == "IDN") {
         if (find(int_nizovi.begin(), int_nizovi.end(), ulaz.vrijednost) != int_nizovi.end()
               || find(char_nizovi.begin(), char_nizovi.end(), ulaz.vrijednost) != char_nizovi.end()) {
               file << "\tMOVE G_" + to_upper(ulaz.vrijednost) + ", R1\n";
               file << "\tPUSH R1\n";
               trenutna_tablica->na_stogu++;
            }
      }
   }
}

void postfiks_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<primarni_izraz>") {
      string tip_prim;
      bool l_izraz_prim;
      primarni_izraz(tip_prim, l_izraz_prim);
      tip = tip_prim;
      l_izraz = l_izraz_prim;
   }
   else if (ulaz.izraz == "<postfiks_izraz>") {
      string tip_post;
      bool l_izraz_post;
      postfiks_izraz(tip_post, l_izraz_post);
      string ime = ulaz.vrijednost;
      iduca_linija();
      string redak;
      if (ulaz.klasa == "L_UGL_ZAGRADA") {
         iduca_linija();
         if (tip_post.find("niz") == string::npos)
            greska(dubina);
         if (tip_post == "niz_int")
            niz_int = true;
         else if (tip_post == "niz_char")
            niz_char = true;
         string tip_izr;
         bool l_izraz_izr;
         izraz(tip_izr, l_izraz_izr);
         moze_li_se_pretvoriti_u_int(tip_izr) ? (void)0 : greska(dubina);
         l_izraz = tip.find("const") != string::npos ? 0 : 1;
         tip = tip_post.substr(tip_post.find("niz")+4);

         file << "\tPOP R0\n";
         trenutna_tablica->na_stogu--;
         redaka++;
         file << "\tMOVE G_" + to_upper(ime) + ", R1\n";
         if (tip == "char") {
            file << "\t`BASE D\n";
            file << "\tADD R1, " + ulaz.vrijednost + ", R1\n";
            file << "\t`BASE H\n";
         }
         else if (tip == "int") {
            file << "\t`BASE D\n";
            file << "\tADD R1, " + to_string(stoi(ulaz.vrijednost)*4) + ", R1\n";
            file << "\t`BASE H\n";
         }
         if (tip == "int")
            file << "\tLOAD R0, (R1)\n";
         else if (tip == "char")
            file << "\tLOADB R0, (R1)\n";
         file << "\tPUSH R0\n";
         trenutna_tablica->na_stogu++;
         redaka += 4;

         iduca_linija();
         if (ulaz.klasa != "D_UGL_ZAGRADA")
            greska(dubina);
      }
      else if (ulaz.klasa == "L_ZAGRADA") {
         iduca_linija();
         if (ulaz.klasa == "D_ZAGRADA") {
            if (tip_post.find("funkcija") != string::npos) {   
               vector<string> fja_arg_pov = split(tip_post, " ");
               if (fja_arg_pov[1] != "void")
                  greska(dubina);
               tip = fja_arg_pov[2];
               l_izraz = 0;

               file << "\tCALL F_" + to_upper(ime) + "\n";
               file << "\tPUSH R6\n";
               redaka += 2;
               trenutna_tablica->na_stogu++;
            }
            else
               greska(dubina);
         }
         else if (ulaz.izraz == "<lista_argumenata>") {
            string tipovi;
            lista_argumenata(tipovi);
            iduca_linija();
            if (tip_post.find("funkcija") != string::npos) {
               vector<string> fja_arg_pov = split(tip_post, " ");
               if (fja_arg_pov[1] == "void")
                  greska(dubina);
               vector<string> tipovi_vec = split(tipovi, "-");
               vector<string> parametri_vec = split(fja_arg_pov[1], "-");
               if (tipovi_vec.size() != parametri_vec.size())
                  greska(dubina);
               for (size_t i = 0; i < tipovi_vec.size(); i++) {
                  moze_li_se_pretvoriti_u_vrijednost(tipovi_vec[i], parametri_vec[i]) ? (void)0 : greska(dubina);
               }
               tip = fja_arg_pov[2];
               l_izraz = false;
            }
            file << "\tCALL F_" + to_upper(ime) + "\n";
            redaka += 1;
            string tipovi_pov = definirane_funkcije[ime];
            vector<string> tipovi_vec = split(split(tipovi_pov, " ")[0], "-");
            if (trenutna_tablica->na_stogu > 0) {
               file << "\t`BASE D\n";
               file << "\tADD R7, " + to_string((tipovi_vec.size())*4) + ", R7\n";
               file << "\t`BASE H\n";
               redaka += 1;
            }
            trenutna_tablica->na_stogu -= tipovi_vec.size();
            file << "\tPUSH R6\n";
            redaka += 1;
            trenutna_tablica->na_stogu++;
         }
         else 
            greska(dubina);
      }
      else if (ulaz.klasa == "OP_INC" || ulaz.klasa == "OP_DEC") {
         tablica_znakova* tb_zn = trenutna_tablica;
         int ukupno_na_stogu = 0;
         while (tb_zn->ugnijezdjeni_blok != nullptr) {
            ukupno_na_stogu += tb_zn->na_stogu;
            if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
               for (size_t i = 0; i < tb_zn->varijable.size(); i++) {
                  if (ulaz.vrijednost == tb_zn->varijable[i]) {
                     file << "\t`BASE D\n";
                     file << "\tLOAD R0, (R7+" + to_string((ukupno_na_stogu-1-i)*4) + ")\n";
                     file << "\tPUSH R0\n";
                     file << "\tADD R0, 1, R0\n";
                     file << "\tSTORE R0, ((R7+" + to_string((ukupno_na_stogu-1-i)*4) + ")\n";
                     file << "\t`BASE H\n";
                     redaka += 4;
                     trenutna_tablica->na_stogu++;
                  }
               }
            }
            tb_zn = tb_zn->ugnijezdjeni_blok;
         }
         iduca_linija();
         if ((l_izraz_post == 1 && ((moze_li_se_pretvoriti_u_int(tip_post))))) {
            tip = "int";
            l_izraz = false;
         }
         else
            greska(dubina);
      }
   }
}

void primarni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.klasa == "IDN") {
      string provjera_imena = dohvati_iz_tablice(ulaz.vrijednost);
      if (provjera_imena != "") {
         if (provjera_imena.find("funkcija") != string::npos) {
            tip = provjera_imena;
            l_izraz = "false";
         }
         else {
            vector<string> zapisi_iz_tablice = split(provjera_imena, " ");
            tip = zapisi_iz_tablice[0];
            l_izraz = zapisi_iz_tablice[1] == "true" ? true : false;

            tablica_znakova* tb_zn = trenutna_tablica;
            bool pronasao = false;
            int ukupno_na_stogu = 0;
            while (tb_zn->ugnijezdjeni_blok != nullptr) {
               ukupno_na_stogu += tb_zn->na_stogu;
               if (tb_zn->tablica.find(ulaz.vrijednost) != tb_zn->tablica.end()) {
                  for (size_t i = 0; i < trenutna_tablica->parametri_funkcije.size(); i++) {
                     if (ulaz.vrijednost == trenutna_tablica->parametri_funkcije[i]) {
                        file << "\t`BASE D\n";
                        file << "\tLOAD R0, (R7+" + to_string((trenutna_tablica->na_stogu+trenutna_tablica->parametri_funkcije.size()-i)*4) + ")\n";
                        file << "\tPUSH R0\n";
                        file << "\t`BASE H\n";
                        redaka += 2;
                        trenutna_tablica->na_stogu++;
                        pronasao = true;
                        break;
                     }
                  }
                  for (size_t i = 0; i < tb_zn->varijable.size(); i++) {
                     if (ulaz.vrijednost == tb_zn->varijable[i]) {
                        file << "\t`BASE D\n";
                        file << "\tLOAD R0, (R7+" + to_string((ukupno_na_stogu-1-i)*4) + ")\n";
                        file << "\tPUSH R0\n";
                        file << "\t`BASE H\n";
                        redaka += 2;
                        trenutna_tablica->na_stogu++;
                     }
                  }
                  pronasao = true;
                  break;
               }
               tb_zn = tb_zn->ugnijezdjeni_blok;
            }
            if (!pronasao) {
               if (!(niz_char || niz_int)) {
                  file << "\tLOAD R0, (G_" + to_upper(ulaz.vrijednost) + ")\n";
                  file << "\tPUSH R0\n";
                  redaka += 2;
                  trenutna_tablica->na_stogu++;
               }
            }
            // if (find(int_nizovi.begin(), int_nizovi.end(), ulaz.vrijednost) != int_nizovi.end()
            //    || find(char_nizovi.begin(), char_nizovi.end(), ulaz.vrijednost) != char_nizovi.end()) {
            //    file << "\tMOVE G_" + to_upper(ulaz.vrijednost) + ", R1\n";
            //    file << "\tPUSH R1\n";
            // }
         }
      }
      else
         greska(dubina);
   }
   else if (ulaz.klasa == "BROJ") {
      try {
         stoi(ulaz.vrijednost);
         tip = "int";
         l_izraz = false;
         if (trenutna_tablica->ugnijezdjeni_blok != nullptr) {
            if (!(stoi(ulaz.vrijednost) & 0xFFFF0000)) {
               if (stavi_minus) {
                  file << "\tMOVE %D -" + ulaz.vrijednost + ", R0\n";
                  stavi_minus = false;
               }
               else
                  file << "\tMOVE %D " + ulaz.vrijednost + ", R0\n";
               
               file << "\tPUSH R0\n";
               redaka += 2;
               trenutna_tablica->na_stogu++;
            }
            else {
               file << "\tLOAD R0, (G_BIG)\n";
               file << "\tPUSH R0\n";
               redaka += 2;
               trenutna_tablica->na_stogu++;
               globalne_varijable.push_back("big");
               globalne_varijable.push_back(ulaz.vrijednost);
            }
         }
         else {
            if (stavi_minus) {
               globalne_varijable.push_back("-" + ulaz.vrijednost);
               stavi_minus = false;
            }
            else
               globalne_varijable.push_back(ulaz.vrijednost);
         }
      }
      catch (...) {
         greska(dubina);
      }
   }
   else if (ulaz.klasa == "ZNAK") {
      size_t poz = ulaz.vrijednost.find("\\");
      if (poz != string::npos) {
         if (ulaz.vrijednost.find('\\') < ulaz.vrijednost.length()) {
            int poz = ulaz.vrijednost.find('\\')+1;
            if ((ulaz.vrijednost.length() == 4) && (ulaz.vrijednost[poz] == 't' || ulaz.vrijednost[poz] == 'n' || ulaz.vrijednost[poz] == 0 
               || ulaz.vrijednost[poz] == '\'' || ulaz.vrijednost[poz] == '\"' || ulaz.vrijednost[poz] == '\\')) {
                  tip = "char";
                  l_izraz = false;
               }
         }
         else
            greska(dubina);
      }
      else {
         tip = "char";
         l_izraz = false;
      }
      if (trenutna_tablica->ugnijezdjeni_blok == nullptr && !niz_char) {
         string s = to_string((int)(ulaz.vrijednost[1]));
         globalne_varijable.push_back(s + "c");
      }
      else if (niz_char) {
         file << "\tMOVE %D " + to_string((int)(ulaz.vrijednost[1])) + ", R0\n";
         redaka++;
      }
   }
   else if (ulaz.klasa == "NIZ_ZNAKOVA") {
      size_t poz = ulaz.vrijednost.find("\\");
      while(poz != string::npos) {
         poz++;
         if (!((ulaz.vrijednost[poz] == '\"' && ulaz.vrijednost[poz+1] != '\"') || ulaz.vrijednost[poz] == 't' || ulaz.vrijednost[poz] == 'n'
            || ulaz.vrijednost[poz] == '\''|| ulaz.vrijednost[poz] == 0 || ulaz.vrijednost[poz] == '\\'))
               greska(dubina);
         poz = ulaz.vrijednost.find("\\", poz);
      }
      tip = "niz_const_char";
      l_izraz = 0;
   }
   else if (ulaz.klasa == "L_ZAGRADA") {
      iduca_linija();
      izraz(tip, l_izraz);
      iduca_linija();
      string redak = ulaz.redak;
      if (ulaz.klasa != "D_ZAGRADA") {
         greska(dubina);
      }
   }
}


int main() {
   string s;
   while(getline(cin, s)) {
      ulazni_niz.push_back(s);
   }

   if (!file.is_open()) {
      cerr << "Error: Could not open a.frisc for writing." << endl;
      return 0;
   }

   file << "\tMOVE 40000, R7\n";
   redaka = 1;
   // file << "\tCALL F_MAIN\n";
   // file << "\tHALT\n";
   // redaka = 3;

   iduca_linija();
   prijevodna_jedinica();
   
   konacne_provjere();

   vector<string> varijable;
   int broj;
   for (string s : globalne_varijable) {
      if (!is_number(s))
         varijable.push_back(s);
      else {
         if (s[s.size()-1] == 'c')
            broj = stoi(s.substr(0, s.size()-1));
         else if (s[0] == '-')
            broj = stoi(s.substr(1));
         else
            broj = stoi(s);
         stringstream hexStream;
         hexStream << setw(4) << setfill('0') << uppercase << hex << broj;
         string hexBroj = hexStream.str();
         for (string s2 : varijable) {
            if (s[s.size()-1] == 'c')
               file << "G_" + to_upper(s2) + " DB " + hexBroj + "\n";
            else if (s[0] != '-')
               file << "G_" + to_upper(s2) + " DW " + hexBroj + "\n";
            else 
               file << "G_" + to_upper(s2) + " DW -" + hexBroj + "\n"; 
         }
         varijable.clear();
      }
   }
   varijable.clear();
   for (string s : int_nizovi) {
      if (!is_number(s))
         varijable.push_back(s);
      else {
         broj = stoi(s);
         broj *= 4;
         stringstream hexStream;
         hexStream << setw(4) << setfill('0') << uppercase << hex << broj;
         string hexBroj = hexStream.str();
         for (string s2: varijable) {
            file << "G_" + to_upper(s2) + " `DS " + hexBroj + "\n";
         }
      }
   }
   varijable.clear();
   for (string s : char_nizovi) {
      if (!is_number(s))
         varijable.push_back(s);
      else {
         broj = stoi(s);
         stringstream hexStream;
         hexStream << setw(4) << setfill('0') << uppercase << hex << broj;
         string hexBroj = hexStream.str();
         for (string s2: varijable) {
            file << "G_" + to_upper(s2) + " `DS " + hexBroj + "\n";
         }
      }
   }

   file.close();

   return 0;
}
