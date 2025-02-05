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

// Tamanho máximo de registros armazenados em cada bloco.
const int tamBloco = 50;

// Estrutura que representa um registro de trade.
struct RegistroTrade {
	int time_ref;               // Referência temporal do registro
	char account[50];           // Conta
	char code[10];              // Código do registro
	char country_code[10];      // Código do país
	char product_type[30];      // Tipo de produto
	float value;                // Valor associado
	char status[5];             // Status do registro

	// Método que exibe os dados do registro formatados
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

// Estrutura que representa um bloco de registros no SequenceSet.
struct Bloco {
	RegistroTrade registros[tamBloco]; // Vetor de registros
	int numRegistros;                  // Número de registros atualmente armazenados no bloco
	Bloco *proximo;                    // Ponteiro para o próximo bloco na lista encadeada

	// Construtor: inicializa o bloco sem registros e sem próximo bloco.
	Bloco() {
		numRegistros = 0;
		proximo = nullptr;
	}
};

// Classe que implementa o Sequence Set, que armazena os blocos com registros.
class SequenceSet {
private:
	Bloco *head;  // Ponteiro para o primeiro bloco

public:
	// Construtor: inicializa o SequenceSet vazio.
	SequenceSet() {
		head = nullptr;
	}

	// Destrutor: libera a memória dos blocos alocados.
	~SequenceSet() {
		Bloco *atual = head;
		while (atual) {
			Bloco *temp = atual;
			atual = atual->proximo;
			delete temp;
		}
	}

	// Retorna o ponteiro para o primeiro bloco.
	Bloco *getHead() const {
		return head;
	}

	// Função auxiliar Merge utilizada no MergeSort.
	// Recebe um vetor de registros e os índices de início, meio e fim, e intercala os subvetores ordenados.
	void Merge(RegistroTrade *registros, int inicio, int meio, int fim) {
		int n1 = meio - inicio + 1;
		int n2 = fim - meio;

		// Cria vetores temporários para a metade esquerda e direita.
		RegistroTrade *vetorEsquerda = new RegistroTrade[n1];
		RegistroTrade *vetorDireita = new RegistroTrade[n2];

		// Copia os elementos para os vetores temporários.
		for (int i = 0; i < n1; i++) {
			vetorEsquerda[i] = registros[inicio + i];
		}
		for (int i = 0; i < n2; i++) {
			vetorDireita[i] = registros[meio + 1 + i];
		}

		// Intercala os vetores, comparando o campo country_code.
		int i = 0, j = 0, k = inicio;
		while (i < n1 and j < n2) {
			if (string(vetorEsquerda[i].country_code) <= string(vetorDireita[j].country_code)) {
				registros[k++] = vetorEsquerda[i++];
			} else {
				registros[k++] = vetorDireita[j++];
			}
		}

		// Copia os elementos restantes (se houver) do vetor esquerdo.
		while (i < n1) {
			registros[k++] = vetorEsquerda[i++];
		}
		// Copia os elementos restantes (se houver) do vetor direito.
		while (j < n2) {
			registros[k++] = vetorDireita[j++];
		}

		// Libera a memória dos vetores temporários.
		delete[] vetorEsquerda;
		delete[] vetorDireita;
	}

	// Função de ordenação MergeSort que ordena um vetor de registros
	// com base no campo country_code.
	void MergeSort(RegistroTrade *registros, int inicio, int fim) {
		if (inicio < fim) {
			int meio = inicio + (fim - inicio) / 2;
			// Ordena recursivamente a metade esquerda.
			MergeSort(registros, inicio, meio);
			// Ordena recursivamente a metade direita.
			MergeSort(registros, meio + 1, fim);
			// Intercala as duas metades ordenadas.
			Merge(registros, inicio, meio, fim);
		}
	}

	// Função que ordena todos os registros do SequenceSet.
	// Ela primeiro copia os registros de todos os blocos para um vetor dinâmico,
	// ordena esse vetor usando MergeSort e depois redistribui os registros ordenados
	// de volta para os blocos.
	void OrdenarTodosRegistros(SequenceSet &sequenceSet) {
		int tamVetor = 10; // Tamanho inicial do vetor dinâmico
		int numRegistrosTotais = 0; // Contador de registros copiados
		RegistroTrade* todosRegistros = new RegistroTrade[tamVetor];
		Bloco *atual = sequenceSet.getHead();

		// Copia todos os registros de cada bloco para o vetor dinâmico
		while (atual) {
			for (int i = 0; i < atual->numRegistros; i++) {

				// Se o vetor estiver cheio, aumenta seu tamanho
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

		// Ordena o vetor de registros usando MergeSort
		MergeSort(todosRegistros, 0, numRegistrosTotais - 1);

		// Redistribui os registros ordenados nos blocos originais
		int aux = 0;
		atual = sequenceSet.getHead();
		while (atual and aux < numRegistrosTotais) {
			int numRegistrosParaEsteBloco;
			// Verifica se o bloco atual receberá menos registros que o tamanho máximo
			if (numRegistrosTotais - aux < tamBloco) {
				numRegistrosParaEsteBloco = numRegistrosTotais - aux;
			} else {
				numRegistrosParaEsteBloco = tamBloco;
			}

			for (int i = 0; i < numRegistrosParaEsteBloco; i++) {
				atual->registros[i] = todosRegistros[aux++];
			}
			atual->numRegistros = numRegistrosParaEsteBloco;

			// Se ainda houver registros a distribuir, avança para o próximo bloco.
			if (aux < numRegistrosTotais) {
				if (!atual->proximo) {
					atual->proximo = new Bloco();
				}
				atual = atual->proximo;
			}
		}
		delete[] todosRegistros;
	}

	// Insere um registro no SequenceSet.
	// A função encontra o bloco adequado (ainda com espaço) e insere o registro.
	// Em seguida, reordena todos os registros do SequenceSet.
	void InserirRegistro(const RegistroTrade &registro, SequenceSet &sequenceSet) {
		// Se não houver bloco inicial, cria um.
		if (!head) {
			head = new Bloco();
		}

		Bloco *atual = head;
		// Procura um bloco com espaço disponível (menor que tamBloco registros)
		while (atual->numRegistros >= tamBloco) {
			if (!atual->proximo) {
				atual->proximo = new Bloco();
			}
			atual = atual->proximo;
		}

		// Insere o registro no bloco encontrado e incrementa o contador de registros do bloco.
		atual->registros[atual->numRegistros++] = registro;

		// Reordena todos os registros no SequenceSet (garante a ordem pelo country_code)
		OrdenarTodosRegistros(sequenceSet);
	}

	// Exibe os dados de todos os blocos de forma detalhada.
	// Para cada bloco, exibe um cabeçalho e os registros contidos nele.
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

			// Exibe cada registro do bloco
			for (int i = 0; i < atual->numRegistros; i++) {
				atual->registros[i].MostrarDados();
			}

			cout << string(95, '-') << endl;

			atual = atual->proximo;
			numeroBloco++;
		}
	}

	// Retorna a quantidade de blocos armazenados no SequenceSet.
	int ContarBlocos() const {
		int cont = 0;
		Bloco *atual = head;
		while (atual) {
			cont++;
			atual = atual->proximo;
		}
		return cont;
	}

	// Salva o SequenceSet em um arquivo binário.
	// Para cada bloco, salva o número de registros e os registros armazenados.
	void SalvarEmArquivo(const string &nomeArquivo) const {
		ofstream arquivo(nomeArquivo, ios::binary);
		if (!arquivo) {
			cout << "Erro ao abrir o arquivo para salvar: " << nomeArquivo << endl;
			return;
		}

		Bloco *atual = head;
		while (atual) {
			// Salva o número de registros do bloco
			arquivo.write(reinterpret_cast<const char*>(&atual->numRegistros), sizeof(atual->numRegistros));
			// Salva os registros armazenados nesse bloco
			arquivo.write(reinterpret_cast<const char*>(atual->registros), sizeof(RegistroTrade) * atual->numRegistros);
			atual = atual->proximo;
		}

		arquivo.close();
	}

	// Carrega os dados do SequenceSet de um arquivo binário.
	// Lê cada bloco (número de registros + registros) e reconstrói a lista encadeada.
	void CarregarDeArquivo(const string &nomeArquivo) {
		ifstream arquivo(nomeArquivo, ios::binary);
		if (!arquivo) {
			cout << "Erro ao abrir o arquivo para carregar: " << nomeArquivo << endl;
			return;
		}

		// Libera os blocos existentes antes de carregar novos dados.
		while (head) {
			Bloco *temp = head;
			head = head->proximo;
			delete temp;
		}

		Bloco *atual = nullptr;
		int numRegistros;
		// Enquanto houver dados para ler, cria novos blocos e os adiciona à lista.
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

// Função para ler os dados de um arquivo CSV e inserir os registros no SequenceSet.
// Lê até 500 registros (ou até o fim do arquivo) e para cada linha monta um RegistroTrade.
void lerArquivoCSV(const string &nomeArquivo, SequenceSet &sequenceSet) {
	ifstream arquivoCSV(nomeArquivo);
	if (!arquivoCSV) {
		cout << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
		return;
	}
	
	string linha;
	char lixo1;
	int contador = 0;

	getline(arquivoCSV, linha); // Pula o cabeçalho do CSV

	while(getline(arquivoCSV, linha) and contador < 500) {
		RegistroTrade registro;
		// Lê os campos do registro (formato CSV)
		arquivoCSV >> registro.time_ref >> lixo1;
		arquivoCSV.getline(registro.account, 50, ','); 
		arquivoCSV.getline(registro.code, 10, ',');
		arquivoCSV.getline(registro.country_code, 10, ',');
		arquivoCSV.getline(registro.product_type, 30, ',');
		arquivoCSV >> registro.value >> lixo1;
		arquivoCSV.getline(registro.status, 5, '\n');
		
		// Insere o registro lido no SequenceSet
		sequenceSet.InserirRegistro(registro, sequenceSet);
		contador++;
	}
	arquivoCSV.close();
}

// Função para inserir registros a partir de um arquivo CSV informado pelo usuário.
void InsercaoViaArquivo(SequenceSet &sequenceSet) {
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
	// Lê os registros do CSV enquanto há dados
	while(arquivoCSV >> registro.time_ref) {
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
	// Salva o SequenceSet atualizado no arquivo binário
	sequenceSet.SalvarEmArquivo("blocos.bin");
	cout << "Arquivo inserido com sucesso!" << endl;
}

// Função que realiza busca binária por country_code em cada bloco.
// Percorre os blocos até encontrar um registro cujo country_code corresponda à chave procurada.
void BuscaBinariaCountryCode(const SequenceSet &sequenceSet, const string &country_code) {
	Bloco *atual = sequenceSet.getHead();
	bool encontrado = false;

	// Percorre cada bloco enquanto não encontra o registro procurado
	while (atual and !encontrado) { 
		int posInicial = 0;
		int posFinal = atual->numRegistros - 1;

		// Realiza a busca binária no bloco atual
		while (posInicial <= posFinal and !encontrado) {
			int meio = (posInicial + posFinal) / 2;

			if (string(atual->registros[meio].country_code) == country_code) {
				cout << "Registro encontrado: " << endl;
				atual->registros[meio].MostrarDados();
				encontrado = true;

				// Busca por possíveis registros iguais à esquerda do encontrado
				int esquerda = meio - 1;
				while (esquerda >= posInicial and string(atual->registros[esquerda].country_code) == country_code) {
					cout << "Registro encontrado: " << endl;
					atual->registros[esquerda].MostrarDados();
					esquerda--;
					encontrado = true;
				}

				// Busca por possíveis registros iguais à direita do encontrado
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

// Função para inserir um registro interativamente via teclado.
// Lê os dados digitados pelo usuário, monta um RegistroTrade, insere-o e atualiza o arquivo binário.
void InserirRegistro(SequenceSet &sequenceSet) {
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

	// Atualiza o arquivo binário com o registro inserido
	sequenceSet.SalvarEmArquivo("blocos.bin");
	cout << endl << "Registro inserido com sucesso!" << endl;
}

// Função que permite remover um registro específico com base no country_code.
// Lista os registros que possuem o country_code informado e permite ao usuário escolher qual excluir.
void RemoverRegistroEspecifico(SequenceSet &sequenceSet) {
	string country_code;
	cout << "Digite o Codigo de Pais que deseja excluir: ";
	cin >> country_code;

	Bloco *atual = sequenceSet.getHead();
	int registrosEncontrados[100];   // Armazena o índice dos registros encontrados em cada bloco
	Bloco *blocosEncontrados[100];     // Armazena o ponteiro para o bloco de cada registro encontrado
	int contador = 0;

	// Percorre os blocos para procurar registros com o country_code informado
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

	// Remove o registro escolhido deslocando os elementos posteriores
	Bloco *blocoEscolhido = blocosEncontrados[escolha - 1];
	int indiceEscolhido = registrosEncontrados[escolha - 1];

	for (int j = indiceEscolhido; j < blocoEscolhido->numRegistros - 1; j++) {
		blocoEscolhido->registros[j] = blocoEscolhido->registros[j + 1];
	}
	blocoEscolhido->numRegistros--;

	cout << "Registro excluido com sucesso!" << endl;

	// Atualiza o arquivo binário após a remoção
	sequenceSet.SalvarEmArquivo("blocos.bin");
}

// Função que implementa a interface de interação com o usuário.
// Exibe um menu e permite que o usuário escolha operações como busca, inserção, remoção e exibição dos registros.
void Interface(SequenceSet &sequenceSet) {
	int opcao;
	string countryCode;
	bool continuar = true;
	
	while (continuar) {
		cout << "-------------------------------------------------MENU----------------------------------------------------" << endl;
		cout << "Escolha o que deseja fazer:" << endl << endl;
		cout << "|Digite [1] para fazer uma busca." << "					" << "| Digite [4] para fazer uma remocao." << endl;
		cout << "|Digite [2] para fazer uma insercao." << "					" << "| Digite [5] para mostrar os dados." << endl;
		cout << "|Digite [3] para fazer uma insercao via arquivo texto." << "			" << "| Digite [0] para encerrar o programa." << endl;
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
			continuar = false;
			cout << "Encerrando programa..." << endl;
			break;

		default:
			cout << "Opcao invalida!" << endl;
			break;
		}
	}
}

// Função que verifica se um arquivo existe (neste caso, o arquivo binário).
bool verificacao(const char *nomeArquivo) {
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
	
	// Verifica se o arquivo binário já existe.
	// Se não existir, faz a exportação do CSV para o SequenceSet e salva em arquivo binário.
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
	} else {
		cout << "Arquivo binario ja existe!" << endl;
	}
	
	// Carrega o SequenceSet do arquivo binário.
	sequenceSet.CarregarDeArquivo("blocos.bin");
	if (sequenceSet.getHead() == nullptr){
		cout << "Erro ao ler o arquivo binario." << endl;
		return 0;
	}
	
	// Inicia a interface de interação com o usuário.
	Interface(sequenceSet);
	
	return 0;
}

