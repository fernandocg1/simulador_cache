#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct CacheLine {
    bool valid = false;
    unsigned int tag = 0;
    int lru_time = 0; //usado na politica lru para saber qual foi a linha mais recentemente usada
    // no caso trocamos o lru_time para um inteiro que vai ser incrementado a cada acesso, e a linha que tiver o menor valor de lru_time é a que foi usada ha mais tempo
};

void simularCache(string trace_file, int capacidade, int tamanho_bloco) {
    
    int num_linhas = capacidade / tamanho_bloco; //quantidade de linhas na cache
    int offset_bits = log2(tamanho_bloco); //quantidade de bits para o deslocamento, feito com log2 do tamanho do bloco
    int index_bits = log2(num_linhas); // quantidade de bits para saber a linha da cache
    
    unsigned int index_mask = num_linhas - 1; // aqui é massa porque a quantidade de linhas é sempre potencia de 2, então a mascara vai ser sempre 2^n - 1
    //ela serve para pegar os bits do meio do endereço, que são os bits de index

    vector<CacheLine> cache(num_linhas);

    //abrindo o arquivo de trace
    ifstream file(trace_file);
    if (!file.is_open()) {
        cout << "Arquivo " << trace_file << " nao encontrado" << endl;
        return;
    }
    //variaveis para contagem de hits e misses
    int hits = 0;
    int misses = 0;
    unsigned int endereco;

    while (file >> endereco) {
        //aqui é feito o deslocamento do endereço para pegar o bloco, e depois é feito a mascara para pegar o index, e por fim é feito o deslocamento para pegar a tag, 
        //a tag é o que sobra do endereço depois de pegar o bloco e o index
        unsigned int endereco_bloco = endereco >> offset_bits;
        
        unsigned int indice = endereco_bloco & index_mask;
        
        unsigned int tag = endereco_bloco >> index_bits;

        // checa os hits e os misses
        if (cache[indice].valid == true && cache[indice].tag == tag) {
            hits++;
        } else {
            misses++;
            cache[indice].valid = true;
            cache[indice].tag = tag;
        }
    }
    file.close();

    //exibindo os resultados
    int total = hits + misses;
    if (total == 0) return;
    
    float hit_rate = ((float)hits / total) * 100.0f;
    float miss_rate = ((float)misses / total) * 100.0f;

    cout << "Cache " << capacidade << "W, Bloco " << tamanho_bloco << "W | ";
    cout << "Hits: " << hit_rate << "% | Misses: " << miss_rate << "%" << endl;
}

// Cache Associativa em Conjunto 
void simularCacheAssociativa(string trace_file, int capacidade, int tamanho_bloco) {
    int num_linhas_total = capacidade / tamanho_bloco;
    int vias = 2; 
    int num_conjuntos = num_linhas_total / vias; // As linhas agora sao divididas em grupos de 2 vias
    
    int offset_bits = log2(tamanho_bloco);
    int index_bits = log2(num_conjuntos); // O index agora aponta para um grupo
    unsigned int index_mask = num_conjuntos - 1;

    // a cache agora virou uma matriz 
    vector<vector<CacheLine>> cache(num_conjuntos, vector<CacheLine>(vias));

    ifstream file(trace_file);
    if (!file.is_open()) return;

    int hits = 0;
    int misses = 0;
    unsigned int endereco;
    int tempo_atual = 0; // usado para saber quem foi acessado por ultimo

    while (file >> endereco) {
        tempo_atual++; // atualiza o tempo a cada acesso

        unsigned int endereco_bloco = endereco >> offset_bits;
        unsigned int indice = endereco_bloco & index_mask;
        unsigned int tag = endereco_bloco >> index_bits;

        bool hit = false;
        
        // aqui é feito o loop para verificar se o bloco ja esta na cache, se tiver atualiza o tempo de uso
        for (int i = 0; i < vias; i++) {
            if (cache[indice][i].valid && cache[indice][i].tag == tag) {
                hit = true;
                hits++;
                cache[indice][i].lru_time = tempo_atual; // Atualiza que acabou de ser usado
                break;
            }
        }

        // caso nao tenha hit, procura a via mais antiga ou vazia para substituir
        if (!hit) {
            misses++;
            int via_substituir = 0;
            
            // logica do LRU - acha o slot mais antigo ou vazio
            int menor_tempo = cache[indice][0].lru_time;
            for (int i = 0; i < vias; i++) {
                if (!cache[indice][i].valid) { // Se tiver espaco vazio, usa ela na hora
                    via_substituir = i;
                    break;
                }
                if (cache[indice][i].lru_time < menor_tempo) { // procura quem foi usado ha mais tempo
                    menor_tempo = cache[indice][i].lru_time;
                    via_substituir = i;
                }
            }
            
            // Salva o novo bloco na via escolhida
            cache[indice][via_substituir].valid = true;
            cache[indice][via_substituir].tag = tag;
            cache[indice][via_substituir].lru_time = tempo_atual; // indica que acabou de ser usado
        }
    }
    file.close();

    int total = hits + misses;
    if (total == 0) return;
    float hit_rate = ((float)hits / total) * 100.0f;
    float miss_rate = ((float)misses / total) * 100.0f;
    cout << "Associativa 2-vias: " << capacidade << "W, Bloco " << tamanho_bloco << "W | ";
    cout << "Hits: " << hit_rate << "% | Misses: " << miss_rate << "%" << endl;
}

//chamando a funcao de simulacao
int main() {
    cout << "========= RESULTADOS TRACE 1 (Acesso por Linha) =========" << endl;
    cout << "--- Diretamente Mapeada ---" << endl;
    simularCache("trace_address1.dat", 128, 16);
    simularCache("trace_address1.dat", 128, 32);
    simularCache("trace_address1.dat", 256, 16);
    simularCache("trace_address1.dat", 256, 32);
    simularCache("trace_address1.dat", 512, 16);
    simularCache("trace_address1.dat", 512, 32);
    simularCache("trace_address1.dat", 512, 64);

    cout << "\n--- Associativa em Conjunto (2 vias) ---" << endl;
    simularCacheAssociativa("trace_address1.dat", 128, 16);
    simularCacheAssociativa("trace_address1.dat", 128, 32);
    simularCacheAssociativa("trace_address1.dat", 256, 16);
    simularCacheAssociativa("trace_address1.dat", 256, 32);
    simularCacheAssociativa("trace_address1.dat", 512, 16);
    simularCacheAssociativa("trace_address1.dat", 512, 32);
    simularCacheAssociativa("trace_address1.dat", 512, 64);
    
    cout << "\n========= RESULTADOS TRACE 2 (Acesso por Coluna) =========" << endl;
    cout << "--- Diretamente Mapeada ---" << endl;
    simularCache("trace_address2.dat", 128, 16);
    simularCache("trace_address2.dat", 128, 32);
    simularCache("trace_address2.dat", 256, 16);
    simularCache("trace_address2.dat", 256, 32);
    simularCache("trace_address2.dat", 512, 16);
    simularCache("trace_address2.dat", 512, 32);
    simularCache("trace_address2.dat", 512, 64);

    cout << "\n--- Associativa em Conjunto (2 vias) ---" << endl;
    simularCacheAssociativa("trace_address2.dat", 128, 16);
    simularCacheAssociativa("trace_address2.dat", 128, 32);
    simularCacheAssociativa("trace_address2.dat", 256, 16);
    simularCacheAssociativa("trace_address2.dat", 256, 32);
    simularCacheAssociativa("trace_address2.dat", 512, 16);
    simularCacheAssociativa("trace_address2.dat", 512, 32);
    simularCacheAssociativa("trace_address2.dat", 512, 64);


    return 0;
}
