#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>
#include <windows.h>
#include "cv/conv.h"

std::ifstream fin("text.txt");

int main(){
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    conv conv;
    std::vector<std::vector<double>> probabilitatea(26);    
    std::vector<int> caractere;
    int total = 0;
    double sumaprocentaje = 0;
    std::string text; 
    
    if(!fin.is_open()){
        std::cout << "Nu am gasit fisierul \n";
        return -1;
    }
    
    std::getline(fin,text);
    std::vector<uint32_t> utf32_int = conv.utf_conv(text);
    
    std::cout << "caracter: " <<utf32_int[0] << '\n';
    std::cout << "utf32_int size: " << utf32_int.size() << '\n';
    std::tie(caractere,total) = conv.decodificare(utf32_int);
    // for(auto& x: caractere){
    //     std::cout << std::dec << x << '\n';
    // }
    // for(auto x : utf32_int){s
    //     std::cout << x << " ";
    // }
    std::cout << '\n';
    std::vector<conv::datelit>& mapacaractere = conv.getdatelitere();
    std::vector<conv::mapare>& diacritice = conv.getdiacritice();
    mapacaractere.resize(31);
    double totalprobabilitate = 0;
    // for(int i = 0 ;i<caractere.size();i++){
    //     double rezultat = conv.calculareProbabilitate(caractere[i],total, totalprobabilitate);
    //     mapacaractere[i].litera = char('a'+ i);
    //     mapacaractere[i].aparitii = caractere[i];
    //     mapacaractere[i].procentaj = ((double)caractere[i]/total)*100;
    //     mapacaractere[i].probabilitate = rezultat;
    //     sumaprocentaje +=  ((double)caractere[i]/total)*100;
    //     if(i>25){
    //         for(auto& x : diacritice){
    //             int contor = 0;
    //             if(x.cod == utf32_int[i] && i < 32){
    //                 double rezultat = conv.calculareProbabilitate(caractere[i],total,totalprobabilitate);
    //                 mapacaractere[i].litera = diacritice[contor].litera;
    //                 mapacaractere[i].aparitii = caractere[i];
    //                 mapacaractere[i].procentaj = ((double)caractere[i]/total)*100;
    //                 mapacaractere[i].probabilitate = rezultat;
    //                 i++;
    //                 contor++;
    //             }
    //         }
    //         break;
    //     }
    // }
    for(int i = 0; i < 26; i++){
        mapacaractere[i].litera = U'a' + i;
        mapacaractere[i].aparitii = caractere[i];
        if(total > 0){
            mapacaractere[i].probabilitate = conv.calculareProbabilitate(caractere[i], total, totalprobabilitate);
            mapacaractere[i].procentaj = ((double)caractere[i]/total) * 100;
            sumaprocentaje += mapacaractere[i].procentaj;
        }
    }

    for(auto& d : diacritice){
        if(d.val >= 1 && d.val <= 5){
            int idx = 25 + d.val;
            mapacaractere[idx].litera = d.litera;
            mapacaractere[idx].aparitii = caractere[idx];
            if(total > 0){
                mapacaractere[idx].probabilitate = conv.calculareProbabilitate(caractere[idx], total, totalprobabilitate);
                mapacaractere[idx].procentaj = ((double)caractere[idx]/total) * 100;
                sumaprocentaje += mapacaractere[idx].procentaj;
            }
        }
    }
    std::cout << "datele literelor\n";
    std::cout << std::left << std::setw(8) << "litera " << std::setw(15) << " aparitii " << std::setw(24) << " probabilitate " << std::setw(32) << " procentaj \n";

    for(auto& x : mapacaractere){
        std::cout << std::left << std::setw(8) << conv.utf_conv2(x.litera) << std::setw(15) << x.aparitii <<std::setw(24) << x.probabilitate << std::setw(32) << x.procentaj << '\n';
    }
    double valentropia = entropia(conv);
    std::cout << "Valoarea entropiei: " << valentropia << " biti/simbol\n";

    std::cout << "shannonfano\n";

    for(auto&x : mapacaractere) x.codbinar = "";

    conv.sortaredescrescatoare(mapacaractere);

    int ultimaliteravalida = -999;
    for(int i=0;i<mapacaractere.size();i++){
        if(mapacaractere[i].probabilitate>0)
            ultimaliteravalida = i;
    }

    if(ultimaliteravalida>=0){
        conv.shannonfano(mapacaractere, 0, ultimaliteravalida, totalprobabilitate);
    }
    std::cout << std::left << std::setw(8) << "litera" << std::setw(15) << "probabilitatea" << "cod\n";

    for(auto& x: mapacaractere){
        if(x.probabilitate > 0){
            std::cout << std::left << std::setw(8) << conv.utf_conv2(x.litera) << std::setw(15) << x.probabilitate << x.codbinar << '\n';
        }
    }

    double lgm_shannonfano = conv.lungimeamedie(mapacaractere);
    double red_shannonfano = conv.redudanta(mapacaractere);
    std::cout << "Lungimea medie shannonfano: "<< lgm_shannonfano << " biti/simbol \n";
    std::cout << "Redudanta shannonfano = " << red_shannonfano << " biti/simbol \n";
    std::cout << "Eficienta shannonfano = " << (valentropia/lgm_shannonfano)*100 << " biti/simbol\n";

    std::cout << "hufmann" << '\n';

    for(auto& x: mapacaractere) x.codbinar = "";
    conv.huffman(mapacaractere);

    std::cout << std::left << std::setw(8) << "litera" << std::setw(15) << "probabilitatea" << "cod\n";

    for(auto& x: mapacaractere){
        if(x.probabilitate > 0){
            std::cout << std::left << std::setw(8) << conv.utf_conv2(x.litera) << std::setw(15) << x.probabilitate << x.codbinar << '\n';
        }
    }

    double lgm_huffman  = conv.lungimeamedie(mapacaractere);
    double red_huffman = conv.redudanta(mapacaractere);
    std::cout << "Lungimea medie huffman: "<< lgm_huffman << " biti/simbol \n";
    std::cout << "Redudanta huffman = " << red_huffman << " biti/simbol \n";
    std::cout << "Eficienta huffman = " << (valentropia/lgm_huffman)*100 << " biti/simbol\n";


    fin.close();
}
