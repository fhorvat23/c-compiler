#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <set>

using namespace std;

/*
tipovi:
"int", "const int," "char", "const char", "X", "const X", "niz X", "niz const char"...
"!T", "!X" -> ne smije biti const
*/

/*
tablica znakova zapisi:
   varijable: "tip l_izraz"
   funkcije: "argumenti povratna_vrijednost" -> argumenti su odvojeni s '_' npr. int_int_char
               ako nema argumenta onda: "void povratna_vrijednost"

tablica znakova ima jednu tablicu koja mapira string -> pair(string, string)
   gdje je prva vrijednost tip varijable (varijabla ili funkcija), a druga podaci za taj tip
*/

//    NIGDJE NE DODAJEM STVARI U TABLICU ZNAKOVA NITI STVARAM NOVE ----> OBAVEZNO DODATI NA SVA MJESTA KOJA TREBA (NE ZNAM DI)
///
//       PROMIJENIT NACIN NA KOJI PROVJERAVAM KOJA JE PRODUKCIJA KAD JE JEDNA PRODUCKCIJA PREFIKS DRUGOJ
/////
///      POPRAVIT POTENCIJALNO u_petlji = false MISLIM DA NIJE NA DOBROM MJESTU
////


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
   //map<string, string> tablica;
   //map<string, string> funkcije;
   tablica_znakova* ugnijezdjeni_blok;
   string return_vrijednost, parametri;
   bool u_petlji;
   tablica_znakova() {
      //tablica;
      ugnijezdjeni_blok = nullptr;
      return_vrijednost = "";
      parametri = "";
      u_petlji = false;
   }
};

citac ulaz;
//string ulazni_niz;
vector<string> ulazni_niz;
vector<string> ulazni_niz_procitano;
bool zadnja_linija = false;
tablica_znakova* trenutna_tablica = new tablica_znakova();
//int u_petlji = 0;
bool pozvala_petlja = false;
map<string, string> definirane_funkcije;
//map<string, string> deklarirane_funkcije;
//map<string, set<string>> definirane_funkcije;
map<string, set<string>> deklarirane_funkcije;


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

// void iduca_linija() {
//    int kraj = ulazni_niz.find('\n', ulaz.glava);
//    string linija_ulaza = ulazni_niz.substr(ulaz.glava, kraj - ulaz.glava);
//    ulaz.duljina = kraj - ulaz.glava;
//    ulaz.glava = kraj+1;
//    int i = 0;
//    ulaz.dubina = 0;
//    ulazni_niz_procitano.push_back(linija_ulaza);
//    while(linija_ulaza[i] == ' ') {
//       linija_ulaza = linija_ulaza.substr(1);
//       ulaz.dubina++;
//    }
//    if (kraj+1 == ulazni_niz.length()) 
//       zadnja_linija = true;
//    ulaz.izraz = linija_ulaza;
//    if (linija_ulaza[0] == '<') {
//       ulaz.klasa = "";
//       ulaz.vrijednost = "";
//    }
//    else {
//       vector<string> linija_ulaza_vec = split(linija_ulaza, " ");
//       ulaz.klasa = linija_ulaza_vec[0];
//       ulaz.redak = linija_ulaza_vec[1];
//       ulaz.vrijednost = linija_ulaza_vec[2];
//    }
// }

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
   //int dubina_produkcije = izracunaj_dubinu(ulazni_niz_procitano[len-1]);
   //int dubina_produkcije = ulaz.dubina;
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

// void greska(string s) {
//    cout << s << endl;
//    exit(0);
// }

void vrati_se_liniju_nazad() {
   ulaz.glava--;
   ulazni_niz_procitano.erase(ulazni_niz_procitano.end());
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
   if (tip == "int" || tip == "const_int" || tip == "char")
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
   //if (find(definirane_funkcije["main"].begin(), definirane_funkcije["main"].end(), "void int") == definirane_funkcije["main"].end())
   //if (definirane_funkcije["main"].find("void int") == definirane_funkcije["main"].end())
   if (definirane_funkcije["main"] != "void int")
      greska(-1);
   for (auto it = deklarirane_funkcije.begin(); it != deklarirane_funkcije.end(); it++) {
      if (it->second.size() > 1)
         greska(-2);
      else if (definirane_funkcije.find(it->first) == definirane_funkcije.end())
         greska(-2);
      else if (definirane_funkcije[it->first] != *deklarirane_funkcije[it->first].begin())
         greska(-2);
      //for (int i = 0; i < deklarirane_funkcije[it->first].size(); i++) {
      //for (auto it_set = deklarirane_funkcije[it->first].begin(); it_set != deklarirane_funkcije[it->first].end(); it_set++) {
         //if (find(definirane_funkcije[it->first].begin(), definirane_funkcije[it->first].end(), it->second[i]) == definirane_funkcije[it->first].end())
         //if (definirane_funkcije[it->first].find(*it_set) == definirane_funkcije[it->first].end())   
            //greska(-2);
      //}
   }
}


void greska(); //sva mjesta di je greska nadopunit na kraju sa potrebnim stvarima
void primarni_izraz(string &tip, bool &l_izraz);
void izraz(string &tip, bool &l_izraz);
void postfiks_izraz(string &tip, bool &l_izraz);
void lista_argumenata(string &tipovi);
void izraz_pridruzivanja(string &tip, bool &l_izraz);
void unarni_izraz(string &tip, bool &l_izraz);
void cast_izraz(string &tip, bool &l_izraz);
void unarni_izraz(string &tip, bool &l_izraz);
//void unarni_operator();
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
            //tip = "fja void " + ntip;
            tip = "funkcija";
            //deklarirane_funkcije[ime] = "void " + ntip;
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
            //tip = "fja " + tipovi + " " + ntip;
            tip = "funkcija";
            //deklarirane_funkcije[ime] = tipovi + " " + ntip;
            deklarirane_funkcije[ime].insert(tipovi + " " + ntip);
         }
      }
      // else
      //    greska(dubina);
   }
}

void init_deklarator(string ntip) {
   int dubina = ulaz.dubina;
   iduca_linija();
   int dubina_prod = ulaz.dubina;
   string tip_dekl;
   int br_elem_dekl;
   izravni_deklarator(ntip, tip_dekl, br_elem_dekl);
   iduca_linija();
   if (ulaz.dubina != dubina_prod) {
      vrati_se_liniju_nazad();
      if (tip_dekl.find("const") != string::npos)
         greska(dubina);
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
   //iduca_linija(); mislim da ne treba ocitat taj zadnji znak
}

void lista_deklaracija() {
   //int dubina = ulaz.dubina;
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
      //tipovi = tipovi + "_" + tip;
      //imena = imena + "_" + ime;
      tipovi = tipovi + "-" + tip;
      imena = imena + "-" + ime;
   }
}

void definicija_funkcije() {
   int dubina = ulaz.dubina;
   iduca_linija();
   string tip;
   ime_tipa(tip);
   if (!(tip != "const_int" || tip != "const_char"))
      greska(dubina);
   iduca_linija();
   if (ulaz.klasa != "IDN")
      greska(dubina);
   tablica_znakova* iduca_tablica = trenutna_tablica;
   string ime = ulaz.vrijednost;
   //jel postoji definirana negdje
   // do {
   //    // if (iduca_tablica->funkcije.find(ime) != iduca_tablica->funkcije.end())
   //    //    greska(dubina);
   //    iduca_tablica = iduca_tablica->ugnijezdjeni_blok;
   // } while (iduca_tablica->ugnijezdjeni_blok != nullptr);
   // while (iduca_tablica->ugnijezdjeni_blok != nullptr) // mislim da ovo ne treba je bi vec trebo bit u globalnom djelokrugu
   //    iduca_tablica = iduca_tablica->ugnijezdjeni_blok;
   //if (get<2>(definirane_funkcije[ime]) == true) ovo je krivo jer ako se nalazi u mapi, onda je sigurno definirana
   if (definirane_funkcije.find(ime) != definirane_funkcije.end())
      greska(dubina);
   // ako koristim setove (u slucaju da moze bit vise fja istog imena) tu mi treba neka provjera dodatna ako je definirana vec s istim parametrima
   // if (definirane_funkcije[ime].find("void " + tip) != definirane_funkcije[ime].end())
   //    greska(dubina);
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
      //stvori novi djelokrug
      stvori_novi_djelokrug();
      //trenutna_tablica->tablica[ime] = make_pair("funkcija", "void " + tip);
      //definirane_funkcije[ime].insert("void " + tip);
      deklarirane_funkcije[ime].insert("void " + tip);
      definirane_funkcije[ime] = "void " + tip;
      //deklarirane_funkcije[ime] = "void " + tip;
      trenutna_tablica->return_vrijednost = tip;
      trenutna_tablica->parametri = "void";
   }
   else if (ulaz.izraz == "<lista_parametara>") {
      string tipovi;
      string imena;
      lista_parametara(tipovi, imena);
      // ako koristim setove (u slucaju da moze bit vise fja istog imena) tu mi treba neka provjera dodatna ako je definirana vec s istim parametrima
      // if (definirane_funkcije[ime].find(tipovi + " " + tip) != definirane_funkcije[ime].end())
      //    greska(dubina);
      //jel postoji u globalnom
      if (iduca_tablica->tablica.find(ime) != iduca_tablica->tablica.end() && iduca_tablica->tablica[ime].first == "funkcija") {
         if (iduca_tablica->tablica[ime].second != tipovi + "_" + tip)
            greska(dubina);
      }
      iduca_linija();
      if (ulaz.klasa != "D_ZAGRADA")   
      greska(dubina);
      trenutna_tablica->tablica[ime] = make_pair("funkcija",  tipovi + " " + tip);
      //stvori novi djelokrug
      stvori_novi_djelokrug();
      //definirane_funkcije[ime].insert(tipovi + " " + tip);
      //deklarirane_funkcije[ime].push_back("void " + tip);
      definirane_funkcije[ime] = tipovi + " " + tip;
      deklarirane_funkcije[ime].insert(tipovi + " " + tip);
      trenutna_tablica->return_vrijednost = tip;
      trenutna_tablica->parametri = tipovi;
      
      vector<string> tipovi_vec = split(tipovi, "-");
      vector<string> imena_vec = split(imena, "-");
      for (size_t i = 0; i < imena_vec.size(); i++) {
         if (tipovi_vec[i].find("const") != string::npos) 
            trenutna_tablica->tablica[imena_vec[i]] = make_pair("varijabla", tipovi_vec[i] + " false");
         else
            trenutna_tablica->tablica[imena_vec[i]] = make_pair("varijabla", tipovi_vec[i] + " true");
      }
   }
   iduca_linija();
   slozena_naredba();
}

void vanjska_deklaracija() {
   //int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<definicija_funkcije>") {
      definicija_funkcije();
   }
   else if (ulaz.izraz == "<deklaracija>") {
      deklaracija();
   }
   // else
   //    greska(dubina);
}

void prijevodna_jedinica() {
   //int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<vanjska_deklaracija>")
      vanjska_deklaracija();
   else if (ulaz.izraz == "<prijevodna_jedinica>") {
      prijevodna_jedinica();
      iduca_linija();
      vanjska_deklaracija();
   }
   // else
   //    greska(dubina);
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
      if (ulaz.klasa == "TOCKAZAREZ") {
         if (!(/*trenutna_tablica->parametri != "void" &&*/ trenutna_tablica->return_vrijednost == "void"))
            greska(dubina);
         //iduca_linija();
      }
      else if (ulaz.izraz == "<izraz>") {
         string tip;
         bool l_izraz;
         izraz(tip, l_izraz);
         if (!(/*trenutna_tablica->parametri != "void" &&*/ trenutna_tablica->return_vrijednost != "void"
               && moze_li_se_pretvoriti_u_vrijednost(tip, trenutna_tablica->return_vrijednost)))
                  greska(dubina);
         iduca_linija();
         if (ulaz.klasa != "TOCKAZAREZ")
            greska(dubina);
      }
      else
         greska(dubina);
   }
   // else 
   //    greska(dubina);
}

void naredba_petlje() {
   int dubina = ulaz.dubina;
   //potencijalno premjestit unutar blokova drito ispred naredba()
   //u_petlji++;
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
   // else 
   //    greska(dubina);
}

void naredba_grananja() {
   int dubina = ulaz.dubina;
   iduca_linija();
   int dubina_prod = ulaz.dubina;
   if (ulaz.klasa == "KR_IF") {
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
      naredba();
      iduca_linija();
      if (ulaz.dubina != dubina_prod)
         vrati_se_liniju_nazad();
      else {
         iduca_linija();
         naredba();
      }
   }
   // else
   //    greska(dubina);
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
   // else
   //    greska(dubina);
}

void naredba() {
   //int dubina = ulaz.dubina;
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
   //int dubina = ulaz.dubina;
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
      //u_petlji--;
      unisti_trenutni_djelokrug();
   }

   // else
   //    greska(dubina);
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
      string tip_izraz;
      if (!l_izraz_post)
         greska(dubina);
      iduca_linija();
      if (ulaz.klasa != "OP_PRIDRUZI")
         greska(dubina);
      iduca_linija();
      izraz_pridruzivanja(tip_izraz, l_izraz);
      moze_li_se_pretvoriti_jedan_u_drugi(tip_izraz, tip_post) ? (void)0 : greska(dubina);
      tip = tip_post;
      l_izraz = false;
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
      iduca_linija();
      odnosni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
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
      iduca_linija();
      aditivni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
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
      iduca_linija();
      if (ulaz.klasa != "PLUS" && ulaz.klasa != "MINUS") 
         greska(dubina);
      iduca_linija();
      multiplikativni_izraz(tip, l_izraz);
      moze_li_se_pretvoriti_u_int(tip) ? (void)0 : greska(dubina);
      tip = "int";
      l_izraz = 0;
   }
   // else 
   //    greska(dubina);
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
   // else 
   //    greska(dubina);
}

void specifikator_tipa(string &tip) {
   //int dubina = ulaz.dubina;
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
   // else
   //    greska(dubina);
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
         //greska("<cast_izraz> ::= L_ZAGRADA(" + ulaz.redak + ",() <ime_tipa> D_ZAGRADA(" + ulaz.redak + ",)) <cast_izraz>");
      iduca_linija();
      cast_izraz(tip, l_izraz);
      if ((tip == "int" || tip == "const_int" || tip == "char" || tip == "const_char")
         && (ime_tip == "int" || ime_tip == "const_int" || ime_tip == "char" || ime_tip == "const_char")) {
            tip = ime_tip;
            l_izraz = 0;
         }
      else
         greska(dubina);
         //greska("<cast_izraz> ::= L_ZAGRADA(" + ulaz.redak + ",() <ime_tipa> D_ZAGRADA(" + ulaz.redak + ",)) <cast_izraz>");
   }
   // else
   //    greska(dubina);
}

// void unarni_operator() {
//    iduca_linija();
//    if (ulaz.klasa != "PLUS" || ulaz.klasa != "MINUS" || ulaz.klasa != "OP_TILDA" || ulaz.klasa != "OP_NEG")
//       greska(dubina);
// }

void unarni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<postfiks_izraz>") {
      postfiks_izraz(tip, l_izraz);
   }
   else if (ulaz.klasa == "OP_INC" || ulaz.klasa == "OP_DEC") {
      iduca_linija();
      unarni_izraz(tip, l_izraz);
      if (l_izraz == 1 && ((moze_li_se_pretvoriti_u_int(tip)))) {
         tip = "int";
         l_izraz = false;
      }
      else
         greska(dubina);
         //greska("<unarni_izraz> ::= (OP_INC | OP_DEC)(" + ulaz.redak + "+ | -) <unarni_izraz>");
   }
   else if (ulaz.izraz == "<unarni_operator>") {
      iduca_linija();
      iduca_linija();
      cast_izraz(tip, l_izraz);
      if ((moze_li_se_pretvoriti_u_int(tip))) {
         tip = "int";
         l_izraz = false;
      }
      else
         greska(dubina);
         //greska("<unarni_izraz> ::= <unarni_operator> <cast_izraz>");
   }
   // else
   //    greska(dubina);
}

void lista_argumenata(string &tipovi) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.izraz == "<izraz_pridruzivanja>") {
      string tip;
      bool l_izraz;
      izraz_pridruzivanja(tip, l_izraz);
      tipovi = tip;
   }
   else if (ulaz.izraz == "<lista_argumenata>") {
      lista_argumenata(tipovi);
      iduca_linija();
      if (ulaz.klasa != "ZAREZ")
         greska(dubina);
         //greska("<lista_argumenata> ::= <lista_argumenata> ZAREZ(" + ulaz.redak + ",) <izraz_pridruzivanja>");
      iduca_linija();
      string tip;
      bool l_izraz;
      izraz_pridruzivanja(tip, l_izraz);
      tipovi = tipovi + "-" + tip;
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
      string ime = ulaz.vrijednost; //ocitao sam ime funkcije
      iduca_linija();
      string redak;
      if (ulaz.klasa == "L_UGL_ZAGRADA") {
         iduca_linija();
         if (tip_post.find("niz") == string::npos)
            greska(dubina);
            //greska("<postfiks_izraz> ::= <postfiks_izraz> L_UGL_ZAGRADA(" + ulaz.redak + ",[) <izraz> D_UGL_ZAGRADA(" + ulaz.redak + "])");
         string tip_izr;
         bool l_izraz_izr;
         izraz(tip_izr, l_izraz_izr);
         moze_li_se_pretvoriti_u_int(tip_izr) ? (void)0 : greska(dubina);//greska("<postfiks_izraz> ::= <postfiks_izraz> L_UGL_ZAGRADA(" + ulaz.redak + ",[) <izraz> D_UGL_ZAGRADA(" + ulaz.redak + "])");
         l_izraz = tip.find("const") != string::npos ? 0 : 1;
         tip = tip_post.substr(tip_post.find("niz")+4);
         iduca_linija();
         if (ulaz.klasa != "D_UGL_ZAGRADA")
            greska(dubina);
            //greska("<postfiks_izraz> ::= <postfiks_izraz> L_UGL_ZAGRADA(" + ulaz.redak + ",[) <izraz> D_UGL_ZAGRADA(" + ulaz.redak + "])");
      }
      else if (ulaz.klasa == "L_ZAGRADA") {
         iduca_linija();
         if (ulaz.klasa == "D_ZAGRADA") {
            // if (trenutna_tablica->tablica.find(ime) != trenutna_tablica->tablica.end() && trenutna_tablica->tablica[ime].first == "funkcija") {
            //mislim da mi ovo gore ne treba jer sam vec sa postfiks_izraz provjerio jel definirana, samo trebam vidjet jel to zapravo fja
            if (tip_post.find("funkcija") != string::npos) {   
               vector<string> fja_arg_pov = split(tip_post, " ");
               if (fja_arg_pov[1] != "void") //|| fja[1] == "void")
                  greska(dubina);
                  //greska("<postfiks_izraz> ::= <postfiks_izraz> L_ZAGRADA(" + ulaz.redak + ",() D_ZAGRADA(" + ulaz.redak + "))");
               tip = fja_arg_pov[2];
               l_izraz = 0;
            }
            else
               greska(dubina);
               //greska("<postfiks_izraz> ::= <postfiks_izraz> L_ZAGRADA(" + ulaz.redak + ",() D_ZAGRADA(" + ulaz.redak + "))");
         }
         else if (ulaz.izraz == "<lista_argumenata>") {
            string tipovi;
            lista_argumenata(tipovi);
            iduca_linija();
            if (tip_post.find("funkcija") != string::npos) {
               vector<string> fja_arg_pov = split(tip_post, " ");
               if (fja_arg_pov[1] == "void")
                  greska(dubina);
                  //greska("<postfiks_izraz> ::= <postfiks_izraz> L_ZAGRADA(" + ulaz.redak + ",() <lista_argumenata> D_ZAGRADA(" + ulaz.redak + "))");
               vector<string> tipovi_vec = split(tipovi, "-");
               vector<string> parametri_vec = split(fja_arg_pov[1], "-");
               if (tipovi_vec.size() != parametri_vec.size())
                  greska(dubina);
               for (size_t i = 0; i < tipovi_vec.size(); i++) {
                  moze_li_se_pretvoriti_u_vrijednost(tipovi_vec[i], parametri_vec[i]) ? (void)0 : greska(dubina);//greska("<postfiks_izraz> ::= <postfiks_izraz> L_ZAGRADA(" + ulaz.redak + ",() <lista_argumenata> D_ZAGRADA(" + ulaz.redak + "))");
               }
               tip = fja_arg_pov[2];
               l_izraz = false;
            }
         }
         else 
            greska(dubina);
            //greska("<postfiks_izraz> ::= <postfiks_izraz> L_ZAGRADA(" + ulaz.redak + ",() D_ZAGRADA(" + ulaz.redak + "))");
      }
      else if (ulaz.klasa == "OP_INC" || ulaz.klasa == "OP_DEC") {
         iduca_linija();
         if ((l_izraz_post == 1 && ((moze_li_se_pretvoriti_u_int(tip_post))))) {
            tip = "int";
            l_izraz = false;
         }
         else
            greska(dubina);
            //greska("<postfiks_izraz> ::= <postfiks_izraz>  (OP_INC | OP_DEC)(" + ulaz.redak + "+ | -)");
      }
      // else
      //    greska(dubina);
   }
}

void primarni_izraz(string &tip, bool &l_izraz) {
   int dubina = ulaz.dubina;
   iduca_linija();
   if (ulaz.klasa == "IDN") {
      //u tablici znakova su mi varijable zapisane kao ime -> <"varijabla", "tip l_izraz">
      //u tablici znakova su mi funkcije  zapisane kao ime -> <"funkcija", "argumenti povratna_vrijednost">
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
         }
      }
      else
         greska(dubina);
         //greska("<primarni_izraz> ::= IDN(" + ulaz.redak + "," + ulaz.vrijednost + ")");
   }
   else if (ulaz.klasa == "BROJ") {
      //if (stoi(ulaz.vrijednost) >= -2147483648 && stoi(ulaz.vrijednost) <= 2147483647) {
      try {
         stoi(ulaz.vrijednost);
         tip = "int";
         l_izraz = false;
      }
      //else
         //greska("<primarni_izraz> ::= BROJ(" + ulaz.redak + "," + ulaz.vrijednost + ")");
      catch (...) {
         greska(dubina);
      }
   }
   //mozda treba provjera je li tip char, ali mislim da se to vec podrazumijeva ako je klasa ZNAK
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
            //greska("<primarni_izraz> ::= ZNAK(" + ulaz.redak + "," + ulaz.vrijednost + ")");
      }
      else {
         tip = "char";
         l_izraz = false;
      }
   }
   //ovdje isto ko i ovo gore, ali mislim da ne bi trebalo jer je to osigurano sve leksickom analizom
   else if (ulaz.klasa == "NIZ_ZNAKOVA") {
      size_t poz = ulaz.vrijednost.find("\\");
      while(poz != string::npos) {
         poz++;
         if (!((ulaz.vrijednost[poz] == '\"' && ulaz.vrijednost[poz+1] != '\"') || ulaz.vrijednost[poz] == 't' || ulaz.vrijednost[poz] == 'n'
            || ulaz.vrijednost[poz] == '\''|| ulaz.vrijednost[poz] == 0 || ulaz.vrijednost[poz] == '\\'))
               //greska("<primarni_izraz> ::= NIZ_ZNAKOVA(" + ulaz.redak + "," + ulaz.vrijednost + ")");
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
         //greska("<primarni_izraz> ::= L_ZAGRADA(" + ulaz.redak + ",()" + "<izraz> " + "D_ZAGRADA(" + ulaz.redak + ",))");
         greska(dubina);
      }
   }
   // else
   //    greska(dubina);
}


int main() {

   string s;
   while(getline(cin, s)) {
      //s = s + '\n';
      //ulazni_niz += s;
      ulazni_niz.push_back(s);
   }
   iduca_linija();
   prijevodna_jedinica();
   
   konacne_provjere();

   return 0;
}
