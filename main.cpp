#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct CacheLine {
    bool valid = false;
    unsigned int tag = 0;
};

void simularCache(string trace_file, int capacidade, int tamanho_bloco) {
    
    int num_linhas = capacidade / tamanho_bloco;
    int offset_bits = log2(tamanho_bloco);
    int index_bits = log2(num_linhas);
    
    unsigned int index_mask = num_linhas - 1;

    vector<CacheLine> cache(num_linhas);

    ifstream file(trace_file);
    if (!file.is_open()) {
        cout << "Arquivo " << trace_file << " nao encontrado" << endl;
        return;
    }

    int hits = 0;
    int misses = 0;
    unsigned int endereco;

    while (file >> endereco) {
        
        // Aplica o deslocamento usando a variavel dinamica 'offset_bits'
        unsigned int endereco_bloco = endereco >> offset_bits;
        
        // Aplica a mascara dinamica 'index_mask'
        unsigned int indice = endereco_bloco & index_mask;
        
        // Pega a Tag empurrando pro lado a quantidade dinamica de 'index_bits'
        unsigned int tag = endereco_bloco >> index_bits;

        // Checagem de Hit e Miss (a logica e sempre identica!)
        if (cache[indice].valid == true && cache[indice].tag == tag) {
            hits++;
        } else {
            misses++;
            cache[indice].valid = true;
            cache[indice].tag = tag;
        }
    }
    file.close();

    int total = hits + misses;
    if (total == 0) return;
    
    float hit_rate = ((float)hits / total) * 100.0f;
    float miss_rate = ((float)misses / total) * 100.0f;

    cout << "Cache " << capacidade << "W, Bloco " << tamanho_bloco << "W | ";
    cout << "Hits: " << hit_rate << "% | Misses: " << miss_rate << "%" << endl;
}

int main() {
    cout << "========= RESULTADOS TRACE 1 (Acesso por Linha) =========" << endl;
    simularCache("trace_address1.dat", 128, 16); // Tarefa 1
    simularCache("trace_address1.dat", 128, 32); // Tarefa 2
    simularCache("trace_address1.dat", 256, 16); // Tarefa 3
    simularCache("trace_address1.dat", 256, 32); // Tarefa 4
    simularCache("trace_address1.dat", 512, 16); // Tarefa 5
    simularCache("trace_address1.dat", 512, 32); // Tarefa 6
    simularCache("trace_address1.dat", 512, 64); // Tarefa 7
    
    cout << "\n========= RESULTADOS TRACE 2 (Acesso por Coluna) =========" << endl;
    simularCache("trace_address2.dat", 128, 16); // Tarefa 1
    simularCache("trace_address2.dat", 128, 32); // Tarefa 2
    simularCache("trace_address2.dat", 256, 16); // Tarefa 3
    simularCache("trace_address2.dat", 256, 32); // Tarefa 4
    simularCache("trace_address2.dat", 512, 16); // Tarefa 5
    simularCache("trace_address2.dat", 512, 32); // Tarefa 6
    simularCache("trace_address2.dat", 512, 64); // Tarefa 7

    return 0;
}
