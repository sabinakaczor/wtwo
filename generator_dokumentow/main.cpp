#include <iostream>
#include <vector>
#include <fstream>

//scenariusz: artykul / blog
// artykul w ascii, blog w html
//builder: html / tekstowy

/*Wykorzystując wzorzec budowniczy stwórz generator dokumentów tekstowych i HTML.
Generator powinien obsługiwać:
dodawanie paragrafów
<p>tekst paragrafu</p> dla html, tekst paragrafu dla dokumentu tekstowego
dodawanie linii poziomych
<hr> dla html, “----------------------------” dla dokumentu tekstowego
rozpoczynanie dokumentu
<html> dla html, brak działania dla dokumentu tekstowego
kończenie dokumentu
</html> dla html, brak działania dla dokumentu tekstowego
Napisz program generujacy tekst dla artykulu i dla blogu pobierając listę tekstów z pliku (wczytujemy linia po linii),
i generujący wynik odpowiednio dla artykułu:
linia pozioma
linia tekstu
linia pozioma
linia tekstu
....

i dla blogu:
linia pozioma
linia pozioma
linia pozioma
linia tekstu
linia pozioma
linia pozioma
linia pozioma
linia tekstu
....*/

using namespace std;

class Dokument
{
public:
    Dokument() {}
    void show_text() {
        for(int i=0; i<this->edited_text.size(); ++i)
            cout << edited_text[i] << endl;
    }
    vector<string> get_text() { return this->edited_text; }
    void dodaj_element(string s) { this->edited_text.push_back(s); }
private:
    vector<string> edited_text;
};

class DokumentBuilder
{
protected:
    Dokument *dok;
public:
    DokumentBuilder() {}
    virtual void edit_text(vector<string> text) = 0;
    void start_editing_text() { this->dok = new Dokument(); }
    Dokument* get_dokument() { return this->dok; }
    virtual void rozpocznij_paragraf() = 0;
    virtual void zakoncz_paragraf() = 0;
    virtual void dodaj_linie_pozioma() = 0;
    virtual void rozpocznij_dokument() = 0;
    virtual void zakoncz_dokument() = 0;
    virtual void dodaj_linie(string s) = 0;
};

class DokumentHTMLBuilder : public DokumentBuilder
{
public:
    DokumentHTMLBuilder() : DokumentBuilder() {}
    void rozpocznij_paragraf() {
        dok->dodaj_element("<p>");
    }
    void zakoncz_paragraf() {
        dok->dodaj_element("</p>");
    }
    void dodaj_linie_pozioma() {
        dok->dodaj_element("<hr>");
    }
    void rozpocznij_dokument() {
        dok->dodaj_element("<html>");
    }
    void zakoncz_dokument() {
        dok->dodaj_element("</html>");
    }
    void dodaj_linie(string s) {
        dok->dodaj_element(s);
    }
    void edit_text(vector<string> text) {
        this->rozpocznij_dokument();
        for (int i=0; i<text.size(); ++i) {
            this->dodaj_linie_pozioma();
            this->dodaj_linie_pozioma();
            this->dodaj_linie_pozioma();
            if (i==0)
                this->rozpocznij_paragraf();
            else {
                if (text[i].size()>0 && text[i-1].size()==0)
                    this->rozpocznij_paragraf();
                else if (text[i].size()==0)
                    this->zakoncz_paragraf();
            }
            this->dodaj_linie(text[i]);
            if (i==text.size()-1)
                this->zakoncz_paragraf();
        }
        this->zakoncz_dokument();
    }
};

class DokumentTXTBuilder : public DokumentBuilder
{
public:
    DokumentTXTBuilder() : DokumentBuilder() {}
    void rozpocznij_paragraf() {}
    void zakoncz_paragraf() {}
    void dodaj_linie_pozioma() {
        dok->dodaj_element("----------------------------");
    }
    void rozpocznij_dokument() {}
    void zakoncz_dokument() {}
    void dodaj_linie(string s) {
        dok->dodaj_element(s);
    }
    void edit_text(vector<string> text) {
        this->rozpocznij_dokument();
        for (int i=0; i<text.size(); ++i) {
            this->dodaj_linie_pozioma();
            this->dodaj_linie(text[i]);
        }
        this->zakoncz_dokument();
    }
};

class Editor
{
public:
    Editor(vector<string> text) { this->text = text; }
    void generate_document() {
        this->db->start_editing_text();
        this->db->edit_text(this->text);
    }
    void set_dokument_builder(DokumentBuilder *db) { this->db = db; }
    Dokument* get_dokument() { return this->db->get_dokument(); }
private:
    DokumentBuilder *db;
    vector<string> text;
};

vector<string> wczytaj_plik(const char* name) {
    ifstream f;
    string line;
    vector<string> text;

    f.open(name,ios::in);

    if(f.is_open()) {
        getline( f, line );
        while(!f.eof()) {
            text.push_back(line);
            getline( f, line );
        }
        f.close();
    }else {
        cout << "blad otwarcia " << name;
    }
    return text;
}

void zapisz_do_pliku(vector<string> text, const char* name) {
    ofstream plik;
    plik.open(name);
    for(int i=0; i<text.size(); ++i)
        plik<<text[i]<<endl;
    plik.close();
}

int main()
{
    vector<string> text = wczytaj_plik("pure_text.txt");
    Editor *editor = new Editor(text);
    DokumentHTMLBuilder *dokhtmlbuilder = new DokumentHTMLBuilder();
    DokumentTXTBuilder *doktxtbuilder =  new DokumentTXTBuilder();

    editor->set_dokument_builder(dokhtmlbuilder);
    editor->generate_document();
    Dokument *dokhtml = editor->get_dokument();
    vector<string> texthtml = dokhtml->get_text();

    editor->set_dokument_builder(doktxtbuilder);
    editor->generate_document();
    Dokument *doktxt = editor->get_dokument();
    vector<string> texttxt = doktxt->get_text();

    zapisz_do_pliku(texthtml, "blog.txt");
    zapisz_do_pliku(texttxt, "artykul.txt");

    cout << "Zapisano do plików.";

    return 0;
}


