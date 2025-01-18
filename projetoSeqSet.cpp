#include <iostream>
#include <fstream>

using namespace std;

const int Max = 100; //Tamanho máximo de registros por bloco

struct RegistroTrade {
    int time_ref;
    char account[30];
    char code[10];
    char country_code[5];
    char product_type[30];
    double value;
    char status;

    // Comparador para ordenação por time_ref
    bool operator<(const RegistroTrade &outro){
        return time_ref < outro.time_ref;
    }

};

class InternationalTrade {
private:
    int qtdTrades = 0;

public:
    InternationalTrade() {
        qtdTrades = 0;
    }

    ~InternationalTrade() {}

    void leituraArquivoCSV();
    void transformarEmBinario(RegistroTrade* dados, int qtdTrades);
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
        qtdTrades++;
    }

    arquivoCSV.clear();
    arquivoCSV.seekg(0, ios::beg);
    getline(arquivoCSV, linha);
    
    RegistroTrade* dados = new RegistroTrade[qtdTrades];
    
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

    transformarEmBinario(dados, qtdTrades);

    cout << "Arquivo binario criado com sucesso!" << endl;

    delete[] dados;
}

void InternationalTrade::transformarEmBinario(RegistroTrade* dados, int qtdTrades) {
    ofstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
    arquivoBinario.write((char*)dados, sizeof(RegistroTrade) * qtdTrades);
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

    qtdTrades = tamanhoArquivo / sizeof(RegistroTrade);

    RegistroTrade* dados = new RegistroTrade[qtdTrades];
    
    arquivoBinario.read((char*)dados, tamanhoArquivo);
    
    arquivoBinario.close();

    delete[] dados;
}

struct Bloco {
    RegistroTrade registros[Max];
    int numRegistros;
    Bloco *proximo;

    Bloco() {
        numRegistros = 0;
        proximo = nullptr;
    }
};

struct Header {
    int totalRegistros; // Número total de registros no Sequence Set
    int totalBlocos;    // Número de blocos na estrutura
    Bloco *primeiroBloco; // Referência ao primeiro bloco
    Bloco *ultimoBloco;   // Referência ao último bloco
};

class SequenceSet {
    private:
        Bloco *head;

        Bloco *EncontrarBloco(int chave) {
            Bloco *atual = head;
            while (atual != nullptr){
                if (atual->numRegistros > 0 and chave <= atual->registros[atual->numRegistros - 1].time_ref) {
                    return atual;
                }
                atual = atual->proximo;
            }
            return nullptr;
        }

    public:
        //construtor
        SequenceSet() {
            head = nullptr;
        }
        //destrutor
        ~SequenceSet() {
            while (head != nullptr) {
                Bloco *temp = head;
                head = head->proximo;
                delete temp;
        }
    }

    // Inserir um registro
    void Inserir(const RegistroTrade &registro) {
        if (head == nullptr) {
            head = new Bloco();
        }

        Bloco *bloco = EncontrarBloco(registro.time_ref);

        if (bloco == nullptr) {
            bloco = new Bloco();
            bloco->proximo = head;
            head = bloco;
        }

        if (bloco->numRegistros < Max) {
            bloco->registros[bloco->numRegistros++] = registro;
            sort(bloco->registros, bloco->registros + bloco->numRegistros);
        } else {
            // Overflow - criar novo bloco
            Bloco *novoBloco = new Bloco();
            for (int i = Max / 2; i < Max; ++i) {
                novoBloco->registros[novoBloco->numRegistros++] = bloco->registros[i];
            }
            bloco->numRegistros = Max / 2;

            novoBloco->proximo = bloco->proximo;
            bloco->proximo = novoBloco;

            if (registro.time_ref <= bloco->registros[bloco->numRegistros - 1].time_ref) {
                Inserir(registro);
            } else {
                Inserir(registro);
            }
        }
    }

    // Remover um registro
    void Remover(int chave) {
        Bloco *bloco = EncontrarBloco(chave);
        if (bloco == nullptr) {
            cout << "Registro não encontrado.\n";
            return;
        }

        auto it = find_if(bloco->registros, bloco->registros + bloco->numRegistros, [&](const RegistroTrade& r) {
            return r.time_ref == chave;
        });

        if (it != bloco->registros + bloco->numRegistros) {
            for (int i = distance(bloco->registros, it); i < bloco->numRegistros - 1; ++i) {
                bloco->registros[i] = bloco->registros[i + 1];
            }
            bloco->numRegistros--;
        }

        if (bloco->numRegistros == 0 && head != bloco) {
            Bloco* anterior = head;
            while (anterior->proximo != bloco) {
                anterior = anterior->proximo;
            }
            anterior->proximo = bloco->proximo;
            delete bloco;
        }
    }

    // Método para exibir os elementos da lista
    /*void Exibir() const {
        if (Vazia()) {
            cout << "A lista está vazia.\n";
            return;
        }
        Noh* atual = head;
        while (atual != nullptr) {
            cout << atual->dado << " -> ";
            atual = atual->proximo;
        }
        cout << "NULL\n";
    }*/
   
};



void Interface(){
    bool encontrado = true;
    while (encontrado){
        int opcao;//da opcoes de escolha para o usuario.
		cout << "-------------------------------------------------MENU----------------------------------------------------"<< endl;
		cout << "Escolha o que deseja fazer:" << endl << endl;
		cout << "Digite [1] para fazer uma busca." << "				" << "Digite [3] para fazer uma remocao." << endl;
		cout << "Digite [2] para fazer uma insercao." << "				" << "Digite [0] para encerrar o programa." << endl;
		cin >> opcao;
		cout << endl;

        /*switch (opcao == 1) {
        case 1:
            // code
            break;
        default:
            break;
        }
        switch (opcao == 2) {
        case 2:
            // code
            break;
        default:
            break;
        }
        switch (opcao == 3) {
        case 3:
            // code
            break;
        default:
            break;
        }
        switch (opcao == 0) {
        case 0:
            // code
            break;
        default:
            break;
        }*/
    }
}

// Função principal
int main() {

    Interface();

    return 0;
}
