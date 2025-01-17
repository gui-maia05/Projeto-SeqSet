#include <iostream>
#include <fstream>

using namespace std;

struct RegistroTrade {
    int time_ref;
    char account[50];
    char code[50];
    char country_code[50];
    char product_type[50];
    float value;
    char status[10];
};

class InternationalTrade {
private:
    int quantTrades = 0;

public:
    InternationalTrade() {
        quantTrades = 0;
    }

    ~InternationalTrade() {}

    void leituraArquivoCSV();
    void transformarEmBinario(RegistroTrade* dados, int quantTrades);
    void leituraArquivoBinario();   
};

void InternationalTrade::leituraArquivoCSV() {
    string linha;
    char lixo;
    int i = 0;

    ifstream arquivoCSV("international-trade-june-2022-quarter-csv.csv");
    if (!arquivoCSV) {
        cout << "Erro ao abrir o arquivo CSV!" << endl;
        return;
    }

    getline(arquivoCSV, linha); // Pular o cabeçalho

    while (getline(arquivoCSV, linha)) {
        quantTrades++;
    }

    arquivoCSV.clear();
    arquivoCSV.seekg(0, ios::beg);
    getline(arquivoCSV, linha);
    
    RegistroTrade* dados = new RegistroTrade[quantTrades];
    
    while (arquivoCSV >> dados[i].time_ref) {
        arquivoCSV >> lixo; // Pular a vírgula
        arquivoCSV.getline(dados[i].account, 50, ',');
        arquivoCSV.getline(dados[i].code, 50, ',');
        arquivoCSV.getline(dados[i].country_code, 50, ',');
        arquivoCSV.getline(dados[i].product_type, 50, ',');
        arquivoCSV >> dados[i].value;
        if (arquivoCSV.fail()) {
            dados[i].value = 0.0; // Se falhou, atribui 0.0
            arquivoCSV.clear(); // Limpar o estado de falha
            arquivoCSV >> lixo; 
        } else {
            arquivoCSV >> lixo; 
        }
        arquivoCSV.getline(dados[i].status, 10);
        arquivoCSV.ignore();

        i++;
    }

    arquivoCSV.close();

    transformarEmBinario(dados, quantTrades);

    cout << "Arquivo binario criado com sucesso!" << endl;

    delete[] dados;
}

void InternationalTrade::transformarEmBinario(RegistroTrade* dados, int quantTrades) {
    ofstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
    arquivoBinario.write((char*)dados, sizeof(RegistroTrade) * quantTrades);
    arquivoBinario.close();
}

void InternationalTrade::leituraArquivoBinario() {
    ifstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
    if (!arquivoBinario) {
        cout << "Erro ao abrir o arquivo binário!" << endl;
        return;
    }

    arquivoBinario.seekg(0, ios::end);
    streamsize tamanhoArquivo = arquivoBinario.tellg();
    arquivoBinario.seekg(0, ios::beg);

    quantTrades = tamanhoArquivo / sizeof(RegistroTrade);

    RegistroTrade* dados = new RegistroTrade[quantTrades];
    
    arquivoBinario.read((char*)dados, tamanhoArquivo);
    
    arquivoBinario.close();

    delete[] dados;
}

int main() {
    int opcao;
    InternationalTrade trade;

    cout << "Escolha uma opcao: " << endl
    << "1 - Converter a leitura do arquivo CSV para o formato binario." << endl
    << "2 - Acessar arquivo binario." << endl;
    cin >> opcao;
    switch (opcao) {
        case 1:
            trade.leituraArquivoCSV();
            break;
        case 2:
            trade.leituraArquivoBinario();
            break;
        default:
            cout << "Opção inválida!" << endl;
            break;
    }

    return 0;
}
