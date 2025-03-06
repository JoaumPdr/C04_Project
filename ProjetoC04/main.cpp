#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <locale>
#include <limits>
#include <map>
#include <cctype>//para não ocorrer problemas com letras minuscula ou maiuscula na ordem crescente
#include <windows.h>  // Para a manipulação de cores no Windows para o mapa
#include <functional> // Para o mapa de pokemon
#include <thread> // criar animações de inicialização
#include <chrono> // criar animações deinicialização

using namespace std;

struct Personagem
{
    int cepAtual;
};

struct ArestaAdjacente {
    int cepDestino;
    double peso;
};

struct Cidades {
    int cep;
    string nome;
    bool cp; // se existe centro Pokémon
    int posicao[2]; // posição no mapa da cidade, para calcular distâncias entre outras cidades
    vector<ArestaAdjacente> cidadesAdjacentes; // cidades adjacentes com peso da aresta
};

struct Pokemon {// Estrutura para representar um Pokémon
    string nome;
    string tipo;
    int numero;
    int posicao[2];
};

struct No {// Nó da árvore binária de busca
    Pokemon dados;
    No* esquerda;
    No* direita;
    int altura;     // Altura do nó (necessária para AVL)


    No(const Pokemon& p) : dados(p), esquerda(nullptr), direita(nullptr), altura(1) {}
};

//=======================Fuçoes sobre as cidades=============================================================

double calcularDistanciaCidades(const Cidades& cidade1, const Cidades& cidade2) {
    int dx = cidade2.posicao[0] - cidade1.posicao[0];
    int dy = cidade2.posicao[1] - cidade1.posicao[1];
    return sqrt(dx * dx + dy * dy);
}

void adicionarCidade(vector<Cidades>& cidades, int& cep) {
    Cidades novaCidade;
    novaCidade.cep = cep;

    cout << "Funcao: Adicionar cidade\n========================================\n\n";
    cout << "Cidade numero: " << novaCidade.cep << endl;

    cout << "Qual o nome da cidade: ";
    cin.ignore();
    getline(cin, novaCidade.nome);

    while(true) {
        cout << "Esta cidade possui um Centro Pokemon? (0 = Nao, 1 = Sim): ";
        int cp;
        cin >> cp;
        if (cp == 1 || cp == 0 ) {
            if (cp == 1) novaCidade.cp = true;
            else novaCidade.cp = false;
            break;
        }
        else cout << "\n( ! ) OPCAO: "<<cp<<" NAO E VALIDA, FAVOR SELECIONAR UMA OPCAO VALIDA ( ! )\n\n";
    }

    while (true) {  // Loop infinito ate coordenadas validas
        cout << "Onde a cidade esta situada? (X, Y):" << endl;
        cout << "X = ";
        cin >> novaCidade.posicao[0];
        cout << "Y = ";
        cin >> novaCidade.posicao[1];

        bool val = true;  // Assume que a coordenada e valida inicialmente

        for (const Cidades& cidade : cidades) {
            if (novaCidade.posicao[0] == cidade.posicao[0] && novaCidade.posicao[1] == cidade.posicao[1]) {
                cout << "\n( ! ) NESSAS COORDENADAS JA EXISTE UMA CIDADE, FAVOR INSERIR OUTRAS COORDENADAS ( ! )\n\n";
                val = false;  // Coordenada invalida
                break;  // Sai do loop para pedir novas coordenadas
            }
        }

        if (val) {
            break;  // Sai do while se a coordenada for valida
        }
    }

    // Solicitar cidades adjacentes e calcular automaticamente as distancias
    cout << "Digite o CEP das cidades adjacentes (digite -1 para terminar):\n";
    while (true) {
        int cepAdjacente;
        cout << "CEP da cidade adjacente: ";
        cin >> cepAdjacente;
        if (cepAdjacente == -1) break;

        for (int i = 0; i < cidades.size(); i++) {
            Cidades& cidade = cidades[i];
            if (cidade.cep == cepAdjacente) {
                double distancia = calcularDistanciaCidades(novaCidade, cidade);
                novaCidade.cidadesAdjacentes.push_back({cepAdjacente, distancia});
                cidade.cidadesAdjacentes.push_back({novaCidade.cep, distancia});
            }
        }
    }

    cidades.push_back(novaCidade);// Adiciona a nova cidade ao vetor de cidades
    cep++;

    cout << "\n\nCidade Adicionada com Sucesso!\n========================================\n\n\n\n\n";
}

void mostrarCidades(const vector<Cidades>& cidades) {
    cout << "Lista de cidades:\n";
    for (int i = 0; i < cidades.size(); ++i) {
        const Cidades& cidade = cidades[i];  // Acessando cada cidade usando o indice i

        cout << "--------------------------------------\n";
        cout << "Cidade: " << cidade.nome << endl;
        cout << "CEP: " << cidade.cep << endl;
        cout << "Centro Pokemon: " << (cidade.cp ? "Sim" : "Nao") << endl;
        cout << "Posicao: (" << cidade.posicao[0] << ", " << cidade.posicao[1] << ")\n";
        cout << "Cidades adjacentes:\n";

        for (int j = 0; j < cidade.cidadesAdjacentes.size(); ++j) {
            const ArestaAdjacente& aresta = cidade.cidadesAdjacentes[j];
            cout << " - CEP: " << aresta.cepDestino << ", Distancia: " << aresta.peso << endl;
        }
    }
    cout << "========================================\n\n\n\n\n\n";
}

void calcularMelhorRotaParaCentroPokemon(vector<Cidades>& cidades, Personagem& personagem, int cep) {

    cout<<"Função: Calcular melhor rota até um centro pokemon\n========================================\n\n";

    int maxCep = cep;
    vector<double> dist(maxCep + 1, numeric_limits<double>::infinity());
    vector<int> anterior(maxCep + 1, -1);
    vector<bool> visitado(maxCep + 1, false);

    dist[personagem.cepAtual] = 0.0;
    double menorDistanciaCentroPokemon = numeric_limits<double>::infinity();
    int cepCentroPokemonMaisProximo = -1;

    while (true) {
        int cidadeAtual = -1;
        double menorDistancia = numeric_limits<double>::infinity();

        for (int i = 1; i <= maxCep; i++) {
            if (!visitado[i] && dist[i] < menorDistancia) {
                cidadeAtual = i;
                menorDistancia = dist[i];
            }
        }

        if (cidadeAtual == -1) break;

        visitado[cidadeAtual] = true;

        for (int j = 0; j < cidades[cidadeAtual - 1].cidadesAdjacentes.size(); j++) {
            const ArestaAdjacente& adj = cidades[cidadeAtual - 1].cidadesAdjacentes[j];
            if (dist[cidadeAtual] + adj.peso < dist[adj.cepDestino]) {
                dist[adj.cepDestino] = dist[cidadeAtual] + adj.peso;
                anterior[adj.cepDestino] = cidadeAtual;
            }
        }

        // Verifica se cidade atual é um centro Pokémon e se a distância é a menor até agora
        if (cidades[cidadeAtual - 1].cp && dist[cidadeAtual] < menorDistanciaCentroPokemon) {
            menorDistanciaCentroPokemon = dist[cidadeAtual];
            cepCentroPokemonMaisProximo = cidadeAtual;
        }
    }

    if (cepCentroPokemonMaisProximo != -1) {
        cout << "Centro Pokémon mais próximo encontrado na cidade com CEP: " << cepCentroPokemonMaisProximo << endl;
        cout << "Distância até o Centro Pokémon mais próximo: " << menorDistanciaCentroPokemon << "\n";

        vector<int> rota;
        int caminho = cepCentroPokemonMaisProximo;
        while (caminho != personagem.cepAtual) {
            rota.push_back(caminho);
            caminho = anterior[caminho];
        }
        rota.push_back(personagem.cepAtual);

        cout << "Rota para o Centro Pokémon mais próximo: ";
        for (auto it = rota.rbegin(); it != rota.rend(); ++it) {
            cout << " -> "<< *it ;
        }
        cout << endl;

        personagem.cepAtual = cepCentroPokemonMaisProximo;
        cout<<"\nSe movendo até a cidade\n========================================\n\n\n\n\n";
    } else {
        cout << "Nenhum Centro Pokémon foi encontrado acessível a partir da cidade atual.\n========================================\n\n\n";
    }
}



//======================Funções de arvore binaria e pokemons===================================================

void inserirPokemon(No*& raiz, const Pokemon& pokemon) {
    Pokemon pokemonCAPS = pokemon;
    pokemonCAPS.nome[0] = toupper(pokemonCAPS.nome[0]);

    for (size_t i = 1; i < pokemonCAPS.nome.size(); ++i) {
        pokemonCAPS.nome[i] = tolower(pokemonCAPS.nome[i]);
    }

    pokemonCAPS.tipo[0] = toupper(pokemonCAPS.tipo[0]);

    for (size_t i = 1; i < pokemonCAPS.tipo.size(); ++i) {
        pokemonCAPS.tipo[i] = tolower(pokemonCAPS.tipo[i]);
    }

    if (raiz == nullptr) {
        raiz = new No(pokemonCAPS);
    } else if (pokemonCAPS.nome < raiz->dados.nome) {
        inserirPokemon(raiz->esquerda, pokemonCAPS);
    } else {
        inserirPokemon(raiz->direita, pokemonCAPS);
    }
}

void inserirPokemonTipo(No*& raiz, const Pokemon& pokemon) {

    Pokemon pokemonCAPS = pokemon;
    pokemonCAPS.nome[0] = toupper(pokemonCAPS.nome[0]);

    for (size_t i = 1; i < pokemonCAPS.nome.size(); ++i) {
        pokemonCAPS.nome[i] = tolower(pokemonCAPS.nome[i]);
    }

    pokemonCAPS.tipo[0] = toupper(pokemonCAPS.tipo[0]);

    for (size_t i = 1; i < pokemonCAPS.tipo.size(); ++i) {
        pokemonCAPS.tipo[i] = tolower(pokemonCAPS.tipo[i]);
    }

    if (raiz == nullptr) {
        raiz = new No(pokemonCAPS);
    } else if (pokemonCAPS.tipo < raiz->dados.tipo) {
        inserirPokemon(raiz->esquerda, pokemonCAPS);
    } else {
        inserirPokemon(raiz->direita, pokemonCAPS);
    }
}

No* buscarPokemon(No* raiz, const string& nome) {
    if (raiz == nullptr || raiz->dados.nome == nome) {
        return raiz;
    } else if (nome < raiz->dados.nome) {
        return buscarPokemon(raiz->esquerda, nome);
    } else {
        return buscarPokemon(raiz->direita, nome);
    }
}

void exibirPokemonsOrdenadosNaArvore(No* raiz) {
    if (raiz != nullptr) {
        exibirPokemonsOrdenadosNaArvore(raiz->esquerda);
        cout << "Nome: " << raiz->dados.nome << ", Tipo: " << raiz->dados.tipo
             << ", Número: " << raiz->dados.numero << ", Localização: ("
             << raiz->dados.posicao[0] << ", " << raiz->dados.posicao[1] << ")" << endl;
        exibirPokemonsOrdenadosNaArvore(raiz->direita);
    }
}

void inserirPokemon_Usuario(No*& raiz, No*& raiztipo) {
    Pokemon novoPokemon;

    cout << "Funcao: Inserir Pokemon na Pokedex\n========================================\n\n";
    cout << "Digite o nome do Pokemon: ";
    cin.ignore();
    getline(cin, novoPokemon.nome);

    // Verifica se o Pokemon ja existe na Pokedex
    if (buscarPokemon(raiz, novoPokemon.nome)) {
        cout << "( ! ) Um Pokemon com esse nome ja existe na Pokedex! ( ! )\n";
        return;
    }

    cout << "Digite o tipo do Pokemon: ";
    cin >> novoPokemon.tipo;

    cout << "Digite o numero do Pokemon: ";
    cin >> novoPokemon.numero;

    cout << "Onde o Pokemon foi encontrado? (X, Y):" << endl;
    cout << "X = ";
    cin >> novoPokemon.posicao[0];
    cout << "Y = ";
    cin >> novoPokemon.posicao[1];

    // Funcao de insercao na arvore binaria
    inserirPokemon(raiz, novoPokemon);
    inserirPokemon(raiztipo, novoPokemon);

    cout << "\n\nPokemon inserido com sucesso!!\n========================================\n\n\n\n\n";
}

void buscarPokemon_Usuario(No* raiz) {
    string nomeBusca;

    cout << "Funcao: Buscar Pokemon na Pokedex\n========================================\n\n";
    cout << "Digite o nome do Pokemon que deseja buscar: ";
    cin.ignore();
    getline(cin, nomeBusca);

    No* encontrado = buscarPokemon(raiz, nomeBusca);
    if (encontrado) {
        cout << "\nPokemon encontrado:\n";
        cout << "Nome: " << encontrado->dados.nome << "\n";
        cout << "Tipo: " << encontrado->dados.tipo << "\n";
        cout << "Numero: " << encontrado->dados.numero << "\n";
        cout << "Localizacao: (" << encontrado->dados.posicao[0] << ", " << encontrado->dados.posicao[1] << ")\n========================================\n\n\n\n\n";
    } else {
        cout << "\n( ! ) Pokemon nao encontrado! ( ! )\n========================================\n\n\n\n\n";
    }
}

void exibirPokemonsColetados_Usuario(No* raiz) {
    cout << "Funcao: Mostrar Pokemon coletados\n========================================\n\n";
    cout << "Lista de Pokemon na Pokedex:\n";
    exibirPokemonsOrdenadosNaArvore(raiz);
    cout << "========================================\n\n";
}

void contarPorTipo(No* raiz, map<string, vector<Pokemon>>& tipoPokemon) {
    if (raiz != nullptr) {
        tipoPokemon[raiz->dados.tipo].push_back(raiz->dados); // Adiciona Pokémon ao tipo correspondente
        contarPorTipo(raiz->esquerda, tipoPokemon);
        contarPorTipo(raiz->direita, tipoPokemon);
    }
}

void exibirPokemonsPorTipo(No* raiz) {
    map<string, vector<Pokemon>> tipoPokemon; // Mapa para armazenar Pokémon agrupados por tipo

    // Conta os Pokémon de cada tipo
    contarPorTipo(raiz, tipoPokemon);

    // Exibe a contagem e os Pokémon de cada tipo
    for (const auto& tipo : tipoPokemon) {
        cout << "Pokemons do tipo " << tipo.first <<": "<< tipo.second.size()<< endl;
        for (const auto& pokemon : tipo.second) {
            cout << "Nome: " << pokemon.nome <<", Numero: " << pokemon.numero << ", Localizacao: ("<< pokemon.posicao[0] << ", " << pokemon.posicao[1] << ")" << endl;
        }
        cout << endl;
    }

    cout << "========================================\n\n";


}

void contarPorTipo(No* raiz, vector<pair<string, vector<Pokemon>>>& tipoPokemon) {
    if (raiz != nullptr) {
        // Verifica se o tipo ja existe no vetor
        bool tipoEncontrado = false;
        for (auto& tipo : tipoPokemon) {
            if (tipo.first == raiz->dados.tipo) {
                tipo.second.push_back(raiz->dados); // Adiciona o Pokemon ao tipo existente
                tipoEncontrado = true;
                break;
            }
        }

        // Se o tipo nao foi encontrado, adiciona um novo tipo ao vetor
        if (!tipoEncontrado) {
            tipoPokemon.push_back({raiz->dados.tipo, {raiz->dados}});
        }

        // Chama a funcao recursivamente para as subarvores
        contarPorTipo(raiz->esquerda, tipoPokemon);
        contarPorTipo(raiz->direita, tipoPokemon);
    }
}

//===================Funções sobre algoritimo geometrico=================================

int contarPokemonsEmRaio(No* raiz, const Personagem& personagem, const vector<Cidades>& cidades) {
    if (raiz == nullptr) {
        return 0; // Caso base: se o nó for nulo, não há Pokémon a contar
    }

    // Calcula a distância entre o Pokémon no nó atual e o personagem
    int dx = raiz->dados.posicao[0] - cidades[personagem.cepAtual].posicao[0];
    int dy = raiz->dados.posicao[1] - cidades[personagem.cepAtual].posicao[1];
    int distancia = sqrt(dx * dx + dy * dy);

    // Verifica se o Pokémon está dentro do raio
    int count = 0;

    if (distancia <= 100) {
        count = 1;
    }

    // Soma o contador com os resultados das subárvores esquerda e direita
    count += contarPokemonsEmRaio(raiz->esquerda, personagem, cidades);
    count += contarPokemonsEmRaio(raiz->direita, personagem, cidades);

    return count;
}

void exibirPokemonsDentroDoRaio(No* raiz, const Personagem& personagem, const vector<Cidades>& cidades) {
    int count = contarPokemonsEmRaio(raiz, personagem, cidades);
    cout << count << " Pokémons podem ser encontrados em 100m do jogador.\n\n\n\n";
}

//====================Funções auxiliares================================================

void mover(const vector<Cidades>& cidades, Personagem& red) {

    cout<<"Funcaoo: Mover personagem:\n========================================\n\n";

    while (true) { //Validação de dados para nao poder mover para cidades que nao exitem
        cout << "Mover Personagem para a Cidade:\n\n";
        for (const Cidades& cidade : cidades) {
            cout << cidade.cep << " --- " << cidade.nome << endl;
        }

        cout << "\nQual cidade deseja visitar?: ";
        int escolha;
        cin >> escolha;

        // Verifica se o cep existe na lista de cidades
        bool cepEx = false;
        for (const Cidades& cidade : cidades) {
            if (cidade.cep == escolha) {
                cepEx = true;
                break;
            }
        }

        if (cepEx) {
            red.cepAtual = escolha;
            cout << "Chegamos...\n========================================\n\n\n\n\n";
            break;
        } else { // Caso contrário, exibe uma mensagem de erro e repete
            cout << "\n⚠ ESSA CIDADE AINDA NÃO EXISTE, FAVOR SELECIONAR UMA CIDADE QUE EXISTA. ⚠\n\n";
        }
    }
}

void funcaonaoReconhecida(int escolha) {
    cout<<"⚠ Função numero:"<<escolha<<" ainda não pronta, favor selecionar outras. ⚠\n========================================\n\n\n\n";
}

void mostrarMapaCidades(const vector<Cidades>& cidades) {
    const int MAP_SIZE = 50; // Definindo um tamanho fixo para o mapa
    char mapa[MAP_SIZE][MAP_SIZE];

    // Inicializa o mapa com espaços em branco
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            mapa[i][j] = '.';
        }
    }

    // Coloca as cidades no mapa de acordo com suas posições
    for (const Cidades& cidade : cidades) {
        int x = cidade.posicao[0];
        int y = cidade.posicao[1];

        if (x < MAP_SIZE && y < MAP_SIZE) {
            mapa[x][y] = cidade.nome[0]; // Usando a primeira letra do nome da cidade
        }
    }

    // Imprime a legenda explicando o que cada cor significa
    cout << "LEGENDA DO MAPA: " << endl;
    cout << "\n";
    cout << "\033[1;34m" << "AZUL >> Exibe a borda do mapa!" << "\033[0m" << endl;
    cout << "\033[1;32m" << "VERDE >> Cidades com Centro Pokémon!" << "\033[0m" << endl;
    cout << "\033[1;31m" << "VERMELHO >> Cidades sem Centro Pokémon!" << "\033[0m" << endl;
    cout << "\n";

    // Exibe a borda superior
    cout << "\033[1;34m" << " ╔══════════════════════════════════════════════════════════════════════════════════════════════════════╗" << "\033[0m" << endl;

    // Exibe o mapa com bordas e cores
    for (int i = 0; i < MAP_SIZE; ++i) {
        cout << "\033[1;34m" << " ║ " << "\033[0m"; // Borda esquerda

        for (int j = 0; j < MAP_SIZE; ++j) {
            // Verifica se há uma cidade e aplica cor conforme o centro Pokémon
            bool cidadeEncontrada = false;
            for (const Cidades& cidade : cidades) {
                int x = cidade.posicao[0];
                int y = cidade.posicao[1];

                if (x == i && y == j) {
                    cidadeEncontrada = true;
                    // Se há centro Pokémon, usa cor verde
                    if (cidade.cp) {
                        cout << "\033[1;32m" << cidade.nome[0] << "\033[0m "; // Verde
                    }
                    // Se não há centro Pokémon, usa cor vermelha
                    else {
                        cout << "\033[1;31m" << cidade.nome[0] << "\033[0m "; // Vermelho
                    }
                    break;
                }
            }

            // Se não há cidade, apenas imprime um ponto
            if (!cidadeEncontrada) {
                cout << ". "; // Espaços vazios
            }
        }

        cout << "\033[1;34m" << " ║" << "\033[1;34m" << endl; // Borda direita
    }

    // Exibe a borda inferior
    cout << "\033[1;34m" << " ╚══════════════════════════════════════════════════════════════════════════════════════════════════════╝" << "\033[0m" << endl;
}

// Função para atribuir cores com base no tipo de Pokémon
auto obterCorPorTipo = [](const string& tipo) {
    string tipoLower = tipo; // Fazendo uma cópia em minúsculas para comparação
    transform(tipoLower.begin(), tipoLower.end(), tipoLower.begin(), ::tolower);

    if (tipoLower == "fogo") return "\033[1;31m"; // Vermelho
    if (tipoLower == "agua") return "\033[1;34m"; // Azul
    if (tipoLower == "grama") return "\033[1;32m"; // Verde
    if (tipoLower == "eletrico") return "\033[1;33m"; // Amarelo
    if (tipoLower == "normal") return "\033[1;37m"; // Branco
    if (tipoLower == "psiquico") return "\033[1;35m"; // Magenta
    if (tipoLower == "inseto") return "\033[0;32m"; // Verde escuro
    if (tipoLower == "dragao") return "\033[1;36m"; // Ciano
    if (tipoLower == "fantasma") return "\033[1;30m"; // Preto
    if (tipoLower == "lutador") return "\033[1;31m"; // Vermelho forte
    if (tipoLower == "terra") return "\033[0;33m"; // Marrom
    if (tipoLower == "voador") return "\033[1;34m"; // Azul claro
    if (tipoLower == "veneno") return "\033[0;35m"; // Roxo
    if (tipoLower == "rocha") return "\033[1;33m"; // Amarelo escuro
    if (tipoLower == "aco") return "\033[1;37m"; // Prata (antigo Steel)
    if (tipoLower == "fada") return "\033[1;35m"; // Magenta para fada
    return "\033[0m"; // Sem cor
};
// Função para preencher o mapa com os Pokémon e suas cores
void mostrarMapaPokemonsPorTipo(No* raiz) {
    const int MAP_SIZE = 50; // Tamanho fixo do mapa
    char mapa[MAP_SIZE][MAP_SIZE];
    string mapaCor[MAP_SIZE][MAP_SIZE]; // Mapa para armazenar as cores associadas

    // Inicializa o mapa com espaços em branco
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            mapa[i][j] = '.'; // Espaço vazio
            mapaCor[i][j] = "\033[0m"; // Sem cor inicial
        }
    }

    // Função recursiva para preencher o mapa
    function<void(No*)> preencherMapa = [&](No* nodo) {
        if (nodo == nullptr) return;

        int x = nodo->dados.posicao[0];
        int y = nodo->dados.posicao[1];

        if (x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE) {
            mapa[x][y] = nodo->dados.nome[0]; // Usa a primeira letra do nome do Pokémon
            mapaCor[x][y] = obterCorPorTipo(nodo->dados.tipo); // Atribui a cor correspondente ao tipo
        } else {
            cout << "Coordenadas inválidas para Pokémon: " << nodo->dados.nome
                 << " (" << x << ", " << y << ")\n";
        }
        preencherMapa(nodo->esquerda);
        preencherMapa(nodo->direita);
    };

    preencherMapa(raiz);

    // Imprime a legenda com todos os tipos em ordem alfabética
    cout << "LEGENDA DO MAPA: \n";
    vector<string> tipos = {
        "fogo", "agua", "grama", "eletrico", "normal", "psiquico",
        "inseto", "dragao", "fantasma", "lutador", "terra", "voador",
        "veneno", "rocha", "aco", "fada"
    };

    // Ordena os tipos em ordem alfabética
    sort(tipos.begin(), tipos.end());

    // Imprime a legenda com as cores correspondentes
    for (const string& tipo : tipos) {
        cout << obterCorPorTipo(tipo) << tipo[0] + tipo.substr(1) << "\033[0m, ";
    }
    cout << "\n\n";

    // Exibe a borda superior
    cout << "\033[1;34m" << " ╔══════════════════════════════════════════════════════════════════════════════════════════════════════╗" << "\033[0m" << endl;

    // Exibe o mapa com bordas e cores
    for (int i = 0; i < MAP_SIZE; ++i) {
        cout << "\033[1;34m" << " ║ " << "\033[0m"; // Borda esquerda

        for (int j = 0; j < MAP_SIZE; ++j) {
            if (mapa[i][j] == '.') {
                cout << ". "; // Espaços vazios
            } else {
                // Exibe a letra do Pokémon com a cor correspondente ao seu tipo
                cout << mapaCor[i][j] << mapa[i][j] << "\033[0m "; // Aplica a cor armazenada
            }
        }

        cout << "\033[1;34m" << " ║" << "\033[0m" << endl; // Borda direita
    }

    // Exibe a borda inferior
    cout << "\033[1;34m" << " ╚══════════════════════════════════════════════════════════════════════════════════════════════════════╝" << "\033[0m"<<endl;
}

//==================== Função inicializar ================================================

void interfaceInicio(string& nomeTreinador) {
    // Limpeza da tela (para melhorar a experiência do usuário)
    system("cls");

    // Cabeçalho com bordas mais elegantes usando '═' e '║'
    cout << "═══════════════════════════════════════════\n";
    cout << "║                                         ║\n";
    cout << "║        Bem-vindo à sua Pokedex!         ║\n";
    cout << "║      Sua jornada Pokémon começa agora!  ║\n";
    cout << "║                                         ║\n";
    cout << "═══════════════════════════════════════════\n";

    // Pausa estética para dar tempo ao usuário apreciar a interface
    this_thread::sleep_for(chrono::milliseconds(800));

    // Mensagem de entrada para o nome do treinador
    cout << "\nPor favor, insira seu nome, treinador(a):\n";
    cout << "> ";
    getline(cin, nomeTreinador);

    // Pausa para estética
    this_thread::sleep_for(chrono::milliseconds(600));

    // Saudação personalizada para o treinador
    cout << "\nOlá, " << nomeTreinador << "! Você está pronto para sua jornada?\n";
    this_thread::sleep_for(chrono::milliseconds(1000));

    // Exibindo a animação de carregamento
    cout << "\nCarregando... Prepare-se para a aventura!\n";
    this_thread::sleep_for(chrono::milliseconds(1200));

    // Animação simples de "carregamento"
    cout << "\n";
    for (int i = 0; i < 3; i++) {
        cout << ".";
        this_thread::sleep_for(chrono::milliseconds(400));
    }

    cout << "\n\nA jornada de " << nomeTreinador << " está prestes a começar!\n";
    this_thread::sleep_for(chrono::milliseconds(700));
    cout << "\nO que você deseja fazer agora?\n";
    this_thread::sleep_for(chrono::milliseconds(500));
}

//==========================Banco de dados para testes========================================================

void inicializarCidadesKanto(vector<Cidades>& cidades, int& cep) {// Definindo cidades principais de Kanto para um pequeno banco de dados já existente com adjacências e pesos
    cidades.push_back(Cidades{cep++, "Pallet", false, {0, 0}, { {2, 5.1}, {10, 25.61} }});
    cidades.push_back(Cidades{cep++, "Viridian", false, {5, 1}, { {1, 5.1}, {3, 5.39} }});
    cidades.push_back(Cidades{cep++, "Pewter", true, {10, 3}, { {2, 5.39}, {4, 5.39} }});
    cidades.push_back(Cidades{cep++, "Cerulean", true, {8, 8}, { {3, 5.39}, {6, 7.28}, {9, 3.16} }});
    cidades.push_back(Cidades{cep++, "Vermilion", false, {12, 6}, { {6, 5.0}, {8, 9.22}, {9, 3.16} }});
    cidades.push_back(Cidades{cep++, "Lavender", true, {15, 10}, { {4, 7.28}, {5, 5.0}, {8, 5.1}, {9, 4.12} }});
    cidades.push_back(Cidades{cep++, "Celadon", false, {10, 12}, { {8, 5.0}, {9, 3.16} }});
    cidades.push_back(Cidades{cep++, "Fuchsia", false, {14, 15}, { {5, 9.22}, {6, 5.1}, {7, 5.0}, {10, 5.39} }});
    cidades.push_back(Cidades{cep++, "Saffron", true, {11, 9}, { {4, 3.16}, {5, 3.16}, {6, 4.12}, {7, 3.16} }});
    cidades.push_back(Cidades{cep++, "Cinnabar", true, {16, 20}, { {1, 25.61}, {8, 5.39} }});
}

void iniciarPokemons(No*& raiz) {
    inserirPokemon(raiz, {"Pikachu", "eletrico", 25, {5, 4}});
    inserirPokemon(raiz, {"Charmander", "fogo", 4, {3, 7}});
    inserirPokemon(raiz, {"Bulbasaur", "grama", 1, {1, 2}});
    inserirPokemon(raiz, {"Squirtle", "agua", 7, {6, 8}});
    inserirPokemon(raiz, {"Snorlax", "Normal", 143, {10, 16}});
    inserirPokemon(raiz, {"Mew", "Psiquico", 151, {30, 6}});
}

void iniciarPokemonsTipo(No*& raiz) {
    inserirPokemon(raiz, {"Pikachu", "Eletrico", 25, {5, 4}});
    inserirPokemon(raiz, {"Charmander", "fogo", 4, {3, 7}});
    inserirPokemon(raiz, {"Bulbasaur", "grama", 1, {1, 2}});
    inserirPokemon(raiz, {"Squirtle", "Agua", 7, {6, 8}});
    inserirPokemon(raiz, {"Snorlax", "Normal", 143, {10, 16}});
    inserirPokemon(raiz, {"Mew", "Psiquico", 151, {30, 6}});
}

//==================== Funções extras para a arvore ================================================

// Função para calcular a altura de um nó
int altura(No* nodo) {
    if (nodo == nullptr)
        return 0;
    return nodo->altura;
}

// Função para calcular o fator de balanceamento
int fatorBalanceamento(No* nodo) {
    if (nodo == nullptr)
        return 0;
    return altura(nodo->esquerda) - altura(nodo->direita);
}

// Função de rotação à esquerda
No* rotacaoEsquerda(No* raiz) {
    No* novaRaiz = raiz->direita;
    raiz->direita = novaRaiz->esquerda;
    novaRaiz->esquerda = raiz;

    // Atualiza as alturas
    raiz->altura = max(altura(raiz->esquerda), altura(raiz->direita)) + 1;
    novaRaiz->altura = max(altura(novaRaiz->esquerda), altura(novaRaiz->direita)) + 1;

    return novaRaiz;
}

// Função de rotação à direita
No* rotacaoDireita(No* raiz) {
    No* novaRaiz = raiz->esquerda;
    raiz->esquerda = novaRaiz->direita;
    novaRaiz->direita = raiz;

    // Atualiza as alturas
    raiz->altura = max(altura(raiz->esquerda), altura(raiz->direita)) + 1;
    novaRaiz->altura = max(altura(novaRaiz->esquerda), altura(novaRaiz->direita)) + 1;

    return novaRaiz;
}

// Função de rotação dupla (esquerda-direita)
No* rotacaoEsquerdaDireita(No* raiz) {
    raiz->esquerda = rotacaoEsquerda(raiz->esquerda);
    return rotacaoDireita(raiz);
}

// Função de rotação dupla (direita-esquerda)
No* rotacaoDireitaEsquerda(No* raiz) {
    raiz->direita = rotacaoDireita(raiz->direita);
    return rotacaoEsquerda(raiz);
}

// Função para balancear a árvore
No* balancear(No* raiz) {
    int fb = fatorBalanceamento(raiz);

    // Se o fator de balanceamento for maior que 1, a árvore está desbalanceada à esquerda
    if (fb > 1) {
        // Se a subárvore esquerda tem fator de balanceamento negativo, fazer rotação à esquerda-direita
        if (fatorBalanceamento(raiz->esquerda) < 0)
            return rotacaoEsquerdaDireita(raiz);
        // Caso contrário, apenas faça rotação à direita
        return rotacaoDireita(raiz);
    }

    // Se o fator de balanceamento for menor que -1, a árvore está desbalanceada à direita
    if (fb < -1) {
        // Se a subárvore direita tem fator de balanceamento positivo, fazer rotação à direita-esquerda
        if (fatorBalanceamento(raiz->direita) > 0)
            return rotacaoDireitaEsquerda(raiz);
        // Caso contrário, apenas faça rotação à esquerda
        return rotacaoEsquerda(raiz);
    }

    // Se o fator de balanceamento for -1, 0 ou 1, a árvore está balanceada
    return raiz;
}

// Função para balancear as árvores
void balancearArvores(No*& raizPokedex, No*& raizPokedextipo) {
    // Balanceando a árvore da Pokedex
    raizPokedex = balancear(raizPokedex); // Se raizPokedex for uma árvore AVL

    // Balanceando a árvore de tipos de Pokémon
    raizPokedextipo = balancear(raizPokedextipo); // Se raizPokedextipo for uma árvore AVL

    // Mensagem para indicar que o balanceamento foi feito
    cout << "Árvores balanceadas com sucesso!\n";
}

//=============================================================================================================

int main() {
    system("chcp 65001");
    setlocale(LC_ALL, "pt_BR.UTF-8");
    Personagem red = {1};    // Inicia personagem
    int cep = 1;       // Valor da primeira cidade
    vector<Cidades> cidades;      // Inicia vetor da struct de cidades
    No* raizPokedex = nullptr;       // Raiz da arvore
    No* raizPokedextipo = nullptr;   // Raiz da arvore

    iniciarPokemons(raizPokedex);           // Inicia um pequeno banco de dados para teste
    iniciarPokemonsTipo(raizPokedextipo);  // Inicia um pequeno banco de dados para teste
    inicializarCidadesKanto(cidades, cep); // Inicia um pequeno banco de dados para teste

    string nomeTreinador;
    interfaceInicio(nomeTreinador);

    string arte = R"(
 ____   ___  _  _______ ____  _______  __
|  _ \ / _ \| |/ / ____|  _ \| ____\ \/ /
| |_) | | | | ' /|  _| | | | |  _|  \  /
|  __/| |_| | . \| |___| |_| | |___ /  \
|_|    \___/|_|\_\_____|____/|_____/_/\_\
    )";
    cout << arte <<"\n\n\n\n";

    while (true) {
        cout << "O que voce deseja fazer? Digite:\n\n";
        cout << "1 --- Adicionar cidade\n";
        cout << "2 --- Centro Pokemon mais proximo\n";
        cout << "3 --- Mostrar cidades\n\n";
        cout << "4 --- Inserir Pokemon na Pokedex\n";
        cout << "5 --- Buscar Pokemon na Pokedex\n";
        cout << "6 --- Mostrar Pokemons coletados (ordem crescente dos nomes)\n";
        cout << "7 --- Mostrar Pokemons coletados (ordem crescente dos tipos) \n\n";
        cout << "8 --- Mostrar mostrar quantos pokemons podem ser encontrados em 100m\n";
        cout << "9 --- Balancear a árvore\n\n";
        cout << "10 -- Mover personagem\n";
        cout << "11 -- Mostrar mapa cidades\n";
        cout << "12 -- Mostrar mapa pokemons\n";
        cout << "-1 -- Sair\n\n";

        int escolha;
        cin >> escolha;

        cout << "\n========================================\n";

        if (escolha != -1) {
            switch (escolha) {
                case 1:
                    adicionarCidade(cidades, cep);
                    break;

                case 2:
                    calcularMelhorRotaParaCentroPokemon(cidades, red, cep);
                    break;

                case 3:
                    mostrarCidades(cidades);
                    break;

                case 4:
                    inserirPokemon_Usuario(raizPokedex, raizPokedextipo);
                    break;

                case 5:
                    buscarPokemon_Usuario(raizPokedex);
                    break;

                case 6:
                    exibirPokemonsColetados_Usuario(raizPokedex);
                    break;

                case 7:
                    exibirPokemonsPorTipo(raizPokedextipo);
                    break;

                case 8:
                    exibirPokemonsDentroDoRaio(raizPokedex,red,cidades);
                    break;

                case 9:
                    balancearArvores(raizPokedex, raizPokedextipo);
                    break;

                case 10:
                    mover(cidades, red);
                    break;


                case 11:
                    mostrarMapaCidades(cidades);
                    break;

                case 12:
                    mostrarMapaPokemonsPorTipo(raizPokedex);
                    break;

                default:
                    cout << "( ! ) Opcao invalida. Escolha um numero correspondente a uma funcao. ( ! )\n";
                    break;
            }
        } else {
            break;
        }
    }

string arte2 = R"(
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣯⠲⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣏⢧⠀⠛⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡗⡞⡆⠀⠈⠛⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⢡⢻⡄⠀⠀⠀⠙⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⢥⢊⢷⡈⠔⠠⠐⢆⣩⢢⡀⢀⣿⣦⡀⠀⣤⠀⠀⠀⢀⣀⣤⢴⡶⡻⣍⡟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⢎⡱⢊⠗⣌⠣⡜⣨⠑⣎⠹⣲⠿⣯⠻⡾⣟⣧⣴⡞⣯⢻⡜⣧⢻⣵⠟⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⣏⠲⣄⣀⠀⠀⠀⠀⠀⢸⡘⢆⡫⠜⣤⠳⣘⠤⣋⠴⠃⠇⠙⠄⠓⠑⠢⠒⠦⡙⢬⠫⡝⢾⣣⣯⣤⢤⣤⣴⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠸⣿⡻⢟⠧⣠⠘⠻⢄⡀⠀⠀⠀⡿⡘⢤⡛⣄⡇⠃⢀⠀⠀⡀⠀⠀⠄⡀⢀⠃⠘⠀⠣⡘⢇⠻⡄⢇⡛⠿⣼⣼⠃⣀⣀⠀⣀⡀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⣀⠀⠀
⠀⢈⣳⢩⠒⡭⢲⡀⠀⠈⠓⢤⣀⡷⣩⡶⡛⣥⠀⠀⠀⠀⢀⠀⠀⠁⢂⠐⠠⠀⠀⠌⠐⠈⡌⠓⡜⢣⢍⠳⣌⠯⡱⣄⠈⠉⠁⠉⠁⢈⠀⠁⢀⠀⢀⠀⣄⣤⠶⣻⠟⠁
⠀⠛⠲⢧⡹⣐⢣⠹⢦⡀⢦⡘⣼⡿⣋⠴⢛⡁⠀⠠⠁⢀⠠⠀⠌⠀⠀⠠⠁⠀⠂⢀⠁⠂⢌⠱⣈⠇⣎⠳⣌⠳⡱⡌⠖⡄⠂⠌⡐⠠⢀⣂⣤⠖⡞⣹⢣⡾⠋⠀⠀⠀
⠀⠀⠀⠈⢳⡱⣊⢕⢪⡙⢮⠳⣉⠖⣡⡾⠋⠀⢰⡀⠀⠠⠀⠀⠀⢀⠀⠁⠠⠐⠀⢂⠈⡐⡈⠔⣌⠚⣤⠛⣌⢳⠱⡸⢱⢈⡡⠤⢖⡻⢍⠳⣌⠳⣜⠗⠋⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠈⢳⡱⣊⢦⡙⣌⢣⠕⣪⠑⢦⡑⢲⡬⣇⠠⠐⠀⠁⢀⠀⡀⠌⠐⡀⢁⠂⢄⢠⡑⢎⡔⣫⢔⡫⣜⡡⢇⡳⡱⢪⢕⡋⢦⢣⡙⢖⣬⠟⠁⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⢻⡜⣶⣋⢦⠓⡎⢥⠛⡤⢭⠋⠀⢸⡄⠀⠌⡐⢀⠂⠄⡈⠄⡐⠠⢌⢢⠦⣙⢦⡹⢔⠮⡱⢦⠹⡜⢲⠍⣇⠞⣌⢣⢲⡽⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⣹⠳⡜⢦⠛⣜⢢⡛⢬⡏⡀⠀⠀⢿⣆⠰⣈⢦⣐⠠⣐⠢⣜⡱⢎⡣⢞⡡⢖⡍⣎⣳⡽⣌⠳⣉⢧⢚⣼⡚⣴⠟⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⢠⢇⡻⢌⠧⡛⣼⡧⢜⡡⣇⠁⢀⠀⢾⣏⢧⡙⢦⣃⠯⣔⢫⡔⠣⣜⠱⣣⣙⡦⠟⠉⣷⡱⣌⢣⢣⢎⡣⢞⡟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⡼⢠⠙⣎⢳⡹⢴⠻⣎⠵⣙⠦⣅⡓⠀⠸⣶⡙⢦⢃⡞⢬⠲⣌⠓⣬⡷⠛⠉⠀⠠⢀⡷⡳⣌⠇⡞⣰⠹⣌⣯⢛⡳⣒⢖⡲⣒⠖⣦⠤⠤⢀⡀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⡏⠤⡙⡌⢧⡹⢾⠀⠈⠷⣎⡹⡜⣭⢛⠿⡽⡜⣣⢣⠞⣡⢳⡼⠛⠹⠃⠀⠀⠠⢡⡼⢳⡱⢪⡙⣖⣡⠓⣼⢧⢫⠴⣩⠎⡵⣸⢘⡴⢋⡽⠻⠤⣀⠀⠀
⠀⠀⠀⠀⠀⠀⣟⠰⢠⡙⢮⢵⣻⡄⠀⢠⠋⠳⢽⣰⢋⢮⡱⢎⡵⢪⡙⣖⢫⠗⣤⣀⣀⣀⣄⣶⠫⡝⣣⡕⣣⠝⡴⣊⡝⢦⣋⢎⢧⡑⢮⠱⢎⡱⢎⡵⢊⡟⠛⠉⠀⠀
⠀⠀⠀⠀⠀⠀⣏⠲⣁⢾⣩⣞⠶⣷⡀⡎⠀⠀⠀⠉⡛⠲⠽⣎⣜⢧⣙⢦⣋⠾⡰⢭⡹⣙⢞⡰⢫⣴⣟⡱⢣⢏⠶⡱⢎⡳⡜⣎⠶⣙⢦⢛⣬⡳⠗⠛⠳⠇⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⣯⢳⡰⢊⡷⣯⢿⣹⣟⣧⠀⠀⠀⢠⠃⠀⠀⠀⠈⠉⠙⠲⠛⠚⠽⠷⠓⡟⠚⠋⣩⡟⣴⢫⡝⣮⢳⣝⣻⡵⣹⣬⣳⠽⠚⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⣴⣿⡧⡽⣭⣻⣽⣯⢷⣻⣞⣷⣦⡀⡼⠀⠀⠀⠀⠀⠀⢀⡇⠀⠀⠀⠀⢰⠁⢀⣼⢧⣻⣜⣧⢿⣼⣳⢾⠟⠛⠉⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣸⢿⣿⣷⡝⣾⣷⣿⣿⣿⣿⣽⣾⣟⣿⣷⣦⣤⣀⡀⠀⠀⢸⠀⠀⠀⠀⢀⣮⣶⣿⣽⣯⣷⢿⣞⣿⢾⣽⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣿⡓⣞⡿⣿⣳⣿⡿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣶⣶⣿⣿⣿⡿⣟⣾⣯⣷⣿⣿⣯⣿⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⣷⠙⣦⢻⣽⣿⢻⣽⣿⣿⡟⣿⣯⣿⣿⣾⣿⣿⢻⣽⣿⣿⣷⣿⣯⣿⣿⣽⣿⣿⢻⣽⣿⣽⣾⣿⣿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠸⣷⣊⠶⣭⢻⣿⣿⣯⣷⣿⣿⣿⡿⣿⣿⢿⣻⣿⣿⣿⣿⣿⣻⣿⢿⡽⢏⡳⢬⢣⣓⢮⡻⢿⣿⠏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠙⢿⣾⣼⣿⣳⣿⣿⣯⡷⣿⡷⣿⣿⢿⣿⣿⣿⢿⣿⣻⣿⣿⢯⡛⣜⢣⡝⣎⢳⢬⠳⣝⢯⡿⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠈⢻⣷⣿⣷⣿⣿⠛⢷⣻⡽⣯⢿⣿⡾⣿⡿⣟⣿⡿⣞⣯⢳⣌⢳⡜⡬⢓⢮⣹⡞⡿⣜⡷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠟⢿⡏⠙⠀⠀⠈⠙⠿⢿⣾⣟⣿⣽⣿⣻⣽⡻⣜⢯⣲⢯⡛⣴⢻⣋⠷⣩⢓⡎⡗⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠻⣿⣿⢿⣿⠏⠉⠙⢯⢎⡳⢎⣵⣏⣶⣯⢒⠥⣋⣼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠿⠏⠀⠀⠀⠈⢧⡝⣾⣿⣿⣿⣿⣾⣿⡼⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⢿⣿⣿⣳⡿⣞⡿⠊⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠛⠛⠛⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    )";

    cout <<arte2<<"Integrantes:\n\nHenrique Junqueira Bicalho\nEduardo Dias Andrade\nEduardo Melo Bertozzi\nJoão Pedro da Silva Escobar de Oliveira"<<endl;


    //Integrante:
    //Henrique Junqueira Bicalho
    //Eduardo Dias Andrade
    //Eduardo Melo Bertozzi
    //João Pedro da Silva Escobar de Oliveira
    return 0;

}