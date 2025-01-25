#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <iomanip>

using namespace std;

const int tamBloco = 50;

struct RegistroTrade {
    int time_ref;
    char account[50];
    char code[10];
    char country_code[5];
    char product_type[30];
    double value;
    char status[5];

    void display() const {
        // Exibe os dados de um único registro
        cout << left << setw(10) << time_ref; 
        cout << setw(13) << account;
        cout << setw(10) << code;
        cout << setw(8) << country_code;
        cout << setw(10) << product_type;
        cout << setw(15) << fixed << setprecision(2) << value;
        cout << setw(10) << status << endl;
    }

};

struct Bloco {
    RegistroTrade registros[tamBloco];
    int numRegistros;
    Bloco *proximo;

    Bloco() {
        numRegistros = 0;
        proximo = nullptr;
    }
};

class SequenceSet {
    private:
        Bloco *head;

    public:

        // Construtor
        SequenceSet() {
            head = nullptr;
        }

        // Destrutor
        ~SequenceSet() {
            Bloco *atual = head;
            while (atual) {
                Bloco *temp = atual;
                atual = atual->proximo;
                delete temp;
            }
        }

        void inserirRegistro(const RegistroTrade &registro) {
            if (!head) {
                head = new Bloco();
            }

            Bloco *atual = head;

            while (true) {
                if (atual->numRegistros < tamBloco) {
                    atual->registros[atual->numRegistros++] = registro;
                    break;
                }

                if (!atual->proximo) {
                    atual->proximo = new Bloco();
                }
                atual = atual->proximo;     
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
                for (int i = 0; i < atual->numRegistros; i++) {
                    const RegistroTrade& record = atual->registros[i];
                    cout << left << setw(10) << record.time_ref;
                    cout << setw(13) << record.account;
                    cout << setw(10) << record.code;
                    cout << setw(8) << record.country_code;
                    cout << setw(10) << record.product_type;
                    cout << setw(15) << fixed << setprecision(2) << record.value;
                    cout << setw(10) << record.status << endl;
                }
                atual = atual->proximo;
            }
        }

        void exibirTodos() const {
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

                for (int i = 0; i < atual->numRegistros; i++) {
                    atual->registros[i].display();
                }

                cout << string(95, '-') << endl;

                atual = atual->proximo;
                numeroBloco++;
            }
        }

        int contarBlocos() const{
            int count = 0;
            Bloco *atual = head;
            while (atual) {
                count++;
                atual = atual->proximo;
            }
            return count;
        }

        void salvarEmArquivo(const string &nomeArquivo) const {
            ofstream arquivo(nomeArquivo, ios::binary);
            if (!arquivo) {
                cout << "Erro ao abrir o arquivo para salvar: " << nomeArquivo << endl;
                return;
            }

            Bloco *atual = head;
            while (atual) {
                arquivo.write(reinterpret_cast<const char*>(&atual->numRegistros), sizeof(atual->numRegistros));
                arquivo.write(reinterpret_cast<const char*>(atual->registros), sizeof(RegistroTrade) * atual->numRegistros);
                atual = atual->proximo;
            }

            arquivo.close();
        }

        void carregarDeArquivo(const string &nomeArquivo) {
            ifstream arquivo(nomeArquivo, ios::binary);
            if (!arquivo) {
                cout << "Erro ao abrir o arquivo para carregar: " << nomeArquivo << endl;
                return;
            }

            while (head) {
                Bloco *temp = head;
                head = head->proximo;
                delete temp;
            }

            Bloco *atual = nullptr;
            while (true) {
                int numRegistros;
                arquivo.read(reinterpret_cast<char*>(&numRegistros), sizeof(numRegistros));
                if (arquivo.eof()) break;

                Bloco *novoBloco = new Bloco();
                novoBloco->numRegistros = numRegistros;
                arquivo.read(reinterpret_cast<char*>(novoBloco->registros), sizeof(RegistroTrade) * numRegistros);

                if (!head) {
                    head = novoBloco;
                    atual = head;
                } else {
                    atual->proximo = novoBloco;
                    atual = novoBloco;
                }
            }

            arquivo.close();
        }
};

void lerArquivoCSV(const string &nomeArquivo, SequenceSet &sequenceSet){
    ifstream arquivoCSV(nomeArquivo);
    if (!arquivoCSV) {
        cout << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return;
    }
    
    string linha;
    int contador = 0;

    getline(arquivoCSV, linha); // Pular o cabeçalho

    while(getline(arquivoCSV, linha) and contador < 500){
        stringstream ss(linha);
        string time_refStr, account, code, country_code, product_type, valueStr, status;

        if(!getline(ss, time_refStr, ',') ||
           !getline(ss, account, ',') ||
           !getline(ss, code, ',') ||
           !getline(ss, country_code, ',') ||
           !getline(ss, product_type, ',') ||
           !getline(ss, valueStr, ',') ||
           !getline(ss, status, ',')){
            cout << "Erro: Linha com formato inválido: " << linha << endl;
            continue;
        }

        if(time_refStr.empty() || account.empty() || code.empty() || country_code.empty() || product_type.empty() || valueStr.empty() || status.empty()){
            cout << "Erro: Linha com formato inválido: " << linha << endl;
            continue;
        }

        RegistroTrade novoRegistro;
        try {
            novoRegistro.time_ref = stoi(time_refStr);
            strcpy(novoRegistro.account, account.c_str());
            strcpy(novoRegistro.code, code.c_str());
            strcpy(novoRegistro.country_code, country_code.c_str());
            strcpy(novoRegistro.product_type, product_type.c_str());
            novoRegistro.value = stod(valueStr);
            strcpy(novoRegistro.status, status.c_str());
        } catch (const exception &e) {
            cout << "Erro: Linha com formato inválido: " << linha << endl;
            continue;
        }

        sequenceSet.inserirRegistro(novoRegistro);
        contador++;
    }
    arquivoCSV.close();
}

void Interface(){
    SequenceSet sequenceSet;

    lerArquivoCSV("international-trade-june-2022-quarter-csv.csv", sequenceSet);
    sequenceSet.salvarEmArquivo("blocos.dat");

    SequenceSet loadedSequenceSet;
    loadedSequenceSet.carregarDeArquivo("blocos.dat");

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

        case 4:
            loadedSequenceSet.exibirTodos();
            cout << "Total de blocos carregados: " << loadedSequenceSet.contarBlocos() << endl;
            break;

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
