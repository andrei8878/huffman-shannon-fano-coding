#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <cmath>
#include <tuple>
#include <algorithm>


class conv{
public:
    struct mapare{
        char32_t litera;
        int cod;
        int val;
    };
    struct datelit{
        char32_t litera;
        int aparitii;
        double procentaj;
        double probabilitate;
        std::string codbinar; 
    };
    struct nodHuffman{
        double probabilitate;
        datelit* litera;
        nodHuffman* stanga;
        nodHuffman* dreapta;
    };
private:
    void genereazaCoduri(nodHuffman* nod, std::string codCurent);
    std::vector<mapare> diacritice = {
        {U'ă',259,1},
        {U'â',226,2},
        {U'î',238,3},
        {U'ș',537,4},
        {U'ț',539,5},
        {U'Ă',258,6},
        {U'Â',194,7},
        {U'Î',206,8},
        {U'Ș',536,9},
        {U'Ț',538,10},
    };
    std::vector<datelit> datelitere;
public:
    conv();
    std::vector<uint32_t> utf_conv(std::string& text);
    std::string utf_conv2(char32_t c);
    std::tuple<std::vector<int>,int> decodificare(std::vector<uint32_t> utf_valori);
    double calculareProbabilitate(int apc, int totalap, double& totalprobabilitate);
    double lungimeamedie(std::vector<datelit>& mapacaractere);
    double redudanta(std::vector<datelit>& mapacaractere);
    void valentropia();
    friend void verificaremapare(const conv& c,std::vector<int>& caractere, int& total, int hex);
    friend double entropia(const conv& c);
    friend void afisareLitere(const conv& c);
    
    std::vector<datelit>& getdatelitere();
    std::vector<mapare>& getdiacritice();

    // Alg

    void sortaredescrescatoare(std::vector<datelit>& mapacaractere);
    void shannonfano(std::vector<datelit>& mapacaractere, int st, int dr, double sumaprobabilitate);
    void huffman(std::vector<datelit>& mapacaractere);


};