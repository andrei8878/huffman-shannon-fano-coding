#include <iostream>
#include "conv.h"


conv::conv(){ std::cout << "\n"; }

std::vector<uint32_t> conv::utf_conv(std::string& text){
    std::vector<uint32_t> co{};
    for(int x = 0 ;x<text.size();x++){
        if((text[x] & 0x80) == 0x0){
            // a = 97 => 01100001 &
            // 0x80   => 10000000  
            //        =  00000000 (1 octet)
            co.push_back(text[x]);
        }else if((text[x] & 0xE0) == 0xC0){
            // ă    = 11000100 10000011 &
            // 0xE0 = 11100000 00000000
            //      = 11000000 00000000 == 0xC0
            // 11000100& 
            // 00011111
            // 00000100
            // 000aaaaa << 6 = > aaaaa000000
            // vrem sa salvem primii 5 biti din primul octet si cei 6 din al doilea octet are sens? o sa mor nebun
            // pt oc1 trb : 110xxxxx => xxx11111 = 0x1F
            uint32_t oc1 = (text[x] & 0x1F) << 6;
            // pt oc2 trb : 10xxxxxx => xx111111 = 0x3F  
            uint8_t oc2 = text[x+1] & 0x3F;
            uint32_t hex = oc1 | oc2; // rez: 0xhex
            co.push_back(hex);
            x++;
        }
    }
    return co;
}
std::string conv::utf_conv2(char32_t c){
    std::string rezultat;
    if(c < 0x80){
        rezultat += (char)c;
    }else { 
        rezultat += (char)(0xc0 | (c >> 6));
        rezultat += (char)(0x80 | (c & 0x3F));
    }
    return rezultat;
}

void verificaremapare(const conv& c, std::vector<int>& caractere, int& total, int hex);


std::tuple<std::vector<int>,int> conv::decodificare(std::vector<uint32_t> utf_valori){
    std::vector<int> caractere(31,0);
    int total = 0;
    for(int i = 0;i<utf_valori.size();i++){
        if(utf_valori[i]>= U'a' && utf_valori[i]<=U'z'){
            caractere[utf_valori[i]-U'a']++;
            total++;
        }else if(utf_valori[i]>=U'A' && utf_valori[i]<=U'Z'){
            caractere[utf_valori[i]-U'A']++;
            total++;
        }else{
            verificaremapare(*this,caractere,total,utf_valori[i]);
        }
    }
    std::cout << "conv.cpp caractere size: " << std::dec << caractere.size() << '\n';
    return std::make_tuple(caractere,total);
}

void verificaremapare(const conv& c, std::vector<int>& caractere, int& total, int hex){
    for(auto& x : c.diacritice){
        if(x.cod == hex){
            if(x.val <= 5){
                caractere[25+x.val]++;
                total++;
            }else if(x.val > 5){
                caractere[20+x.val]++;
                total++;
            }
        }
    }
}

///////////////

std::vector<conv::datelit>& conv::getdatelitere(){
    return datelitere;
}

std::vector<conv::mapare>& conv::getdiacritice(){
    return diacritice;
}

double entropia(const conv& c){
    std::vector<conv::datelit> mapacaractere = c.datelitere;
    double sumaEntropie = 0;
    for(auto& l : mapacaractere){
        if(l.probabilitate > 0){
            sumaEntropie += l.probabilitate * log2(l.probabilitate);
        }else{continue;}
    }
    return -sumaEntropie;
}

double conv::calculareProbabilitate(int apc, int totalap, double& totalprobabilitate){
    totalprobabilitate += (double)apc/totalap;
    return (double)apc/totalap;
}

double conv::lungimeamedie(std::vector<datelit>& mapacaractere){
    double suma = 0;
    for(auto& x: mapacaractere){
        if(x.probabilitate > 0){
            suma += x.probabilitate * x.codbinar.size();
        }
    }
    return suma;
}

double conv::redudanta(std::vector<datelit>& mapacaractere){
    return lungimeamedie(mapacaractere) - entropia(*this);
}

// void afisareLitere(const conv& c){
//     for(auto& x : c.datelitere){
//         ;
//     }
// }

void conv::valentropia(){
    std::cout << "Entropia: " <<  entropia(*this) << '\n';
}

void conv::sortaredescrescatoare(std::vector<conv::datelit>& mapacaractere){
    std::sort(mapacaractere.begin(), mapacaractere.end(),
    [](const datelit& a, const datelit& b){
        return a.probabilitate > b.probabilitate;
    });
}


//snannonfano
void conv::shannonfano(std::vector<conv::datelit>& mapacaractere, int st, int dr, double sumaprobabilitate){
    double secvsumaprob = 0;
    int mij = st;
    if (st>=dr){return;}
    while(secvsumaprob<=sumaprobabilitate/2 && mij < dr){
        secvsumaprob+= mapacaractere[mij].probabilitate;
        mij++;
    }
    for(int i = st;i<mij;i++){
        mapacaractere[i].codbinar += "0";
    }
    for(int i = mij; i <= dr; i++){
        mapacaractere[i].codbinar += "1";
    }
    shannonfano(mapacaractere, st, mij-1,secvsumaprob);
    shannonfano(mapacaractere,mij,dr,sumaprobabilitate - secvsumaprob);    
}

// huffman

void conv::huffman(std::vector<conv::datelit>& mapacaractere){
    std::vector<nodHuffman*> coada;
    for(auto& lit : mapacaractere){
        if(lit.probabilitate > 0){
            nodHuffman* nod = new nodHuffman{lit.probabilitate, &lit, nullptr, nullptr};
            coada.push_back(nod);
        }
    }
    while(coada.size() > 1){
        std::sort(coada.begin(), coada.end(),
            [](nodHuffman* a, nodHuffman* b){
                return a->probabilitate < b->probabilitate;
            });
        nodHuffman* mic1 = coada[0];
        nodHuffman* mic2 = coada[1];

        nodHuffman* parinte = new nodHuffman{
            mic1->probabilitate + mic2->probabilitate,
            nullptr,
            mic1,
            mic2
        };

        coada.erase(coada.begin(), coada.begin() + 2);
        coada.push_back(parinte);
    }
    nodHuffman* radacina = coada[0];
    if(radacina->litera != nullptr){
        radacina->litera->codbinar = "0";
    } else {
        genereazaCoduri(radacina, "");
    }
}

void conv::genereazaCoduri(nodHuffman* nod, std::string codCurent){
    if(nod == nullptr) return;

    if(nod->litera != nullptr){
        nod->litera->codbinar = codCurent;
        return;
    }

    genereazaCoduri(nod->stanga, codCurent + "0");
    genereazaCoduri(nod->dreapta, codCurent + "1");
}