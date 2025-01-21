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
	char status[5];

	// Comparador para ordenação por time_ref
	bool operator<(const RegistroTrade &outro) {
		return time_ref < outro.time_ref;
	}
};

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
	int totalRegistros;
	int totalBlocos;
	Bloco *primeiroBloco;
	Bloco *ultimoBloco; 
};

class SequenceSet {
private:
	Bloco *head;
	Header header;
    int qtdTrades = 0;

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

	Bloco *EncontrarBloco(int chave) {
		Bloco *atual = head;
		while (atual != nullptr){
			if (chave >= atual->registros[0].time_ref and chave <= atual->registros[atual->numRegistros - 1].time_ref) {
				return atual;
			}
			atual = atual->proximo;
		}
		return nullptr;
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
        // Encontra a posição correta para inserir o registro
            int pos = 0;
            while (pos < bloco->numRegistros and bloco->registros[pos].time_ref < registro.time_ref) {
                pos++;
            }

            // Desloca os registros subsequentes para abrir espaço
            for (int i = bloco->numRegistros; i > pos; --i) {
                bloco->registros[i] = bloco->registros[i - 1];
            }

            // Insere o novo registro na posição correta
            bloco->registros[pos] = registro;

            // Incrementa o número de registros no bloco
            bloco->numRegistros++;
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

	void Remover(int chave) {
		// Localiza o bloco onde o registro pode estar
		Bloco *bloco = EncontrarBloco(chave);
		if (bloco == nullptr) {
			cout << "Registro não encontrado.\n";
			return;
		}

		// Localiza o índice do registro com a chave
		int indice = -1;
		for (int i = 0; i < bloco->numRegistros; ++i) {
			if (bloco->registros[i].time_ref == chave) {
				indice = i;
				i = bloco->numRegistros; // Termina o loop
			}
		}

		// Verifica se o registro foi encontrado
		if (indice == -1) {
			cout << "Registro não encontrado.\n";
			return;
		}

		// Remove o registro deslocando os elementos seguintes
		for (int i = indice; i < bloco->numRegistros - 1; ++i) {
			bloco->registros[i] = bloco->registros[i + 1];
		}
		bloco->numRegistros--;

		// Verifica se o bloco ficou vazio
		if (bloco->numRegistros == 0 and head != bloco) {
			// Encontra o bloco anterior para remover o bloco vazio
			Bloco *anterior = head;
			while (anterior->proximo != bloco) {
				anterior = anterior->proximo;
			}
			anterior->proximo = bloco->proximo;
			delete bloco;
		}
	}

    void LeituraArqCSV(SequenceSet &sequenceSet){
        ifstream arquivoCSV("international-trade-june-2022-quarter-csv.csv");
        if (!arquivoCSV) {
            cout << "Erro ao abrir o arquivo CSV!\n";
            return;
        }
        
		string linha;
        char lixo;
        getline(arquivoCSV, linha); //pular o cabeçalho

        while (arquivoCSV.peek() != EOF) {
			RegistroTrade registro;

			arquivoCSV >> registro.time_ref >> lixo; //le time_ref e pula a vírgula
            arquivoCSV.getline(registro.account, sizeof(registro.account), ',');
            arquivoCSV.getline(registro.code, sizeof(registro.code), ',');
            arquivoCSV.getline(registro.country_code, sizeof(registro.country_code), ',');
            arquivoCSV.getline(registro.product_type, sizeof(registro.product_type), ',');
            arquivoCSV >> registro.value >> lixo;
            if (arquivoCSV.fail()) {
                registro.value = 0.0; // Se falhou, atribui 0.0
                arquivoCSV.clear(); // Limpar o estado de falha
                arquivoCSV >> lixo; 
            } else {
                arquivoCSV >> lixo; 
            }
            arquivoCSV.getline(registro.status, 10);
            arquivoCSV.ignore();
            sequenceSet.Inserir(registro);
        }
        arquivoCSV.close();
    }

    bool Verificacao(const char *nomeArq){
        ifstream arquivoBinario(nomeArq);
        if (arquivoBinario.is_open()) {
            arquivoBinario.close();
			return true;
        }
        return false;
    }

	void ExportarParaBIN(){
		ofstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
		if (!arquivoBinario) {
			cout << "Erro ao abrir o arquivo binario para escrita!\n";
			return;
		}
		
		Bloco *atual = head;
		int totalBlocos = 0;

		while (atual != nullptr) {
			totalBlocos++;
			atual = atual->proximo;
		}

		arquivoBinario.write((char *)&totalBlocos, sizeof(totalBlocos));

		atual = head;		
		while (atual != nullptr) {
			arquivoBinario.write((char *)&atual->numRegistros, sizeof(atual->numRegistros));
			arquivoBinario.write((char *)atual->registros, sizeof(RegistroTrade) * atual->numRegistros);
			atual = atual->proximo;
		}

		arquivoBinario.close();
	}

	void ImportarDeBIN(){
		ifstream arquivoBinario("international-trade-june-2022-quarter-bin.bin", ios::binary);
		if (!arquivoBinario) {
			cout << "Erro ao abrir o arquivo binario para leitura!\n";
			return;
		}

		int totalBlocos;
		arquivoBinario.read((char *)&totalBlocos, sizeof(totalBlocos));

		for (int i = 0; i < totalBlocos; i++) {
			Bloco *novoBloco = new Bloco();
			arquivoBinario.read((char *)&novoBloco->numRegistros, sizeof(novoBloco->numRegistros));
			arquivoBinario.read((char *)novoBloco->registros, sizeof(RegistroTrade) * novoBloco->numRegistros);

			if (head == nullptr) {
				head = novoBloco;
			} else {
				Bloco *atual = head;
				while (atual->proximo != nullptr) {
					atual = atual->proximo;
				}
				atual->proximo = novoBloco;
			}
		}
		arquivoBinario.close();
	}
};

void InsercaoTerminal(SequenceSet &sequenceSet) {
    RegistroTrade novoRegistro;
    char lixo;

    cout << "Digite o time_ref (int): ";
    cin >> novoRegistro.time_ref;

    cout << "Digite a account (string): ";
    cin.ignore();
    cin.getline(novoRegistro.account, sizeof(novoRegistro.account));

    cout << "Digite o code (string): ";
    cin.getline(novoRegistro.code, sizeof(novoRegistro.code));

    cout << "Digite o country_code (string): ";
    cin.getline(novoRegistro.country_code, sizeof(novoRegistro.country_code));

    cout << "Digite o product_type (string): ";
    cin.getline(novoRegistro.product_type, sizeof(novoRegistro.product_type));

    cout << "Digite o value (double): ";
    cin >> novoRegistro.value;

    cout << "Digite o status (string): ";
    cin >> novoRegistro.status;

    sequenceSet.Inserir(novoRegistro);
    cout << "Registro inserido com sucesso!" << endl;
}

void RemocaoUsuario(SequenceSet &sequenceSet) {
    int chave;
    cout << "Digite o time_ref do registro que deseja remover: ";
    cin >> chave;

    sequenceSet.Remover(chave);
    cout << "Remocao concluida!\n";
}

void BuscaUsuario(SequenceSet &sequenceSet) {
    int chave;
    cout << "Digite o time_ref que deseja buscar: ";
    cin >> chave;

    Bloco *bloco = sequenceSet.EncontrarBloco(chave);
    if (bloco == nullptr) {
        cout << "Nenhum registro encontrado para o time_ref fornecido.\n";
        return;
    }

	bool encontrado = false;
    cout << "Registros encontrados:\n";
    for (int i = 0; i < bloco->numRegistros; i++) {
        if (bloco->registros[i].time_ref == chave) {
            cout << "time_ref: " << bloco->registros[i].time_ref << ", " << endl;
            cout << "account: " << bloco->registros[i].account << ", " << endl;
            cout << "code: " << bloco->registros[i].code << ", " << endl;
            cout << "country_code: " << bloco->registros[i].country_code << ", " << endl;
            cout << "product_type: " << bloco->registros[i].product_type << ", " << endl;
            cout << "value: " << bloco->registros[i].value << ", " << endl;
            cout << "status: " << bloco->registros[i].status << endl << endl;
			encontrado = true;
        }
    }

	if (!encontrado) {
		cout << "Nenhum registro encontrado para a chave fornecida.\n";
	}
}

void Interface(){
	SequenceSet sequenceSet;

	// Verifica se existe um arquivo binário previamente gerado
	if (sequenceSet.Verificacao("international-trade-june-2022-quarter-bin.bin")) {
		sequenceSet.ImportarDeBIN();
	} else {
		sequenceSet.LeituraArqCSV(sequenceSet);
		sequenceSet.ExportarParaBIN();
	}
	
	bool encontrado = true;
	while (encontrado){
		int opcao;
		cout << "-------------------------------------------------MENU----------------------------------------------------"<< endl;
		cout << "Escolha o que deseja fazer:" << endl << endl;
		cout << "Digite [1] para fazer uma busca." << "				" << "Digite [3] para fazer uma remocao." << endl;
		cout << "Digite [2] para fazer uma insercao." << "				" << "Digite [0] para encerrar o programa." << endl;
		cin >> opcao;
		cout << endl;

		switch (opcao) {
		case 1:
			BuscaUsuario(sequenceSet);
			break;
		case 2:
			InsercaoTerminal(sequenceSet);
			break;
		case 3:
			RemocaoUsuario(sequenceSet);
			break;
		case 0:
			encontrado = false;
			sequenceSet.ExportarParaBIN();
			cout << "Encerrando programa..." << endl;
			break;
		default:
            cout << "Erro!" << endl;
		break;
		}
	}
}

int main() {

	Interface();

	return 0;
}
