#include <iostream>
#include <string>

using namespace std;

struct RegistroTrade {
    int time_ref;
    string account;
    int code;
    string country_code;
    string product_type;
    double value;
    char status;
};

struct Noh {
    RegistroTrade dado;
    Noh* proximo;
};

class InternationalTrade {
    private:
        Noh* head;

    public:
        //construtor
        InternationalTrade(){
            head = nullptr;
        }
        //destrutor
        ~InternationalTrade() {
            Noh* atual = head;
            while (atual != nullptr) {
                Noh* temp = atual;
                atual = atual->proximo;
                delete temp;
        }
    }

    // Método para verificar se a lista está vazia
    bool Vazia() const {
        return head == nullptr;
    }

    // Método para inserir um elemento no início da lista
    void InserirInicio(const RegistroTrade& registro) {
        Noh* novoNoh = new Noh{registro, head};
        head = novoNoh;
    }

    // Método para remover o primeiro elemento da lista
    void RemoverInicio() {
        if (Vazia()) {
            cout << "A lista está vazia.\n";
            return;
        }
        Noh* temp = head;
        head = head->proximo;
        delete temp;
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
// Função principal para testar a lista
int main() {
    InternationalTrade lista;
    Interface();

    return 0;
}
