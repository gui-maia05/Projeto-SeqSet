#include <iostream>
#include <fstream>

using namespace std;

struct Trade
{
    int time_ref;
    string account;
    int code;
    string country_code;
    string product_type;
    float value;
    char status;
};

class InternationalTrade
{
    private:
    Trade* registrosTrade;
    int quantidadeTrades;

    public:
    InternationalTrade() : registrosTrade(NULL), quantidadeTrades(0){}

    ~InternationalTrade() 
    {
        delete[] registrosTrade;
        }

    void carregarDeArquivo();
    void adicionarTrade();
    void removerTrade();
    Trade* buscarTrade();
};


void InternationalTrade::carregarDeArquivo(){
    ifstream arquivo ("international-trade-june-2022-quarter-csv.csv");

    if (!arquivo.is_open()){
        cout << "Erro ao abrir o arquivo!" << endl;
        return;
    }




    arquivo.close();
}







int main(){





    return 0;
}