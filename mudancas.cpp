#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

const int tamBloco = 50; 

struct RegistroTrade {
    int time_ref;
    char account[30];
    char code[10];
    char country_code[5];
    char product_type[30];
    double value;
    char status[5];

    // Comparador para ordenação por time_ref
    bool operator<(const RegistroTrade &outro){
        return time_ref < outro.time_ref;
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

    // Adiciona registro ao bloco
    bool adicionarRegistro(const RegistroTrade &registro) {
        if (numRegistros < tamBloco) {
            registros[numRegistros++] = registro;
            sort(registros, registros + numRegistros); // Ordena o bloco após inserir
            return true;
        }
        return false; // Bloco cheio
    }

};

class SequenceSet {
    private:
        Bloco *head;

        // Encontra o bloco onde um registro deveria ser inserido
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

        // Divide um bloco em dois quando está cheio
        void dividirBloco(Bloco *bloco) {
        Bloco *novoBloco = new Bloco();
        int metade = tamBloco / 2;

        // Move metade dos registros para o novo bloco
        for (int i = metade; i < tamBloco; i++) {
            novoBloco->registros[novoBloco->numRegistros++] = bloco->registros[i];
        }

        bloco->numRegistros = metade; // Atualiza o número de registros no bloco original

        // Insere o novo bloco na sequência
        novoBloco->proximo = bloco->proximo;
        bloco->proximo = novoBloco;
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

        RegistroTrade* buscarRegistro(int time_ref) {
            Bloco *atual = head;
            while (atual) {
                for (int i = 0; i < atual->numRegistros; i++) {
                    if (atual->registros[i].time_ref == time_ref) {
                        return &atual->registros[i];
                    }
                }
                atual = atual->proximo;
            }
            return nullptr;
        }


        void leituraArquivoCSV() {
            string linha;
            char lixo;

            // Abre o arquivo CSV para leitura
            ifstream arquivoCSV("international-trade-june-2022-quarter-csv.csv");
            if (!arquivoCSV) {
                cout << "Erro ao abrir o arquivo CSV!" << endl;
                return;
            }

            getline(arquivoCSV, linha); // Pular o cabeçalho

            // Abre o arquivo binário para escrita
            ofstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
            if (!arquivoBinario) {
                cout << "Erro ao criar o arquivo binário!" << endl;
                arquivoCSV.close();
                return;
            }

            // Cria o bloco inicial
            Bloco* blocoAtual = new Bloco();
            int registrosBlocoAtual = 0;

            // Lê cada linha do arquivo CSV
            while (getline(arquivoCSV, linha)) {
                RegistroTrade novoRegistro;

                arquivoCSV >> novoRegistro.time_ref;
                arquivoCSV >> lixo; // Pular a vírgula
                arquivoCSV.getline(novoRegistro.account, 50, ',');
                arquivoCSV.getline(novoRegistro.code, 50, ',');
                arquivoCSV.getline(novoRegistro.country_code, 50, ',');
                arquivoCSV.getline(novoRegistro.product_type, 50, ',');
                arquivoCSV >> novoRegistro.value;
                if (arquivoCSV.fail()) {
                    novoRegistro.value = 0.0; // Se falhou, atribui 0.0
                    arquivoCSV.clear(); // Limpar o estado de falha
                    arquivoCSV >> lixo; 
                } else {
                    arquivoCSV >> lixo; 
                }
                arquivoCSV.getline(novoRegistro.status, 5);
                arquivoCSV.ignore();

                // Armazena o registro no bloco atual
                blocoAtual->registros[registrosBlocoAtual] = novoRegistro;
                registrosBlocoAtual++;

                // Se o bloco atingir o tamanho máximo, escreve no arquivo binário e reinicia o bloco
                if (registrosBlocoAtual >= tamBloco) {
                    // Escreve o bloco inteiro no arquivo binário
                    arquivoBinario.write((char*)blocoAtual, sizeof(Bloco));

                    // Resetar o bloco
                    registrosBlocoAtual = 0;
                    delete blocoAtual;
                    blocoAtual = new Bloco(); // Cria um novo bloco
                }
            }

            // Se restaram registros no último bloco (menor que 50), escreve o último bloco no arquivo binário
            if (registrosBlocoAtual > 0) {
                arquivoBinario.write((char*)blocoAtual, sizeof(Bloco));
            }

            // Fecha os arquivos
            arquivoCSV.close();
            arquivoBinario.close();

            cout << "Arquivo binário com blocos criado com sucesso!" << endl;
        }


        void leituraArquivoBinario() {
            ifstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
            if (!arquivoBinario) {
                cout << "Erro ao abrir o arquivo binário!" << endl;
                return;
            }

            // Determina o tamanho do arquivo
            arquivoBinario.seekg(0, ios::end);
            streamsize tamanhoArquivo = arquivoBinario.tellg();
            arquivoBinario.seekg(0, ios::beg);

            // Calcular o número de blocos
            int numBlocos = tamanhoArquivo / sizeof(Bloco);
            
            // Loop para ler os blocos do arquivo binário
            for (int i = 0; i < numBlocos; ++i) {
                Bloco blocoAtual;
                
                // Lê um bloco do arquivo binário
                arquivoBinario.read((char*)&blocoAtual, sizeof(Bloco));

                // Verifica se a leitura foi bem-sucedida
                if (arquivoBinario.fail()) {
                    cout << "Erro ao ler o bloco do arquivo binário!" << endl;
                    break;
                }
            }
            arquivoBinario.close();
        }


};


void interfacePrograma(){
    SequenceSet sequenceSet;

    ifstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
    if (arquivoBinario) {
        arquivoBinario.close();
        sequenceSet.leituraArquivoBinario();
    } else {
        cout << "Arquivo binario nao encontrado. Lendo arquivo CSV..." << endl;
        sequenceSet.leituraArquivoCSV();
        sequenceSet.leituraArquivoBinario();
    }

    int opcao;
    bool repetir = true;
    while(repetir){
        cout << "Escolha uma das opcoes abaixo:" << endl << endl;
        cout << "[1] Realizar uma busca" << endl;
        cout << "[2] Realizar uma insercao" << endl;
        cout << "[3] Realizar uma remocao" << endl;
        cout << "[-1] Encerrar o programa" << endl;
        cout << "Digite sua opcao: ";
        cin >> opcao;
        switch (opcao)
        {
        case 1:{
            int time_ref;
            cout << "Digite o time_ref para busca: ";
            cin >> time_ref;

            RegistroTrade* registroEncontrado = sequenceSet.buscarRegistro(time_ref);
            
            if (registroEncontrado) {
                cout << "Registro encontrado:" << endl;
                cout << "time_ref: " << registroEncontrado->time_ref << endl;
                cout << "Account: " << registroEncontrado->account << endl;
                cout << "Code: " << registroEncontrado->code << endl;
                cout << "Country Code: " << registroEncontrado->country_code << endl;
                cout << "Product Type: " << registroEncontrado->product_type << endl;
                cout << "Value: " << registroEncontrado->value << endl;
                cout << "Status: " << registroEncontrado->status << endl;
            } else {
                cout << "Registro com time_ref " << time_ref << " não encontrado." << endl;
            }
            break;
        }
        
        case 2:
            
            break;

        case 3:
            
            break;    
        
        case -1:
            repetir = false;
            cout << "Encerrando programa..." << endl;
            break;

        default:
            cout << "Opcao invalida!" << endl << endl;
            break;
        }
    }
}




int main() {

    interfacePrograma();

    return 0;
}