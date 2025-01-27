/*
 * Nome do Arquivo: projeto_sequence_set.cpp
 * Autores: Guilherme Casarino Maia e Luiz Phelipe Silva de Almeida
 * Data de Criação: 11/01/2025
 * Última Modificação: 26/01/2025
 * 
 * Descrição:
 * Este programa implementa a estrutura de dados Sequence Set em C++,
 * utilizando manipulação de arquivos binários e operações com CSV.
 * A estrutura armazena dados em blocos, onde cada bloco contém até
 * 50 registros.
 * 
 * Funcionalidades:
 * - Inserção, busca e remoção de registros.
 * - Armazenamento e manipulação de registros em arquivos binários.
 * - Exportação de dados para CSV.
 * - Suporte para grandes volumes de dados, com cerca de 2 milhões de registros.
 * 
 * Uso:
 * 1. Execute operações como busca, inserção ou remoção conforme necessário.
 * 2. Visualize e exporte dados após as operações.
 * 
 * Observações:
 * - Implementado para um sistema eficiente e otimizado para Linux.
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

using namespace std;

const int tamBloco = 50;

struct RegistroTrade {
	int time_ref;
	char account[50];
	char code[10];
	char country_code[10];
	char product_type[30];
	float value;
	char status[5];

	// Exibe os dados de um único registro
	void MostrarDados() const {
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
	SequenceSet() {
		head = nullptr;
	}

	~SequenceSet() {
		Bloco *atual = head;
		while (atual) {
			Bloco *temp = atual;
			atual = atual->proximo;
			delete temp;
		}
	}

	Bloco *getHead() const {
		return head;
	}

	void Merge(RegistroTrade *registros, int inicio, int meio, int fim) {
        int n1 = meio - inicio + 1;
        int n2 = fim - meio;

        RegistroTrade *vetorEsquerda = new RegistroTrade[n1];
        RegistroTrade *vetorDireita = new RegistroTrade[n2];

        for (int i = 0; i < n1; i++) {
            vetorEsquerda[i] = registros[inicio + i];
        }
        for (int i = 0; i < n2; i++) {
            vetorDireita[i] = registros[meio + 1 + i];
        }

        int i = 0, j = 0, k = inicio;
        while (i < n1 and j < n2) {
            if (string(vetorEsquerda[i].country_code) <= string(vetorDireita[j].country_code)) {
                registros[k++] = vetorEsquerda[i++];
            } else {
                registros[k++] = vetorDireita[j++];
            }
        }

        while (i < n1) {
            registros[k++] = vetorEsquerda[i++];
        }
        while (j < n2) {
            registros[k++] = vetorDireita[j++];
        }

        delete[] vetorEsquerda;
        delete[] vetorDireita;
    }

    void MergeSort(RegistroTrade *registros, int inicio, int fim) {
        if (inicio < fim) {
        
            int meio = inicio + (fim - inicio) / 2;

            MergeSort(registros, inicio, meio);
            MergeSort(registros, meio + 1, fim);

            Merge(registros, inicio, meio, fim);
        }
    }

    void OrdenarTodosRegistros(SequenceSet &sequenceSet) {
        int tamVetor = 10;
        int numRegistrosTotais = 0;
        RegistroTrade* todosRegistros = new RegistroTrade[tamVetor];
        Bloco *atual = sequenceSet.getHead();

        while (atual) {
            for (int i = 0; i < atual->numRegistros; i++) {

                if (numRegistrosTotais == tamVetor) {
                    tamVetor += 10;
                    RegistroTrade* temp = new RegistroTrade[tamVetor];
                        
                    for (int j = 0; j < numRegistrosTotais; j++) {
                        temp[j] = todosRegistros[j];
                    }

                    delete[] todosRegistros;
                    todosRegistros = temp;
                }

                todosRegistros[numRegistrosTotais++] = atual->registros[i];
            }
            atual = atual->proximo;
        }

        MergeSort(todosRegistros, 0, numRegistrosTotais - 1);

        int aux = 0;
        atual = sequenceSet.getHead();
        while (atual and aux < numRegistrosTotais) {
            int numRegistrosParaEsteBloco;
            if (numRegistrosTotais - aux < tamBloco) {
                numRegistrosParaEsteBloco = numRegistrosTotais - aux;
            } else {
                numRegistrosParaEsteBloco = tamBloco;
            }

            for (int i = 0; i < numRegistrosParaEsteBloco; i++) {
                atual->registros[i] = todosRegistros[aux++];
            }
            atual->numRegistros = numRegistrosParaEsteBloco;

            if (aux < numRegistrosTotais) {
                if (!atual->proximo) {
                    atual->proximo = new Bloco();
                }
                atual = atual->proximo;
            }
        }
        delete[] todosRegistros;
    }

	void InserirRegistro(const RegistroTrade &registro, SequenceSet &sequenceSet) {
		if (!head) {
			head = new Bloco();
		}

		Bloco *atual = head;

		while (atual->numRegistros >= tamBloco) {
			if (!atual->proximo) {
				atual->proximo = new Bloco();
			}
			atual = atual->proximo;
		}

		atual->registros[atual->numRegistros++] = registro;

		OrdenarTodosRegistros(sequenceSet);
	}

	void ExibirBlocosDetalhados() const {
		Bloco *atual = head;
		int numeroBloco = 1;

		while (atual) {
			cout << "\nBLOCO " << numeroBloco << endl << endl;
			cout << left << setw(9) << "TimeRef";
			cout << setw(11) << "\"Account\"";
			cout << setw(8) << "\"Code\"";
			cout << setw(15) << "\"CountryCode\"";
			cout << setw(15) << "\"ProductType\"";
			cout << setw(8) << "Value";
			cout << setw(10) << "\"Status\"" << endl;

			cout << string(95, '-') << endl;

			for (int i = 0; i < atual->numRegistros; i++) {
				atual->registros[i].MostrarDados();
			}

			cout << string(95, '-') << endl;

			atual = atual->proximo;
			numeroBloco++;
		}
	}

	int ContarBlocos() const {
		int cont = 0;
		Bloco *atual = head;
		while (atual) {
			cont++;
			atual = atual->proximo;
		}
		return cont;
	}

	void SalvarEmArquivo(const string &nomeArquivo) const {
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

	void CarregarDeArquivo(const string &nomeArquivo) {
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
		int numRegistros;
		while (arquivo.read(reinterpret_cast<char*>(&numRegistros), sizeof(numRegistros))) {
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
	char lixo1;
	int contador = 0;

	getline(arquivoCSV, linha); // Pular o cabeçalho

	while(getline(arquivoCSV, linha) and contador < 500){
		RegistroTrade registro;
		arquivoCSV >> registro.time_ref >> lixo1;
		arquivoCSV.getline(registro.account, 50, ','); 
		arquivoCSV.getline(registro.code, 10, ',');
		arquivoCSV.getline(registro.country_code, 10, ',');
		arquivoCSV.getline(registro.product_type, 30, ',');
		arquivoCSV >> registro.value >> lixo1;
		arquivoCSV.getline(registro.status, 5, '\n');
		
		sequenceSet.InserirRegistro(registro, sequenceSet);
		contador++;
	}
	arquivoCSV.close();
}

void InsercaoViaArquivo(SequenceSet &sequenceSet){
    string nomeArquivo;
    cout << "Digite o nome do arquivo CSV que deseja inserir: ";
    cin >> nomeArquivo;
    ifstream arquivoCSV(nomeArquivo);
    if (!arquivoCSV) {
        cout << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return;
    }

    string linha;
    char lixo1;
    int contador = 0;
    RegistroTrade registro;
    while(arquivoCSV >> registro.time_ref){
        arquivoCSV >> lixo1;
        arquivoCSV.getline(registro.account, 50, ','); 
        arquivoCSV.getline(registro.code, 10, ',');
        arquivoCSV.getline(registro.country_code, 10, ',');
        arquivoCSV.getline(registro.product_type, 30, ',');
        arquivoCSV >> registro.value >> lixo1;
        arquivoCSV.getline(registro.status, 5, '\n');

        sequenceSet.InserirRegistro(registro, sequenceSet);
        contador++;
    }
    arquivoCSV.close();
    sequenceSet.SalvarEmArquivo("blocos.bin");
    cout << "Arquivo inserido com sucesso!" << endl;
}

void BuscaBinariaCountryCode(const SequenceSet &sequenceSet, const string &country_code) {
    Bloco *atual = sequenceSet.getHead();
    bool encontrado = false;

    while (atual and !encontrado) { 
        int posInicial = 0;
        int posFinal = atual->numRegistros - 1;

        while (posInicial <= posFinal and !encontrado) {
            int meio = (posInicial + posFinal) / 2;

            if (string(atual->registros[meio].country_code) == country_code) {
                cout << "Registro encontrado: " << endl;
                atual->registros[meio].MostrarDados();
                encontrado = true;

                int esquerda = meio - 1;
                while (esquerda >= posInicial and string(atual->registros[esquerda].country_code) == country_code) {
                    cout << "Registro encontrado: " << endl;
                    atual->registros[esquerda].MostrarDados();
                    esquerda--;
                    encontrado = true;
                }

                int direita = meio + 1;
                while (direita <= posFinal and string(atual->registros[direita].country_code) == country_code) {
                    cout << "Registro encontrado: " << endl;
                    atual->registros[direita].MostrarDados();
                    direita++;
                    encontrado = true;
                }
            } else if (string(atual->registros[meio].country_code) < country_code) {
                posInicial = meio + 1;
            } else {
                posFinal = meio - 1;
            }
        }

        if (!encontrado) {
            atual = atual->proximo;
        }
    }

    if (!encontrado) {
        cout << "Nenhum registro encontrado com country_code '" << country_code << "'." << endl;
    }
}

void InserirRegistro(SequenceSet &sequenceSet){
	RegistroTrade registro;
	cout << "Digite os dados do registro a ser inserido:" << endl;
	cout << "Digite o time_ref: ";
	cin >> registro.time_ref;
	cout << "Digite o account: ";
	cin.ignore();
	cin.getline(registro.account, 50);
	cout << "Digite o code: ";
	cin.getline(registro.code, 10);
	cout << "Digite o country_code: ";
	cin.getline(registro.country_code, 10);
	cout << "Digite o product_type: ";
	cin.getline(registro.product_type, 30);
	cout << "Digite o value: ";
	cin >> registro.value;
	cout << "Digite o status: ";
	cin.ignore();
	cin.getline(registro.status, 5);

	sequenceSet.InserirRegistro(registro, sequenceSet);

	sequenceSet.SalvarEmArquivo("blocos.bin");
    cout << endl << "Registro inserido com sucesso!" << endl;
}

void RemoverRegistroEspecifico(SequenceSet &sequenceSet) {
    string country_code;
    cout << "Digite o Codigo de Pais que deseja excluir: ";
    cin >> country_code;

    Bloco *atual = sequenceSet.getHead();
    int registrosEncontrados[100];
    Bloco *blocosEncontrados[100];
    int contador = 0;

    while (atual) {
        for (int i = 0; i < atual->numRegistros; i++) {
            if (string(atual->registros[i].country_code) == country_code) {
                if (contador < 100) {
                    registrosEncontrados[contador] = i;
                    blocosEncontrados[contador] = atual;
                    contador++;

                    cout << "Registro encontrado [" << contador << "]: "
                         << atual->registros[i].time_ref << ", "
                         << atual->registros[i].account << ", "
                         << atual->registros[i].code << ", "
                         << atual->registros[i].country_code << ", "
                         << atual->registros[i].product_type << ", "
                         << atual->registros[i].value << ", "
                         << atual->registros[i].status 
                         << endl;
                }
            }
        }
        atual = atual->proximo;
    }

    if (contador == 0) {
        cout << "Nenhum registro encontrado com o Codigo de Pais '" << country_code << "'." << endl;
        return;
    }

    int escolha;
    cout << endl;
    cout << "Digite o numero do registro que deseja excluir (1 a " << contador << "): ";
    cin >> escolha;

    if (escolha < 1 or escolha > contador) {
        cout << "Escolha invalida. Operacao cancelada." << endl;
        return;
    }

    Bloco *blocoEscolhido = blocosEncontrados[escolha - 1];
    int indiceEscolhido = registrosEncontrados[escolha - 1];

    for (int j = indiceEscolhido; j < blocoEscolhido->numRegistros - 1; j++) {
        blocoEscolhido->registros[j] = blocoEscolhido->registros[j + 1];
    }
    blocoEscolhido->numRegistros--;

    cout << "Registro excluido com sucesso!" << endl;

    sequenceSet.SalvarEmArquivo("blocos.bin");
}

void Interface(SequenceSet &sequenceSet){
	int opcao;
	string countryCode;
	bool encontrado = true;
	
	while (encontrado){
		cout << "-------------------------------------------------MENU----------------------------------------------------"<< endl;
		cout << "Escolha o que deseja fazer:" << endl << endl;
		cout << "|Digite [1] para fazer uma busca." << "				" << "| Digite [4] para fazer uma remocao." << endl;
		cout << "|Digite [2] para fazer uma insercao." << "				" << "| Digite [5] para mostrar os dados." << endl;
		cout << "|Digite [3] para fazer uma insercao via arquivo texto." << "		" << "| Digite [0] para encerrar o programa." << endl;
		cin >> opcao;
		cout << endl;

		switch (opcao) {
		case 1:
			cout << "Digite o codigo do pais que deseja buscar: ";
			cin >> countryCode;
			BuscaBinariaCountryCode(sequenceSet, countryCode);
			break;

		case 2:
			InserirRegistro(sequenceSet);
			break;

		case 3:
			InsercaoViaArquivo(sequenceSet);
			break;

		case 4:
			RemoverRegistroEspecifico(sequenceSet);
			break;

        case 5:
            sequenceSet.ExibirBlocosDetalhados();
			cout << "Total de blocos carregados: " << sequenceSet.ContarBlocos() << endl;
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

bool verificacao(const char *nomeArquivo){
	ifstream arquivoBinario(nomeArquivo);
	if (arquivoBinario.is_open()){
		arquivoBinario.close();
		return true;
	}
	return false;
}

int main() {
	SequenceSet sequenceSet;
	const char* nomeBin = "blocos.bin";
	
	if (!verificacao(nomeBin)){
		cout << "Arquivo binario nao encontrado. Fazendo exportacao!" << endl;
		ifstream arquivoCsv("international-trade-june-2022-quarter-csv.csv");
		if (!arquivoCsv){
			cout << "Erro ao abrir o arquivo CSV." << endl;
			return 0;
		}
		lerArquivoCSV("international-trade-june-2022-quarter-csv.csv", sequenceSet);
		sequenceSet.SalvarEmArquivo(nomeBin);
		arquivoCsv.close();
		cout << "Arquivo exportado com sucesso!" << endl;
	}else {
		cout << "Arquivo binario ja existe!" << endl;
	}
	
    sequenceSet.CarregarDeArquivo("blocos.bin");
	if (sequenceSet.getHead() == nullptr){
		cout << "Erro ao ler o arquivo binario." << endl;
		return 0;
	}
	
	Interface(sequenceSet);
	
	return 0;
}
