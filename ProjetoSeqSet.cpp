#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct RegistroTrade {
    int time_ref;
    char account[20];
    char code[20];
    char country_code[20];
    char product_type[20];
    float value;
    char status[10];
};

class InternationalTrade{
    private:
    int quantidadeTrades = 0;

    public:
    InternationalTrade(){
        quantidadeTrades = 0;
        
    }

    ~InternationalTrade() {

    }

    void leituraArquivoCSV();
};


void InternationalTrade::leituraArquivoCSV(){

    ifstream arquivoCSV("international-trade-june-2022-quarter-csv.csv");
    if (not (arquivoCSV)){
        cout << "Erro ao abrir o arquivo CSV!" << endl;
    }

    string linha;
    getline(arquivoCSV, linha); //pular o cabeçalho

    RegistroTrade* dados = new RegistroTrade[quantidadeTrades];
    char lixo;


    while (arquivoCSV >> dados[quantidadeTrades].time_ref){
        arquivoCSV >> lixo;
        arquivoCSV.getline(dados[quantidadeTrades].account, 20, ',');
        arquivoCSV.getline(dados[quantidadeTrades].code, 20, ',');
        arquivoCSV.getline(dados[quantidadeTrades].country_code, 20, ',');
        arquivoCSV.getline(dados[quantidadeTrades].product_type, 20, ',');
        arquivoCSV >> dados[quantidadeTrades].value;
        arquivoCSV >> lixo;
        arquivoCSV.getline(dados[quantidadeTrades].status, 10);
        arquivoCSV.ignore();

        quantidadeTrades++;

        for (int i = 0; i < quantidadeTrades; i++) {
            cout << "time_ref: " << dados[i].time_ref << endl;
            cout << "account: " << dados[i].account << endl;
            cout << "code: " << dados[i].code << endl;
            cout << "country_code: " << dados[i].country_code << endl;
            cout << "product_type: " << dados[i].product_type << endl;
            cout << "value: " << dados[i].value << endl;
            cout << "status: " << dados[i].status << endl;
            cout << endl;
        }
    }
}



int main(){

    InternationalTrade trade;
    trade.leituraArquivoCSV();

    return 0;
}