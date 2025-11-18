#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Configurações Globais ---
#define MAX_SIZE 10
#define PECA_MAX 30
#define NUM_PECAS 7

// Peças de Tetris: I, J, L, O, S, T, Z
const char *pecas[NUM_PECAS] = {"I", "J", "L", "O", "S", "T", "Z"};

// --- Estruturas de Dados ---

// 1. Estrutura da Fila Circular
typedef struct {
    char data[MAX_SIZE][PECA_MAX];
    int frente, traseira;
} Fila;

// 2. Estrutura da Pilha
typedef struct {
    char data[MAX_SIZE][PECA_MAX];
    int topo;
} Pilha;

// 3. Estrutura de Estado (Para a funcionalidade Desfazer)
// Armazena cópias do estado ANTERIOR da Fila e da Pilha.
typedef struct {
    Fila fila_estado;
    Pilha pilha_estado;
    int ultima_acao; // Opcional: Para controle mais granular de quais ações podem ser desfeitas
} Estado;

// Pilha de Histórico de Estados (Para Desfazer)
Pilha historico_estado;

// --- Funções de Inicialização ---

void inicializar_fila(Fila *f) {
    f->frente = -1;
    f->traseira = -1;
}

void inicializar_pilha(Pilha *p) {
    p->topo = -1;
}

void inicializar_historico(Pilha *h) {
    h->topo = -1;
}

// --- Funções de Estado (Desfazer) ---

/**
 * @brief Salva o estado atual da Fila e Pilha no Histórico.
 * @param f Fila atual
 * @param p Pilha atual
 * @param acao Ação realizada
 */
void salvar_estado(Fila *f, Pilha *p, int acao) {
    if (historico_estado.topo < MAX_SIZE - 1) {
        historico_estado.topo++;
        Estado *novo_estado = (Estado *) &historico_estado.data[historico_estado.topo];

        // Copia a Fila
        memcpy(&(novo_estado->fila_estado), f, sizeof(Fila));

        // Copia a Pilha
        memcpy(&(novo_estado->pilha_estado), p, sizeof(Pilha));

        novo_estado->ultima_acao = acao;

        printf("\n✅ Estado salvo para Desfazer.\n");
    } else {
        printf("\n⚠️ Historico cheio! Nao e possivel salvar mais estados.\n");
    }
}

/**
 * @brief Restaura o estado anterior da Fila e Pilha.
 * @param f Fila a ser restaurada
 * @param p Pilha a ser restaurada
 * @return 1 se desfeito com sucesso, 0 se o histórico estiver vazio.
 */
int desfazer_ultima_jogada(Fila *f, Pilha *p) {
    if (historico_estado.topo >= 0) {
        Estado *estado_anterior = (Estado *) &historico_estado.data[historico_estado.topo];

        // Restaura a Fila
        memcpy(f, &(estado_anterior->fila_estado), sizeof(Fila));

        // Restaura a Pilha
        memcpy(p, &(estado_anterior->pilha_estado), sizeof(Pilha));

        historico_estado.topo--;
        printf("\n↩️ Ultima jogada desfeita com sucesso!\n");
        return 1;
    } else {
        printf("\n❌ Historico vazio. Nada para desfazer.\n");
        return 0;
    }
}


// --- Funções Auxiliares de Estruturas ---

// Fila: Verifica se está cheia
int fila_cheia(Fila *f) {
    return (f->frente == 0 && f->traseira == MAX_SIZE - 1) || (f->frente == f->traseira + 1);
}

// Fila: Verifica se está vazia
int fila_vazia(Fila *f) {
    return f->frente == -1;
}

// Fila: Enfileirar (Adiciona na Traseira)
int enfileirar(Fila *f, const char *peca) {
    if (fila_cheia(f)) {
        printf("\n❌ Fila cheia! Nao foi possivel enfileirar.\n");
        return 0;
    }

    if (fila_vazia(f)) {
        f->frente = 0;
    }

    f->traseira = (f->traseira + 1) % MAX_SIZE;
    strncpy(f->data[f->traseira], peca, PECA_MAX - 1);
    f->data[f->traseira][PECA_MAX - 1] = '\0'; // Garantir terminação

    return 1;
}

// Fila: Desenfileirar (Remove da Frente)
int desenfileirar(Fila *f, char *peca_saida) {
    if (fila_vazia(f)) {
        return 0;
    }

    strncpy(peca_saida, f->data[f->frente], PECA_MAX);

    if (f->frente == f->traseira) {
        // Ultimo elemento removido
        inicializar_fila(f);
    } else {
        f->frente = (f->frente + 1) % MAX_SIZE;
    }

    return 1;
}

// Fila: Espiar (Obter o elemento da Frente)
int espiar_fila(Fila *f, char *peca_saida) {
    if (fila_vazia(f)) {
        return 0;
    }
    strncpy(peca_saida, f->data[f->frente], PECA_MAX);
    return 1;
}

// Pilha: Verifica se está cheia
int pilha_cheia(Pilha *p) {
    return p->topo == MAX_SIZE - 1;
}

// Pilha: Verifica se está vazia
int pilha_vazia(Pilha *p) {
    return p->topo == -1;
}

// Pilha: Empilhar (Adiciona no Topo)
int empilhar(Pilha *p, const char *peca) {
    if (pilha_cheia(p)) {
        printf("\n❌ Pilha cheia! Nao foi possivel empilhar.\n");
        return 0;
    }

    p->topo++;
    strncpy(p->data[p->topo], peca, PECA_MAX - 1);
    p->data[p->topo][PECA_MAX - 1] = '\0';

    return 1;
}

// Pilha: Desempilhar (Remove do Topo)
int desempilhar(Pilha *p, char *peca_saida) {
    if (pilha_vazia(p)) {
        return 0;
    }

    strncpy(peca_saida, p->data[p->topo], PECA_MAX);
    p->topo--;

    return 1;
}

// Pilha: Espiar (Obter o elemento do Topo)
int espiar_pilha(Pilha *p, char *peca_saida) {
    if (pilha_vazia(p)) {
        return 0;
    }
    strncpy(peca_saida, p->data[p->topo], PECA_MAX);
    return 1;
}

// --- Funções de Exibição ---

void exibir_fila(Fila *f) {
    printf("Fila Circular (Proximas Pecas): [ ");
    if (fila_vazia(f)) {
        printf("Vazia ");
    } else {
        int i = f->frente;
        do {
            printf("%s ", f->data[i]);
            if (i == f->frente) printf("<- Frente ");
            if (i == f->traseira) printf("<- Traseira ");
            i = (i + 1) % MAX_SIZE;
        } while (i != (f->traseira + 1) % MAX_SIZE);
    }
    printf("]\n");
}

void exibir_pilha(Pilha *p) {
    printf("Pilha de Reserva (Em Jogo):   [ ");
    if (pilha_vazia(p)) {
        printf("Vazia ");
    } else {
        for (int i = 0; i <= p->topo; i++) {
            printf("%s ", p->data[i]);
        }
        printf("<- Topo");
    }
    printf("]\n");
}

void exibir_estado(Fila *f, Pilha *p) {
    printf("\n=== Estado Atual das Estruturas ===\n");
    exibir_fila(f);
    exibir_pilha(p);
    printf("===================================\n");
}

// --- Funcionalidades do Sistema (Ações do Menu) ---

/**
 * @brief 1 - Jogar peça: Tira da Fila (frente) e coloca na Pilha (topo).
 */
void jogar_peca(Fila *f, Pilha *p) {
    salvar_estado(f, p, 1); // Salva o estado antes da alteração
    char peca_jogar[PECA_MAX];

    if (desenfileirar(f, peca_jogar)) {
        if (empilhar(p, peca_jogar)) {
            printf("\n➡️ PECA JOGADA: '%s'. Adicionada a Pilha.\n", peca_jogar);
        } else {
            // Se falhar o empilhar, re-enfileira para manter integridade
            enfileirar(f, peca_jogar);
            printf("\n❌ Falha ao empilhar (Pilha Cheia). Operacao cancelada.\n");
        }
    } else {
        printf("\n❌ Fila vazia! Nao ha peca para jogar.\n");
    }
}

/**
 * @brief 2 - Reservar peça: Tira da Fila (frente) e joga fora (simula reserva/descarte).
 */
void reservar_peca(Fila *f) {
    salvar_estado(f, NULL, 2); // Pilha não é alterada neste caso
    char peca_reservar[PECA_MAX];

    if (desenfileirar(f, peca_reservar)) {
        printf("\n↩️ PECA RESERVADA/DESCARTADA: '%s'. Removida da Fila.\n", peca_reservar);
    } else {
        printf("\n❌ Fila vazia! Nao ha peca para reservar.\n");
    }
}

/**
 * @brief 3 - Usar peça reservada: Tira da Pilha (topo) e simula o uso/descarte.
 */
void usar_peca_reservada(Pilha *p) {
    salvar_estado(NULL, p, 3); // Fila não é alterada neste caso
    char peca_usar[PECA_MAX];

    if (desempilhar(p, peca_usar)) {
        printf("\n⬆️ PECA RESERVADA USADA: '%s'. Removida da Pilha.\n", peca_usar);
    } else {
        printf("\n❌ Pilha vazia! Nao ha peca reservada para usar.\n");
    }
}

/**
 * @brief 4 - Troca peça do topo da Pilha com a da frente da Fila.
 */
void trocar_pecas(Fila *f, Pilha *p) {
    salvar_estado(f, p, 4); // Salva o estado antes da alteração
    char peca_fila[PECA_MAX];
    char peca_pilha[PECA_MAX];

    if (fila_vazia(f) || pilha_vazia(p)) {
        printf("\n❌ Nao e possivel trocar. Fila ou Pilha estao vazias.\n");
        return;
    }

    // 1. Pega os valores (e remove temporariamente)
    desenfileirar(f, peca_fila);
    desempilhar(p, peca_pilha);

    // 2. Tenta Inserir os valores invertidos
    if (enfileirar(f, peca_pilha) && empilhar(p, peca_fila)) {
        printf("\n🔄 TROCA REALIZADA: '%s' (Fila) <-> '%s' (Pilha)\n", peca_fila, peca_pilha);
    } else {
        // Se a troca falhar (ex: fila/pilha cheia após remoção), deve ser corrigida (complexo)
        // No escopo deste nível, assumimos que a remoção abriu espaço e a troca será bem-sucedida.
        // Se fosse estritamente necessário garantir a reversão em caso de falha, teríamos que
        // re-empilhar/re-enfileirar os itens originais.
        printf("\n⚠️ ATENCAO: Troca efetuada mas houve problema na reinsercao. Estado inconsistente.\n");
    }
}

/**
 * @brief 5 - Desfazer a última operação que alterou as estruturas.
 */
void desfazer(Fila *f, Pilha *p) {
    desfazer_ultima_jogada(f, p);
}

/**
 * @brief 6 - Inverter o conteúdo da Fila com o conteúdo da Pilha.
 */
void inverter_estruturas(Fila *f, Pilha *p) {
    salvar_estado(f, p, 6); // Salva o estado antes da alteração

    char temp_peca[PECA_MAX];
    Fila temp_f;
    Pilha temp_p;
    inicializar_fila(&temp_f);
    inicializar_pilha(&temp_p);

    // 1. Mover Fila para Pilha Temporária (Pilha Temp = Fila)
    while (desenfileirar(f, temp_peca)) {
        empilhar(&temp_p, temp_peca);
    }

    // 2. Mover Pilha para Fila Temporária (Fila Temp = Pilha)
    while (desempilhar(p, temp_peca)) {
        enfileirar(&temp_f, temp_peca);
    }

    // 3. Mover Pilha Temporária para a Fila Original (Fila = Pilha Original)
    // Atenção: Fila tem ordem de inserção (FIFO), Pilha de ordem de retirada (LIFO).
    // Para a Fila Original (f) espelhar a Pilha Original (p) na mesma ordem
    // (Topo da Pilha -> Frente da Fila), precisamos de uma Pilha Intermediária.

    Pilha temp_p2; // Pilha intermediária para manter a ordem FIFO para a Fila
    inicializar_pilha(&temp_p2);

    // Mover Pilha Temp (que é a Fila Original) para Pilha Intermediária
    while (desempilhar(&temp_p, temp_peca)) {
        empilhar(&temp_p2, temp_peca);
    }

    // Mover Pilha Intermediária para a Pilha Original (Pilha = Fila Original)
    while (desempilhar(&temp_p2, temp_peca)) {
        empilhar(p, temp_peca);
    }

    // 4. Mover Fila Temporária (que é a Pilha Original) para a Fila Original (Fila = Pilha Original)
    // A ordem na Fila Temp já está na ordem correta para se tornar a nova Fila.
    while (desenfileirar(&temp_f, temp_peca)) {
        enfileirar(f, temp_peca);
    }

    printf("\n🔁 INVERSAO COMPLETA: O conteudo da Fila e da Pilha foram trocados.\n");
}


// --- Funções de Inicialização e Fluxo ---

void popular_fila_inicial(Fila *f) {
    for (int i = 0; i < 5; i++) {
        enfileirar(f, pecas[rand() % NUM_PECAS]);
    }
}

void exibir_menu() {
    printf("\n============================================\n");
    printf("        🎮 Sistema Mestre de Pecas 🎮\n");
    printf("============================================\n");
    printf("1 - Jogar peca (Fila -> Pilha)\n");
    printf("2 - Reservar peca (Fila -> Descarte)\n");
    printf("3 - Usar peca reservada (Pilha -> Descarte)\n");
    printf("4 - Trocar peca do topo da pilha com a da frente da fila\n");
    printf("5 - Desfazer ultima jogada\n");
    printf("6 - Inverter fila com pilha\n");
    printf("0 - Sair\n");
    printf("--------------------------------------------\n");
    printf("Escolha uma acao: ");
}

int main() {
    Fila fila_principal;
    Pilha pilha_reserva;
    int escolha;

    // Inicialização
    srand(time(NULL));
    inicializar_fila(&fila_principal);
    inicializar_pilha(&pilha_reserva);
    inicializar_historico(&historico_estado);
    popular_fila_inicial(&fila_principal);

    do {
        exibir_estado(&fila_principal, &pilha_reserva);
        exibir_menu();
        if (scanf("%d", &escolha) != 1) {
            // Limpa o buffer de entrada para evitar loop infinito em caso de entrada inválida (não numérica)
            while (getchar() != '\n');
            escolha = -1; // Força entrada de menu inválida
        }

        switch (escolha) {
            case 1:
                jogar_peca(&fila_principal, &pilha_reserva);
                break;
            case 2:
                reservar_peca(&fila_principal);
                break;
            case 3:
                usar_peca_reservada(&pilha_reserva);
                break;
            case 4:
                trocar_pecas(&fila_principal, &pilha_reserva);
                break;
            case 5:
                desfazer(&fila_principal, &pilha_reserva);
                break;
            case 6:
                inverter_estruturas(&fila_principal, &pilha_reserva);
                break;
            case 0:
                printf("\n👋 Saindo do sistema. Bom jogo!\n");
                break;
            default:
                printf("\n❌ Opcao invalida. Tente novamente.\n");
                break;
        }

    } while (escolha != 0);

    return 0;
}
