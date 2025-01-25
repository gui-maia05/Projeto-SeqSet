#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

const int MAX_REGISTROS = 50;

struct Registro {
    int timeRef;
    char account[50];
    char code[10];
    char countryCode[10];
    char productType[30];
    double value;
    char status[20];

    Registro() : timeRef(0), account(""), code(""), countryCode(""), productType(""), value(0), status("") {}

    void MostrarDados() const {
        // Exibe os dados de um único registro
        cout << left << setw(10) << timeRef; 
        cout << setw(13) << account;
        cout << setw(10) << code;
        cout << setw(8) << countryCode;
        cout << setw(10) << productType;
        cout << setw(15) << fixed << setprecision(2) << value;
        cout << setw(10) << status << endl;
    }
};

struct Bloco {
    Registro registros[MAX_REGISTROS];
    int qtdRegistros;
    Bloco* prox;

    Bloco() : qtdRegistros(0), prox(nullptr) {}
};

class SequenceSet {
private:
    Bloco* head;

public:
    SequenceSet() : head(nullptr) {}

    ~SequenceSet() {
        Bloco* atual = head;
        while (atual) {
            Bloco* temp = atual;
            atual = atual->prox;
            delete temp;
        }
    }

    void InserirRegistro(const Registro& registro) {
        if (!head) {
            head = new Bloco();
        }

        Bloco* atual = head;
        while (true) {
            if (atual->qtdRegistros < MAX_REGISTROS) {
                atual->registros[atual->qtdRegistros++] = registro;
                break;
            }

            if (!atual->prox) {
                atual->prox = new Bloco();
            }
            atual = atual->prox;
        }
    }

    void MostrarDados() const {
        Bloco* atual = head;
        
        // Cabeçalho da tabela
        cout << left << setw(10) << "Time Ref";
        cout << setw(13) << "Account";
        cout << setw(10) << "Code";
        cout << setw(8) << "Country Code";
        cout << setw(10) << "Product Type";
        cout << setw(15) << "Value";
        cout << setw(10) << "Status" << endl;
        cout << string(95, '-') << endl;

        // Percorrer todos os blocos e registros
        while (atual) {
            for (int i = 0; i < atual->qtdRegistros; i++) {
                const Registro& record = atual->registros[i];
                cout << left << setw(10) << record.timeRef;
                cout << setw(13) << record.account;
                cout << setw(10) << record.code;
                cout << setw(8) << record.countryCode;
                cout << setw(10) << record.productType;
                cout << setw(15) << fixed << setprecision(2) << record.value;
                cout << setw(10) << record.status << endl;
            }
            atual = atual->prox;
        }
    }

    void ExibirTodos() const {
        Bloco* atual = head;
        int numeroBloco = 1;

        while (atual) {
            cout << "\nBLOCO " << numeroBloco << endl << endl;

            // Cabeçalho da tabela para cada bloco
            cout << left << setw(10) << "TimeRef";
            cout << setw(11) << "Account";
            cout << setw(8) << "Code";
            cout << setw(13) << "CountryCode";
            cout << setw(13) << "ProductType";
            cout << setw(8) << "Value";
            cout << setw(10) << "Status" << endl;

            cout << string(95, '-') << endl;

            for (int i = 0; i < atual->qtdRegistros; i++) {
                atual->registros[i].MostrarDados();
            }

            cout << string(95, '-') << endl;

            atual = atual->prox;
            numeroBloco++;
        }
    }

    int ContarBlocos() const {
        int count = 0;
        Bloco* atual = head;
        while (atual) {
            count++;
            atual = atual->prox;
        }
        return count;
    }

    void SalvarEmArquivo(const string& nomeArquivo) const {
        ofstream file(nomeArquivo, ios::binary);
        if (!file) {
            cout << "Erro ao abrir o arquivo para salvar: " << nomeArquivo << endl;
            return;
        }

        Bloco* atual = head;
        while (atual) {
            // Escreve o número de registros no bloco
            file.write(reinterpret_cast<const char*>(&atual->qtdRegistros), sizeof(atual->qtdRegistros));

            // Escreve todos os registros do bloco
            file.write(reinterpret_cast<const char*>(atual->registros), sizeof(Registro) * atual->qtdRegistros);

            atual = atual->prox;
        }

        file.close();
        cout << "Blocos salvos no arquivo: " << nomeArquivo << endl;
    }

    void CarregarDeArquivo(const string& nomeArquivo) {
        ifstream file(nomeArquivo, ios::binary);
        if (!file) {
            cout << "Erro ao abrir o arquivo para carregar: " << nomeArquivo << endl;
            return;
        }

        // Limpa qualquer estrutura existente
        while (head) {
            Bloco* temp = head;
            head = head->prox;
            delete temp;
        }

        Bloco* atual = nullptr;
        while (true) {
            int qtdRegistros;
            file.read(reinterpret_cast<char*>(&qtdRegistros), sizeof(qtdRegistros));
            if (file.eof()) break; // Se chegou ao fim do arquivo, termina

            Bloco* novoBloco = new Bloco();
            novoBloco->qtdRegistros = qtdRegistros;
            file.read(reinterpret_cast<char*>(novoBloco->registros), sizeof(Registro) * qtdRegistros);

            if (!head) {
                head = novoBloco;
                atual = head;
            } else {
                atual->prox = novoBloco;
                atual = novoBloco;
            }
        }

        file.close();
    }
};

void lerCSVEInserir(const string& nomeArquivo, SequenceSet& sequenceSet) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return;
    }

    string linha;
    int cont = 0;

    while (getline(arquivo, linha) and cont < 500) {
        stringstream ss(linha);
        string timeRefStr, account, code, countryCode, productType, valueStr, status;

        if (!getline(ss, timeRefStr, ',') || 
            !getline(ss, account, ',') || 
            !getline(ss, code, ',') || 
            !getline(ss, countryCode, ',') || 
            !getline(ss, productType, ',') || 
            !getline(ss, valueStr, ',') ||
            !getline(ss, status, ',')) {
            cout << "Erro: Linha com formato inválido: " << linha << endl;
            continue;
        }

        if (timeRefStr.empty() || account.empty() || code.empty() || countryCode.empty() || 
            productType.empty() || valueStr.empty() || status.empty()) {
            cout << "Erro: Campo vazio encontrado na linha: " << linha << endl;
            continue;
        }

        Registro registro;
        try {
            registro.timeRef = stoi(timeRefStr);
            strcpy(registro.account, account.c_str());
            strcpy(registro.code, code.c_str());
            strcpy(registro.countryCode, countryCode.c_str());
            strcpy(registro.productType, productType.c_str());
            registro.value = stod(valueStr);
            strcpy(registro.status, status.c_str());
        } catch (const invalid_argument& e) {
            cout << "Erro ao converter valores na linha: " << linha << endl;
            continue;
        }

        sequenceSet.InserirRegistro(registro);
        cont++;
    }

    arquivo.close();
}

void Interface(){
    SequenceSet sequenceSet;
    
    // Ler o arquivo CSV e preencher o Sequence Set
    lerCSVEInserir("international-trade-june-2022-quarter-csv.csv", sequenceSet);

    // Salvar os blocos em um arquivo
    sequenceSet.SalvarEmArquivo("blocos.dat");

	int opcao;
	bool encontrado = true;
	while (encontrado){
		cout << "-------------------------------------------------MENU----------------------------------------------------"<< endl;
		cout << "Escolha o que deseja fazer:" << endl << endl;
		cout << "Digite [1] para fazer uma busca." << "				" << "Digite [4] para mostrar os dados." << endl;
		cout << "Digite [2] para fazer uma insercao." << "				" << "Digite [0] para encerrar o programa." << endl;
        cout << "Digite [3] para fazer uma remocao." << endl;
		cin >> opcao;
		cout << endl;

		switch (opcao) {
		case 1:
			cout << "em desenvolvimento..." << endl;
			break;
		case 2:
			cout << "em desenvolvimento..." << endl;
			break;
		case 3:
			cout << "em desenvolvimento..." << endl;
			break;
        case 4: {
            // Limpar e carregar os blocos do arquivo
            SequenceSet loadedSequenceSet;
            loadedSequenceSet.CarregarDeArquivo("blocos.dat");
            loadedSequenceSet.ExibirTodos();
            // Exibir a quantidade de blocos carregados
            cout << "Total de blocos carregados: " << loadedSequenceSet.ContarBlocos() << endl;
            break;
        }
		case 0:
			encontrado = false;
			cout << "Encerrando programa..." << endl;
			break;

		default:
            cout << "Opcao invalida!" << endl;
		break;
		}
	}
}

int main() {
    Interface();
    
    return 0;
}
