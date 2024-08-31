#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LENGTH 255
#define BUFFER_SIZE 30000

typedef enum { RED, BLACK } Color;

//===================================NODI=RICETTE===================
typedef struct Ingrediente {
    char *ingrediente;        // Nome dell'ingrediente
    int quantita;             // Quantità dell'ingrediente
} Ingrediente;

typedef struct RecipeNode {
    char *key;                     //la chiave è il nome della ricetta
    Ingrediente *ingredienti;                       // lista degli ingredienti necessari
    int num_ingredienti;                            // Numero di ingredienti nella ricetta
    int activeOrders;                               // Puntatore alla ricetta (equivalente a 'key')
    Color color;                                    // Colore del nodo (RED o BLACK)
    struct RecipeNode *left, *right, *parent;       // Puntatore ai figli
} RecipeNode;

typedef struct {
    RecipeNode *root;              // Radice dell'albero RED-BLACK che gestisce le ricette
    RecipeNode *nil;               // Nodo sentinella per rappresentare le foglie NIL delle ricette
    int num_ricette;               // Numero di ricette nel ricettario
} Catalogo;

//===================================NODI=MAGAZZINO========================

// Definizione della struttura per un lotto
typedef struct Lotto {
    int ammount;        // Quantità in grammi
    int scadenza;       // Data di scadenza
} Lotto;

// Definizione della struttura per un ingrediente
typedef struct ResourceNode {
    char *key;   // la chiave è il nome dell'ingrediente
    Lotto *lotti;                // Puntatore ai lotti disponibili
    int num_lotti;               // Numero di lotti disponibili
    int maxGrammi;               // Totale degli ingredienti
    Color color;                 // Colore del nodo (RED o BLACK)
    struct ResourceNode *left, *right, *parent; // Puntatori per l'albero
} ResourceNode;

// Definizione della struttura per il magazzino
typedef struct {
    ResourceNode *root;              // Radice dell'albero RED-BLACK che gestisce le ricette
    ResourceNode *nil;               // Nodo sentinella per rappresentare le foglie NIL delle ricette
    int num_risorse;                 // Numero di ricette nel ricettario
} Magazzino;

//===================================NODI=ORDINI=======================================
typedef struct{
    char *nome;                // Puntatore al nome dell'ordine
    int ammount;               // Quantità richiesta dell'ordine
    int peso;                  // Peso ordine
    int arrivalTime;           // Tempo di arrivo dell'ordine
    RecipeNode *associatedRecipe; //puntatore alla ricetta associata all'ordine
}Ordine;

typedef struct OrderNode{
    Ordine *ordinePronto;
    int key;                                 //la chiave è il tempo di arrivo
    Color color;                             // Colore del nodo (RED o BLACK)
    struct OrderNode *left, *right, *parent; // Puntatori per l'albero
}OrderNode;

// Definizione del struct Ordine
typedef struct {
    Ordine *sospesi;             // 0 preparare, 1 attesa, 3 pronto per la spedizione, 4 spedito
    OrderNode *root;             // Radice dell'albero che gestisce gli ordini pronti
    OrderNode *nil;              // Nodo sentinella per rappresentare le foglie NIL delle ricette
    int num_ordinazioni_sospese; // Numero di ordinazioni pronte per la spedizione
    int num_ordinazioni_pronte;  // Numero di ordinazioni pronte per la spedizione
}Ordinazioni;
//===================================CORRIERE==========================

typedef struct {
    int timeCorriere;    //periodicita corriere
    int caricoMax;       //carico massimo corriere
    int caricoParziale;   //carcico attuale
} Corriere;

//===================================VARIABILI=GLOBALI==================

//catalogo delle ricette
Catalogo catalogo;

//magazino degli ingredienti
Magazzino magazzino;

//Ordinazioni
Ordinazioni ordinazioni;

//def corriere
Corriere corriere = {0,0,0};

//quanti di tempo
int tempo = 0;

//==========================Funzioni=Di=Inizializzazione=Alberi===================================
void initCatalogo(Catalogo *T) {
    // Allocazione memoria per il nodo sentinella nil
    T->nil = (RecipeNode*)malloc(sizeof(RecipeNode));

    // Inizializzazione del nodo sentinella nil
    T->nil->color = BLACK;
    T->nil->left = T->nil;
    T->nil->right = T->nil;
    T->nil->parent = T->nil;
    T->nil->key = NULL;
    T->nil->ingredienti = NULL;
    T->nil->num_ingredienti = 0;
    T->nil->activeOrders = 0;

    // Inizializzazione della radice dell'albero
    T->root = T->nil;

    // Inizializzazione del numero di ricette
    T->num_ricette = 0;
}

void initMagazzino(Magazzino *T) {
    // Allocazione memoria per il nodo sentinella nil
    T->nil = (ResourceNode*)malloc(sizeof(ResourceNode));

    // Inizializzazione del nodo sentinella nil
    T->nil->color = BLACK;
    T->nil->left = T->nil;
    T->nil->right = T->nil;
    T->nil->parent = T->nil;
    T->nil->key = NULL;
    T->nil->lotti = NULL;
    T->nil->num_lotti = 0;
    T->nil->maxGrammi = 0;

    // Inizializzazione della radice dell'albero
    T->root = T->nil;

    // Inizializzazione del numero di ricette
    T->num_risorse = 0;
}

void initOrdinazioni(Ordinazioni *T) {
    // Allocazione memoria per il nodo sentinella nil
    T->nil = (OrderNode*)malloc(sizeof(OrderNode));

    // Inizializzazione del nodo sentinella nil
    T->nil->color = BLACK;
    T->nil->left = T->nil;
    T->nil->right = T->nil;
    T->nil->parent = T->nil;
    T->nil->key = 0;
    T->nil->ordinePronto = NULL;

    // Inizializzazione della radice dell'albero
    T->root = T->nil;

    // Inizializzazione del numero di ricette
    T->num_ordinazioni_pronte = 0;
    T->num_ordinazioni_sospese = 0;
}

//========================================AGGIUNGI=RICETTA========================================================

RecipeNode* createRecipeNode(const char* key) {
    // Allocazione della memoria per il nuovo nodo della ricetta
    RecipeNode* recipenode = (RecipeNode*)malloc(sizeof(RecipeNode));

    // Assegnazione della chiave (nome della ricetta)
    recipenode->key = strdup(key);  // Duplica la stringa per la chiave

    // Assegnazione degli ingredienti e il numero di ingredienti
    recipenode->ingredienti = NULL;
    recipenode->num_ingredienti = 0;

    // Assegnazione del numero di ordini attivi
    recipenode->activeOrders = 0;

    // Inizializzazione degli attributi del nodo
    recipenode->color = RED;  // Colore iniziale del nodo (RED)
    recipenode->left = recipenode->right = recipenode->parent = catalogo.nil;  // Puntatori al nodo sentinella nil

    return recipenode;
}

void leftRotateRicetta(Catalogo *T, RecipeNode *x) {
    RecipeNode *y = x->right;           // y viene settato come il figlio destro di x
    x->right = y->left;                 // il sottoalbero sinistro di y diventa quello destro di x

    if (y->left != T->nil) {
        y->left->parent = x;            // aggiorna il padre del sottoalbero sinistro di y
    }

    y->parent = x->parent;              // attacca il padre di x a y

    if (x->parent == T->nil) {
        T->root = y;                    // se x era la radice, ora la radice è y
    } else if (x == x->parent->left) {
        x->parent->left = y;            // se x era il figlio sinistro, ora y è il figlio sinistro
    } else {
        x->parent->right = y;           // se x era il figlio destro, ora y è il figlio destro
    }

    y->left = x;                        // mette x a sinistra di y
    x->parent = y;                      // aggiorna il padre di x a y
}

void rightRotateRicetta(Catalogo *T, RecipeNode *y) {
    RecipeNode *x = y->left;            // x viene settato come il figlio sinistro di y
    y->left = x->right;                 // il sottoalbero destro di x diventa quello sinistro di y

    if (x->right != T->nil) {
        x->right->parent = y;           // aggiorna il padre del sottoalbero destro di x
    }

    x->parent = y->parent;              // attacca il padre di y a x

    if (y->parent == T->nil) {
        T->root = x;                    // se y era la radice, ora la radice è x
    } else if (y == y->parent->right) {
        y->parent->right = x;           // se y era il figlio destro, ora x è il figlio destro
    } else {
        y->parent->left = x;            // se y era il figlio sinistro, ora x è il figlio sinistro
    }

    x->right = y;                       // mette y a destra di x
    y->parent = x;                      // aggiorna il padre di y a x
}

void RicettaInsertFix(Catalogo *T, RecipeNode *z) {
    while (z->parent->color == RED) {  // Continua finché il padre di z è rosso
        RecipeNode *x = z->parent;

        if (x == x->parent->left) {  // Se x è il figlio sinistro
            RecipeNode *y = x->parent->right;  // y è lo zio (fratello del padre di z)

            if (y->color == RED) {  // Caso 1: lo zio y è rosso
                x->color = BLACK;   // Colora il padre di z in nero
                y->color = BLACK;   // Colora lo zio y in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                z = x->parent;  // Sposta z al livello del nonno
            } else {
                if (z == x->right) {  // Caso 2: z è figlio destro
                    z = x;
                    leftRotateRicetta(T, z);  // Effettua una rotazione a sinistra
                }

                // Caso 3: z è figlio sinistro
                x->color = BLACK;  // Colora il padre di z in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                rightRotateRicetta(T, x->parent);  // Effettua una rotazione a destra
            }
        } else {  // Simmetrico: se x è il figlio destro
            RecipeNode *y = x->parent->left;  // y è lo zio (fratello del padre di z)

            if (y->color == RED) {  // Caso 1: lo zio y è rosso
                x->color = BLACK;   // Colora il padre di z in nero
                y->color = BLACK;   // Colora lo zio y in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                z = x->parent;  // Sposta z al livello del nonno
            } else {
                if (z == x->left) {  // Caso 2: z è figlio sinistro
                    z = x;
                    rightRotateRicetta(T, z);  // Effettua una rotazione a destra
                }

                // Caso 3: z è figlio destro
                x->color = BLACK;  // Colora il padre di z in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                leftRotateRicetta(T, x->parent);  // Effettua una rotazione a sinistra
            }
        }
    }

    T->root->color = BLACK;  // Assicurati che la radice sia sempre nera
}

void InserisciRicetta(Catalogo *T, RecipeNode *z) {
    RecipeNode *y = T->nil;       // y è il padre del nodo considerato
    RecipeNode *x = T->root;      // x è il nodo considerato (inizialmente la radice)

    while (x != T->nil) {         // Trova la posizione corretta per il nuovo nodo z
        y = x;
        if (strcmp(z->key, x->key) < 0) // Confronta le chiavi per decidere la direzione
            x = x->left;          // Se la chiave di z è minore, si muove a sinistra
        else
            x = x->right;         // Altrimenti, si muove a destra
    }

    z->parent = y;                // Collega il genitore al nuovo nodo z
    if (y == T->nil) {
        T->root = z;              // Se l'albero è vuoto, z diventa la radice
    } else if (strcmp(z->key, y->key) < 0) {
        y->left = z;              // Se la chiave di z è minore, z diventa il figlio sinistro
    } else {
        y->right = z;             // Altrimenti, z diventa il figlio destro
    }

    z->left = T->nil;             // Imposta i figli di z al nodo sentinella NIL
    z->right = T->nil;
    z->color = RED;               // Imposta il colore di z a rosso

    RicettaInsertFix(T, z);        // Effettua il fixup per mantenere le proprietà dell'albero rosso-nero
}

// Funzione per aggiungere un nuovo ingrediente a un nodo ricetta
void inserisciIngredienteRicetta(RecipeNode *nodo, const char *nomeIngrediente, const int quantita) {
    // Alloca spazio per un nuovo array di ingredienti (uno in più)
    nodo->ingredienti = realloc(nodo->ingredienti, (nodo->num_ingredienti + 1) * sizeof(Ingrediente));

    // Aggiungi il nuovo ingrediente alla fine dell'array
    nodo->ingredienti[nodo->num_ingredienti].ingrediente = strdup(nomeIngrediente);
    nodo->ingredienti[nodo->num_ingredienti].quantita = quantita;

    // Incrementa il numero di ingredienti
    nodo->num_ingredienti++;
}

RecipeNode* cercaRicetta(RecipeNode *x, char *key) {
    // Controllo se x è il nodo nil (foglia vuota) o se la chiave è stata trovata
    if (x == catalogo.nil || strcmp(key, x->key) == 0) {
        return x;
    }

    // Se la chiave cercata è minore della chiave del nodo corrente, continua a sinistra
    if (strcmp(key, x->key) < 0) {
        return cercaRicetta(x->left, key);
    }
    // Altrimenti, continua a destra
    else {
        return cercaRicetta(x->right, key);
    }
}

// Funzione che calcola il peso totale degli ingredienti nella ricetta
int calcolaPeso(const RecipeNode *ricetta) {
    int somma = 0;

    for (int i = 0; i < ricetta->num_ingredienti; i++) {
        somma += ricetta->ingredienti[i].quantita;
    }
    return somma;
}

//========================================RIMUOVI=RICETTA========================================================

// Funzione di utilità per liberare la memoria di un nodo ricetta (facoltativo, ma importante)
void liberaRecipeNode(RecipeNode *nodo) {
    for (int i = 0; i < nodo->num_ingredienti; i++) {
        free(nodo->ingredienti[i].ingrediente);
    }
    free(nodo->ingredienti);
}

RecipeNode* treeMinimum(RecipeNode *x) {
    while (x->left != catalogo.nil) {
        x = x->left;
    }
    return x;
}

RecipeNode* treeSuccessor(RecipeNode *x) {
    RecipeNode *y;

    if (x->right != catalogo.nil) {
        return treeMinimum(x->right);
    }
    y = x->parent;
    while (y != catalogo.nil && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

void RicettaDeleteFix(Catalogo *T, RecipeNode *x) {
    RecipeNode *w;

    while (x != T->root && x->color == BLACK) {
        if (x == x->parent->left) {  // Caso in cui x è figlio sinistro
            w = x->parent->right;    // w è il fratello di x
            if (w->color == RED) {   // Caso 1
                w->color = BLACK;    // Caso 1
                x->parent->color = RED; // Caso 1
                leftRotateRicetta(T, x->parent); // Caso 1
                w = x->parent->right; // Caso 1
            }
            if (w->left->color == BLACK && w->right->color == BLACK) { // Caso 2
                w->color = RED;  // Caso 2
                x = x->parent;   // Caso 2
            } else {
                if (w->right->color == BLACK) { // Caso 3
                    w->left->color = BLACK; // Caso 3
                    w->color = RED; // Caso 3
                    rightRotateRicetta(T, w); // Caso 3
                    w = x->parent->right; // Caso 3
                }
                w->color = x->parent->color; // Caso 4
                x->parent->color = BLACK; // Caso 4
                w->right->color = BLACK; // Caso 4
                leftRotateRicetta(T, x->parent); // Caso 4
                x = T->root; // Caso 4
            }
        } else {  // Caso in cui x è figlio destro
            w = x->parent->left;    // w è il fratello di x
            if (w->color == RED) {   // Caso 1
                w->color = BLACK;    // Caso 1
                x->parent->color = RED; // Caso 1
                rightRotateRicetta(T, x->parent); // Caso 1
                w = x->parent->left; // Caso 1
            }
            if (w->right->color == BLACK && w->left->color == BLACK) { // Caso 2
                w->color = RED;  // Caso 2
                x = x->parent;   // Caso 2
            } else {
                if (w->left->color == BLACK) { // Caso 3
                    w->right->color = BLACK; // Caso 3
                    w->color = RED; // Caso 3
                    leftRotateRicetta(T, w); // Caso 3
                    w = x->parent->left; // Caso 3
                }
                w->color = x->parent->color; // Caso 4
                x->parent->color = BLACK; // Caso 4
                w->left->color = BLACK; // Caso 4
                rightRotateRicetta(T, x->parent); // Caso 4
                x = T->root; // Caso 4
            }
        }
    }
    x->color = BLACK;
}

RecipeNode* RimuoviRicetta(RecipeNode *z) {
    RecipeNode *y, *x;

    if (z->left == catalogo.nil || z->right == catalogo.nil) {
        y = z;
    } else {
        y = treeSuccessor(z);
    }

    if (y->left != catalogo.nil) {
        x = y->left;
    } else {
        x = y->right;
    }

    x->parent = y->parent;

    if (y->parent == catalogo.nil) {
        catalogo.nil = x;
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    if (y != z) {
        // Libera la vecchia chiave e lista di ingredienti di z
        free(z->key);  // Libera la vecchia chiave di z
        liberaRecipeNode(z); // Libera la lista di ingredienti di z

        // Copia i dati di y in z
        z->key = strdup(y->key); // Copia della chiave (nome ricetta)
        z->ingredienti = y->ingredienti; // Copia la lista di ingredienti
        z->num_ingredienti = y->num_ingredienti; // Copia il numero di ingredienti
        z->activeOrders = y->activeOrders; // Copia il numero di ordini attivi
    }

    if (y->color == BLACK) {
        RicettaDeleteFix(&catalogo, x);
    }

    // Libera la chiave e la memoria di y prima di deallocarlo
    liberaRecipeNode(y); // Libera la lista di ingredienti di y
    free(y->key);       // Libera la chiave di y
    free(y);             // Dealloca il nodo y

    return y;
}




//==========================FUNZIONI=ALBERO=ORDINI===================================

Ordine* createOrdine( char *nome, int ammount, RecipeNode *associatedRecipe) {
    Ordine *newOrder = (Ordine *)malloc(sizeof(Ordine));

    newOrder->nome = nome;
    newOrder->ammount = ammount;
    newOrder->arrivalTime = tempo;
    newOrder->associatedRecipe = associatedRecipe;
    newOrder->peso = calcolaPeso(associatedRecipe);

    return newOrder;
}

OrderNode* createOrderNode(Ordine* ordine){
    // Allocazione della memoria per il nuovo nodo della ricetta
    OrderNode* orderNode = (OrderNode*)malloc(sizeof(OrderNode));

    // Assegnazione della chiave (tempo arrivo)
    orderNode->key = ordine->arrivalTime;  // Duplica la stringa per la chiave

    // Assegnazione degli ingredienti e il numero di ingredienti
    orderNode->ordinePronto = ordine;

    // Inizializzazione degli attributi del nodo
    orderNode->color = RED;  // Colore iniziale del nodo (RED)
    orderNode->left = orderNode->right = orderNode->parent = ordinazioni.nil;  // Puntatori al nodo sentinella nil

    return orderNode;
}

void inserisciOrdineSospeso(Ordine* ordine)
{
    // Alloca spazio per un nuovo array di ordini (uno in più)
    ordinazioni.sospesi = realloc(ordinazioni.sospesi, (ordinazioni.num_ordinazioni_sospese+ 1) * sizeof(Ordine));

    // Aggiungi il nuovo ordine alla fine dell'array
    ordinazioni.sospesi[ordinazioni.num_ordinazioni_sospese] = *ordine;

    // Incrementa il numero di ordini sospesi
    ordinazioni.num_ordinazioni_sospese++;
}

void leftRotateOrdine(Ordinazioni *T, OrderNode *x) {
    OrderNode *y = x->right;           // y viene settato come il figlio destro di x
    x->right = y->left;                 // il sottoalbero sinistro di y diventa quello destro di x

    if (y->left != T->nil) {
        y->left->parent = x;            // aggiorna il padre del sottoalbero sinistro di y
    }

    y->parent = x->parent;              // attacca il padre di x a y

    if (x->parent == T->nil) {
        T->root = y;                    // se x era la radice, ora la radice è y
    } else if (x == x->parent->left) {
        x->parent->left = y;            // se x era il figlio sinistro, ora y è il figlio sinistro
    } else {
        x->parent->right = y;           // se x era il figlio destro, ora y è il figlio destro
    }

    y->left = x;                        // mette x a sinistra di y
    x->parent = y;                      // aggiorna il padre di x a y
}

void rightRotateOrdine(Ordinazioni *T, OrderNode *y) {
    OrderNode *x = y->left;            // x viene settato come il figlio sinistro di y
    y->left = x->right;                 // il sottoalbero destro di x diventa quello sinistro di y

    if (x->right != T->nil) {
        x->right->parent = y;           // aggiorna il padre del sottoalbero destro di x
    }

    x->parent = y->parent;              // attacca il padre di y a x

    if (y->parent == T->nil) {
        T->root = x;                    // se y era la radice, ora la radice è x
    } else if (y == y->parent->right) {
        y->parent->right = x;           // se y era il figlio destro, ora x è il figlio destro
    } else {
        y->parent->left = x;            // se y era il figlio sinistro, ora x è il figlio sinistro
    }

    x->right = y;                       // mette y a destra di x
    y->parent = x;                      // aggiorna il padre di y a x
}

void OrdinazioneInsertFix(Ordinazioni *T, OrderNode *z) {
    while (z->parent->color == RED) {  // Continua finché il padre di z è rosso
        OrderNode *x = z->parent;

        if (x == x->parent->left) {  // Se x è il figlio sinistro
            OrderNode *y = x->parent->right;  // y è lo zio (fratello del padre di z)

            if (y->color == RED) {  // Caso 1: lo zio y è rosso
                x->color = BLACK;   // Colora il padre di z in nero
                y->color = BLACK;   // Colora lo zio y in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                z = x->parent;  // Sposta z al livello del nonno
            } else {
                if (z == x->right) {  // Caso 2: z è figlio destro
                    z = x;
                    leftRotateOrdine(T, z);  // Effettua una rotazione a sinistra
                }

                // Caso 3: z è figlio sinistro
                x->color = BLACK;  // Colora il padre di z in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                rightRotateOrdine(T, x->parent);  // Effettua una rotazione a destra
            }
        } else {  // Simmetrico: se x è il figlio destro
            OrderNode *y = x->parent->left;  // y è lo zio (fratello del padre di z)

            if (y->color == RED) {  // Caso 1: lo zio y è rosso
                x->color = BLACK;   // Colora il padre di z in nero
                y->color = BLACK;   // Colora lo zio y in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                z = x->parent;  // Sposta z al livello del nonno
            } else {
                if (z == x->left) {  // Caso 2: z è figlio sinistro
                    z = x;
                    rightRotateOrdine(T, z);  // Effettua una rotazione a destra
                }

                // Caso 3: z è figlio destro
                x->color = BLACK;  // Colora il padre di z in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                leftRotateOrdine(T, x->parent);  // Effettua una rotazione a sinistra
            }
        }
    }

    T->root->color = BLACK;  // Assicurati che la radice sia sempre nera
}

void InserisciOrdinePronto(Ordinazioni *T, OrderNode *z) {
    OrderNode *y = T->nil;       // y è il padre del nodo considerato
    OrderNode *x = T->root;      // x è il nodo considerato (inizialmente la radice)

    while (x != T->nil) {         // Trova la posizione corretta per il nuovo nodo z
        y = x;

        // Confronta prima il peso, in caso di parità confronta il tempo di arrivo
        if (z->ordinePronto->peso < x->ordinePronto->peso ||
           (z->ordinePronto->peso == x->ordinePronto->peso && z->ordinePronto->arrivalTime < x->ordinePronto->arrivalTime)) {
            x = x->left;          // Se z ha un peso minore o stesso peso ma arrivo precedente, va a sinistra
           } else {
               x = x->right;         // Altrimenti, va a destra
           }
    }

    z->parent = y;                // Collega il genitore al nuovo nodo z
    if (y == T->nil) {
        T->root = z;              // Se l'albero è vuoto, z diventa la radice
    } else if (z->ordinePronto->peso < y->ordinePronto->peso ||
              (z->ordinePronto->peso == y->ordinePronto->peso && z->ordinePronto->arrivalTime < y->ordinePronto->arrivalTime)) {
        y->left = z;              // Se z ha un peso minore o stesso peso ma arrivo precedente, diventa figlio sinistro
              } else {
                  y->right = z;             // Altrimenti, diventa figlio destro
              }

    z->left = T->nil;             // Imposta i figli di z al nodo sentinella NIL
    z->right = T->nil;
    z->color = RED;               // Imposta il colore di z a rosso

    OrdinazioneInsertFix(T, z);   // Effettua il fixup per mantenere le proprietà dell'albero rosso-nero
}

OrderNode* treeMinimumOrder(OrderNode *x) {
    while (x->left != ordinazioni.nil) {
        x = x->left;
    }
    return x;
}

OrderNode* treeSuccessorOrder(OrderNode *x) {
    OrderNode *y;

    if (x->right != ordinazioni.nil) {
        return treeMinimumOrder(x->right);
    }
    y = x->parent;
    while (y != ordinazioni.nil && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

void OrdineDeleteFix(Ordinazioni *T, OrderNode *x) {
     OrderNode *w;

    while (x != T->root && x->color == BLACK) {
        if (x == x->parent->left) {  // Caso in cui x è figlio sinistro
            w = x->parent->right;    // w è il fratello di x
            if (w->color == RED) {   // Caso 1
                w->color = BLACK;    // Caso 1
                x->parent->color = RED; // Caso 1
                leftRotateOrdine(T, x->parent); // Caso 1
                w = x->parent->right; // Caso 1
            }
            if (w->left->color == BLACK && w->right->color == BLACK) { // Caso 2
                w->color = RED;  // Caso 2
                x = x->parent;   // Caso 2
            } else {
                if (w->right->color == BLACK) { // Caso 3
                    w->left->color = BLACK; // Caso 3
                    w->color = RED; // Caso 3
                    rightRotateOrdine(T, w); // Caso 3
                    w = x->parent->right; // Caso 3
                }
                w->color = x->parent->color; // Caso 4
                x->parent->color = BLACK; // Caso 4
                w->right->color = BLACK; // Caso 4
                leftRotateOrdine(T, x->parent); // Caso 4
                x = T->root; // Caso 4
            }
        } else {  // Caso in cui x è figlio destro
            w = x->parent->left;    // w è il fratello di x
            if (w->color == RED) {   // Caso 1
                w->color = BLACK;    // Caso 1
                x->parent->color = RED; // Caso 1
                rightRotateOrdine(T, x->parent); // Caso 1
                w = x->parent->left; // Caso 1
            }
            if (w->right->color == BLACK && w->left->color == BLACK) { // Caso 2
                w->color = RED;  // Caso 2
                x = x->parent;   // Caso 2
            } else {
                if (w->left->color == BLACK) { // Caso 3
                    w->right->color = BLACK; // Caso 3
                    w->color = RED; // Caso 3
                    leftRotateOrdine(T, w); // Caso 3
                    w = x->parent->left; // Caso 3
                }
                w->color = x->parent->color; // Caso 4
                x->parent->color = BLACK; // Caso 4
                w->left->color = BLACK; // Caso 4
                rightRotateOrdine(T, x->parent); // Caso 4
                x = T->root; // Caso 4
            }
        }
    }
    x->color = BLACK;
}

void removeLoadedOrder(Ordinazioni *T, OrderNode *z) {
    OrderNode *y, *x;

    if (z->left == T->nil || z->right == T->nil) {
        y = z;  // Il nodo da eliminare è z se ha un figlio NIL
    } else {
        y = treeSuccessorOrder(z);  // Trova il successore
    }

    if (y->left != T->nil) {
        x = y->left;
    } else {
        x = y->right;
    }

    x->parent = y->parent;

    if (y->parent == T->nil) {
        T->root = x;  // Se y era la radice, x diventa la nuova radice
    } else if (y == y->parent->left) {
        y->parent->left = x;
    } else {
        y->parent->right = x;
    }

    if (y != z) {
        // Libera la memoria dell'ordine associato a z
        if (z->ordinePronto) {
            free(z->ordinePronto->nome);  // Libera il nome dell'ordine
            free(z->ordinePronto);  // Libera la struttura Ordine
        }

        // Copia i dati di y in z
        z->ordinePronto = y->ordinePronto;
        z->key = y->key;
    }

    if (y->color == BLACK) {
        OrdineDeleteFix(T, x);
    }

    // Libera la memoria del nodo y
    free(y);  // Dealloca il nodo y
}


//TODO qui


//ricalcola i puntatori delgli ordini alle ricette se una ricetta e stata eliminata
// Funzione per aggiornare la ricetta associata ad un singolo nodo
void aggiornaRicettaPerNodo(OrderNode *nodo, RecipeNode *rootRicette) {
    if (nodo == NULL) {
        return; // Condizione base: nodo nullo
    }

    // Cerca la ricetta associata al nome dell'ordine
    if (nodo->ordine.associatedRecipe != NULL && nodo->ordine.nome != NULL) {
        nodo->ordine.associatedRecipe = searchRicetta(rootRicette, nodo->ordine.nome);
    }

    // Visita ricorsivamente i sottoalberi sinistro e destro
    aggiornaRicettaPerNodo(nodo->left, rootRicette);
    aggiornaRicettaPerNodo(nodo->right, rootRicette);
}

// Funzione principale per aggiornare tutte le ricette nell'albero delle ordinazioni
void updateOrderRecipeIndex(Ordinazioni *ordinazioni, RecipeNode *rootRicette) {
    if (ordinazioni == NULL || ordinazioni->ordinazioni == NULL) {
        return; // Se non ci sono ordinazioni o se l'albero è vuoto, esci dalla funzione
    }

    // Avvia l'aggiornamento partendo dalla radice dell'albero delle ordinazioni
    aggiornaRicettaPerNodo(ordinazioni->ordinazioni, rootRicette);
}

//===================================MAGAZZINO=================================

ResourceNode* createResourceNode(const char* key) {
    // Allocazione della memoria per il nuovo nodo della ricetta
    ResourceNode* resourcenode = (ResourceNode*)malloc(sizeof(ResourceNode));

    // Assegnazione della chiave (nome della ricetta)
    resourcenode->key = strdup(key);  // Duplica la stringa per la chiave

    // Assegnazione degli ingredienti e il numero di ingredienti
    resourcenode->lotti = NULL;
    resourcenode->num_lotti = 0;
    resourcenode->maxGrammi = 0;

    // Inizializzazione degli attributi del nodo
    resourcenode->color = RED;  // Colore iniziale del nodo (RED)
    resourcenode->left = resourcenode->right = resourcenode->parent = magazzino.nil;  // Puntatori al nodo sentinella nil

    return resourcenode;
}

ResourceNode* cercaIngredienteMagazzino(ResourceNode *x, char *key) {
    // Controllo se x è il nodo nil (foglia vuota) o se la chiave è stata trovata
    if (x == magazzino.nil || strcmp(key, x->key) == 0) {
        return x;
    }

    // Se la chiave cercata è minore della chiave del nodo corrente, continua a sinistra
    if (strcmp(key, x->key) < 0) {
        return cercaIngredienteMagazzino(x->left, key);
    }
    // Altrimenti, continua a destra
    else {
        return cercaIngredienteMagazzino(x->right, key);
    }
}

void inserisciLottoMagazzino(ResourceNode *nodo, int quantita, int scadenza) {
    // Cerca se esiste già un lotto con la stessa data di scadenza
    for (int i = 0; i < nodo->num_lotti; i++) {
        if (nodo->lotti[i].scadenza == scadenza) {
            // Se esiste, somma la quantità al lotto esistente
            nodo->lotti[i].ammount += quantita;
            nodo->maxGrammi += quantita;
            return;
        }
    }

    // Se non esiste un lotto con la stessa data di scadenza,
    // alloca spazio per un nuovo array di lotti (uno in più)
    nodo->lotti = realloc(nodo->lotti, (nodo->num_lotti + 1) * sizeof(Lotto));

    int i;
    for (i = nodo->num_lotti; i > 0 && nodo->lotti[i-1].scadenza > scadenza; i--) {
        nodo->lotti[i] = nodo->lotti[i-1]; // Sposta i lotti esistenti per fare spazio
    }

    // Inserisci il nuovo lotto nella posizione corretta
    nodo->lotti[i].ammount = quantita;
    nodo->lotti[i].scadenza = scadenza;

    // Incrementa il numero di lotti
    nodo->num_lotti++;

    // Aggiorna il totale degli ingredienti
    nodo->maxGrammi += quantita;
}

void leftRotateResource(Magazzino *T, ResourceNode *x) {
    ResourceNode *y = x->right;           // y viene settato come il figlio destro di x
    x->right = y->left;                 // il sottoalbero sinistro di y diventa quello destro di x

    if (y->left != T->nil) {
        y->left->parent = x;            // aggiorna il padre del sottoalbero sinistro di y
    }

    y->parent = x->parent;              // attacca il padre di x a y

    if (x->parent == T->nil) {
        T->root = y;                    // se x era la radice, ora la radice è y
    } else if (x == x->parent->left) {
        x->parent->left = y;            // se x era il figlio sinistro, ora y è il figlio sinistro
    } else {
        x->parent->right = y;           // se x era il figlio destro, ora y è il figlio destro
    }

    y->left = x;                        // mette x a sinistra di y
    x->parent = y;                      // aggiorna il padre di x a y
}

void rightRotateResource(Magazzino *T, ResourceNode *y) {
    ResourceNode *x = y->left;            // x viene settato come il figlio sinistro di y
    y->left = x->right;                 // il sottoalbero destro di x diventa quello sinistro di y

    if (x->right != T->nil) {
        x->right->parent = y;           // aggiorna il padre del sottoalbero destro di x
    }

    x->parent = y->parent;              // attacca il padre di y a x

    if (y->parent == T->nil) {
        T->root = x;                    // se y era la radice, ora la radice è x
    } else if (y == y->parent->right) {
        y->parent->right = x;           // se y era il figlio destro, ora x è il figlio destro
    } else {
        y->parent->left = x;            // se y era il figlio sinistro, ora x è il figlio sinistro
    }

    x->right = y;                       // mette y a destra di x
    y->parent = x;                      // aggiorna il padre di y a x
}

void ResourceInsertFix(Magazzino *T, ResourceNode *z) {
    while (z->parent->color == RED) {  // Continua finché il padre di z è rosso
        ResourceNode *x = z->parent;

        if (x == x->parent->left) {  // Se x è il figlio sinistro
            ResourceNode *y = x->parent->right;  // y è lo zio (fratello del padre di z)

            if (y->color == RED) {  // Caso 1: lo zio y è rosso
                x->color = BLACK;   // Colora il padre di z in nero
                y->color = BLACK;   // Colora lo zio y in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                z = x->parent;  // Sposta z al livello del nonno
            } else {
                if (z == x->right) {  // Caso 2: z è figlio destro
                    z = x;
                    leftRotateResource(T, z);  // Effettua una rotazione a sinistra
                }

                // Caso 3: z è figlio sinistro
                x->color = BLACK;  // Colora il padre di z in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                rightRotateResource(T, x->parent);  // Effettua una rotazione a destra
            }
        } else {  // Simmetrico: se x è il figlio destro
            ResourceNode *y = x->parent->left;  // y è lo zio (fratello del padre di z)

            if (y->color == RED) {  // Caso 1: lo zio y è rosso
                x->color = BLACK;   // Colora il padre di z in nero
                y->color = BLACK;   // Colora lo zio y in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                z = x->parent;  // Sposta z al livello del nonno
            } else {
                if (z == x->left) {  // Caso 2: z è figlio sinistro
                    z = x;
                    rightRotateResource(T, z);  // Effettua una rotazione a destra
                }

                // Caso 3: z è figlio destro
                x->color = BLACK;  // Colora il padre di z in nero
                x->parent->color = RED;  // Colora il nonno in rosso
                leftRotateResource(T, x->parent);  // Effettua una rotazione a sinistra
            }
        }
    }

    T->root->color = BLACK;  // Assicurati che la radice sia sempre nera
}

void aggiungiIngredienteMagazzino(Magazzino *T, ResourceNode *z)
{
    ResourceNode *y = T->nil;       // y è il padre del nodo considerato
    ResourceNode *x = T->root;      // x è il nodo considerato (inizialmente la radice)

    while (x != T->nil) {         // Trova la posizione corretta per il nuovo nodo z
        y = x;
        if (strcmp(z->key, x->key) < 0) // Confronta le chiavi per decidere la direzione
            x = x->left;          // Se la chiave di z è minore, si muove a sinistra
        else
            x = x->right;         // Altrimenti, si muove a destra
    }

    z->parent = y;                // Collega il genitore al nuovo nodo z
    if (y == T->nil) {
        T->root = z;              // Se l'albero è vuoto, z diventa la radice
    } else if (strcmp(z->key, y->key) < 0) {
        y->left = z;              // Se la chiave di z è minore, z diventa il figlio sinistro
    } else {
        y->right = z;             // Altrimenti, z diventa il figlio destro
    }

    z->left = T->nil;             // Imposta i figli di z al nodo sentinella NIL
    z->right = T->nil;
    z->color = RED;               // Imposta il colore di z a rosso

    T->num_risorse++;

    ResourceInsertFix(T, z);        // Effettua il fixup per mantenere le proprietà dell'albero rosso-nero
}

//TODO QUI

void riparaAlberoIngredienteRM(ResourceNode **root, ResourceNode *x) {
    // Controlla se x è NULL all'inizio del ciclo
    while (x != *root && (x == NULL || x->color == BLACK)) {
        if (x == NULL) {
            // In questo caso, x è NULL e non possiamo fare nulla
            break;
        }

        if (x == x->parent->left) {
            ResourceNode *w = x->parent->right;

            // Caso 1: Il fratello w è rosso
            if (w != NULL && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                ruotaSinistra(root, x->parent);
                w = x->parent->right;
            }

            // Caso 2: Entrambi i figli di w sono neri
            if (w != NULL && (w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                // Caso 3: Il figlio destro di w è nero e il figlio sinistro è rosso
                if (w != NULL && (w->right == NULL || w->right->color == BLACK)) {
                    if (w->left != NULL) {
                        w->left->color = BLACK;
                    }
                    w->color = RED;
                    ruotaDestra(root, w);
                    w = x->parent->right;
                }

                // Caso 4: Il figlio destro di w è rosso
                if (w != NULL) {
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->right != NULL) {
                        w->right->color = BLACK;
                    }
                    ruotaSinistra(root, x->parent);
                    x = *root;
                }
            }
        } else {
            // Simmetrico al caso sopra, ma per il lato destro
            ResourceNode *w = x->parent->left;

            // Caso 1: Il fratello w è rosso
            if (w != NULL && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                ruotaDestra(root, x->parent);
                w = x->parent->left;
            }

            // Caso 2: Entrambi i figli di w sono neri
            if (w != NULL && (w->left == NULL || w->left->color == BLACK) &&
                (w->right == NULL || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                // Caso 3: Il figlio sinistro di w è nero e il figlio destro è rosso
                if (w != NULL && (w->left == NULL || w->left->color == BLACK)) {
                    if (w->right != NULL) {
                        w->right->color = BLACK;
                    }
                    w->color = RED;
                    ruotaSinistra(root, w);
                    w = x->parent->left;
                }

                // Caso 4: Il figlio sinistro di w è rosso
                if (w != NULL) {
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->left != NULL) {
                        w->left->color = BLACK;
                    }
                    ruotaDestra(root, x->parent);
                    x = *root;
                }
            }
        }
    }

    if (x != NULL) {
        x->color = BLACK;
    }
}


void trapiantaNodo(ResourceNode **root, ResourceNode *u, ResourceNode *v) {
    if (u->parent == NULL) {
        *root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    if (v != NULL) {
        v->parent = u->parent;
    }
}

ResourceNode *trovaMinimoNodo(ResourceNode *node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}


void rimuoviNodo(ResourceNode **root, ResourceNode *z) {
    ResourceNode *y = z;
    ResourceNode *x;
    Color yOriginalColor = y->color;

    if (z->left == NULL) {
        x = z->right;
        trapiantaNodo(root, z, z->right);
    } else if (z->right == NULL) {
        x = z->left;
        trapiantaNodo(root, z, z->left);
    } else {
        y = trovaMinimoNodo(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z) {
            if (x != NULL) {
                x->parent = y;
            }
        } else {
            trapiantaNodo(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        trapiantaNodo(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (yOriginalColor == BLACK) {
        riparaAlberoIngredienteRM(root, x);
    }

    free(z);
}

//============================PREPARA=ORDINI===========================

void verificaScadenzaLotti(Lotto **root, ResourceNode *ingredienteMagazzino, int time, int *trovatoScaduto, int *trovatoNonScaduto) {
    if (*root == NULL || *trovatoScaduto || *trovatoNonScaduto) {
        return;
    }

    // Controlla prima il sottoalbero sinistro
    verificaScadenzaLotti(&(*root)->left, ingredienteMagazzino, time, trovatoScaduto, trovatoNonScaduto);

    if (*trovatoScaduto || *trovatoNonScaduto) {
        return;
    }

    // Controllo del nodo corrente
    if ((*root)->scadenza <= time) {
        Lotto *nodoDaRimuovere = *root;

        // Decrementa il numero di lotti disponibili
        ingredienteMagazzino->num_lotti--;

        // Aggiorna il totale di grammi disponibili per l'ingrediente
        ingredienteMagazzino->maxGrammi -= nodoDaRimuovere->ammount;

        // Rimuove il lotto scaduto
        rimuoviLotto(root, nodoDaRimuovere, ingredienteMagazzino);

        *trovatoScaduto = 1;
        return;
    }

    if ((*root)->scadenza > time) {
        *trovatoNonScaduto = 1;
        return;
    }

    // Controlla il sottoalbero destro
    verificaScadenzaLotti(&(*root)->right, ingredienteMagazzino, time, trovatoScaduto, trovatoNonScaduto);
}

void verificaScadenza(ResourceNode *resource_node, int time) {
    if (resource_node == NULL) {
        return;
    }

    ResourceNode *currentNode = resource_node;

    while (currentNode != NULL) {
        int trovatoScaduto = 0;
        int trovatoNonScaduto = 0;

        verificaScadenzaLotti(&currentNode->lotti, currentNode, time, &trovatoScaduto, &trovatoNonScaduto);

        // Se il nodo corrente non ha più lotti, rimuovilo e aggiorna currentNode
        if (currentNode->num_lotti == 0) {
            ResourceNode *nodoDaRimuovere = currentNode;

            // Trova il successore per continuare l'iterazione
            if (currentNode->right != NULL) {
                currentNode = currentNode->right;
                while (currentNode->left != NULL) {
                    currentNode = currentNode->left;
                }
            } else {
                ResourceNode *parent = nodoDaRimuovere->parent;
                while (parent != NULL && nodoDaRimuovere == parent->right) {
                    nodoDaRimuovere = parent;
                    parent = parent->parent;
                }
                currentNode = parent;
            }

            // Rimuovi il nodo ResourceNode e bilancia l'albero
            rimuoviNodo(&resource_node, nodoDaRimuovere);
        } else {
            // Passa al nodo successivo
            if (currentNode->right != NULL) {
                currentNode = currentNode->right;
                while (currentNode->left != NULL) {
                    currentNode = currentNode->left;
                }
            } else {
                ResourceNode *parent = currentNode->parent;
                while (parent != NULL && currentNode == parent->right) {
                    currentNode = parent;
                    parent = parent->parent;
                }
                currentNode = parent;
            }
        }
    }
}





void verificaQuantitaLotti(Lotto **root, ResourceNode *ingredienteMagazzino, int *trovatoEsaurito, int *trovatoNonVuoto) {
    if (*root == NULL || *trovatoEsaurito || *trovatoNonVuoto) {
        return;
    }

    // Controlla prima il sottoalbero sinistro
    verificaQuantitaLotti(&(*root)->left, ingredienteMagazzino, trovatoEsaurito, trovatoNonVuoto);

    // Se un lotto esaurito o non vuoto è stato trovato, termina la ricerca
    if (*trovatoEsaurito || *trovatoNonVuoto) {
        return;
    }

    // Controllo del nodo corrente
    if ((*root)->ammount == 0) {
        Lotto *nodoDaRimuovere = *root;

        // Rimuove il lotto esaurito
        rimuoviLotto(root, nodoDaRimuovere, ingredienteMagazzino);

        // Decrementa il numero di lotti disponibili
        ingredienteMagazzino->num_lotti--;

        // Lotto esaurito trovato, si può terminare la ricerca per questo ingrediente
        *trovatoEsaurito = 1;
        return;
    } else if ((*root)->ammount > 0) {
        // Lotto non vuoto trovato, si può terminare la ricerca per questo ingrediente
        *trovatoNonVuoto = 1;
        return;
    }

    // Se non è stato trovato un lotto esaurito o non vuoto, controlla il sottoalbero destro
    verificaQuantitaLotti(&(*root)->right, ingredienteMagazzino, trovatoEsaurito, trovatoNonVuoto);
}

void verificaQuantita(ResourceNode *resource_node) {
    if (resource_node == NULL) {
        return;
    }

    ResourceNode *currentNode = resource_node;
    ResourceNode *nextNode;

    while (currentNode != NULL) {
        int trovatoEsaurito = 0;
        int trovatoNonVuoto = 0;
        verificaQuantitaLotti(&currentNode->lotti, currentNode, &trovatoEsaurito, &trovatoNonVuoto);

        // Se il nodo corrente non ha più lotti, rimuovilo e aggiorna currentNode
        if (currentNode->num_lotti == 0) {
            ResourceNode *nodoDaRimuovere = currentNode;

            // Trova il successore per continuare l'iterazione
            if (nodoDaRimuovere->right != NULL) {
                nextNode = nodoDaRimuovere->right;
                while (nextNode->left != NULL) {
                    nextNode = nextNode->left;
                }
            } else {
                nextNode = nodoDaRimuovere->parent;
                while (nextNode != NULL && nodoDaRimuovere == nextNode->right) {
                    nodoDaRimuovere = nextNode;
                    nextNode = nextNode->parent;
                }
            }

            // Rimuovi il nodo ResourceNode e bilancia l'albero
            rimuoviNodo(&resource_node, nodoDaRimuovere);

            // Dopo la rimozione, aggiorna currentNode
            currentNode = nextNode;
        } else {
            // Passa al nodo successivo
            if (currentNode->right != NULL) {
                nextNode = currentNode->right;
                while (nextNode->left != NULL) {
                    nextNode = nextNode->left;
                }
            } else {
                nextNode = currentNode->parent;
                while (nextNode != NULL && currentNode == nextNode->right) {
                    currentNode = nextNode;
                    nextNode = nextNode->parent;
                }
            }
            currentNode = nextNode;
        }
    }
}

int calcolaFattibilita(const Ordine *orderToProcess) {

    RecipeNode *associatedRecipe = orderToProcess->associatedRecipe;
    int num_ingredienti = associatedRecipe->num_ingredienti;

    // Per ogni ingrediente della ricetta, verifica se è disponibile nel magazzino in quantità sufficiente
    for (int i = 0; i < num_ingredienti; i++) {

        Ingrediente ingredienteNecessario = associatedRecipe->ingredienti[i];

        // Cerca l'ingrediente nel magazzino
        ResourceNode *ingredienteMagazzino = cercaIngredienteMagazzino(magazzino.root, ingredienteNecessario.ingrediente);
        if (ingredienteMagazzino == magazzino.nil) {
            // Se l'ingrediente non esiste nel magazzino, restituisce false
            return 0;
        }

        // Verifica se la quantità disponibile è sufficiente
        if (ingredienteMagazzino->maxGrammi < ingredienteNecessario.quantita) {
            // Se la quantità non è sufficiente, restituisce false
            return 0;
        }
    }

    // Se tutti gli ingredienti sono disponibili in quantità sufficiente, restituisce true
    return 1;
}



// Funzione ricorsiva per consumare lotti in ordine di scadenza
void consumaLottiPerScadenza(Lotto *lotti, int num_lotti, int *grammiRichiesti, int *maxGrammi) {
    for (int i = 0; i < num_lotti && *grammiRichiesti > 0; i++) {
        Lotto *nodoLotto = &lotti[i];

        // Consuma dal lotto corrente
        if (*grammiRichiesti <= nodoLotto->ammount) {
            nodoLotto->ammount -= *grammiRichiesti;
            *maxGrammi -= *grammiRichiesti;
            *grammiRichiesti = 0; // Tutto soddisfatto, interrompi il loop
        } else {
            *grammiRichiesti -= nodoLotto->ammount;
            *maxGrammi -= nodoLotto->ammount;
            nodoLotto->ammount = 0; // Lotto esaurito, passa al successivo
        }
    }
}



// Funzione principale per preparare l'ordine
void preparaOrdine(Ordine *nuovoOrdine, RecipeNode *Recipe ) {
    RecipeNode *associatedRecipe = Recipe;

        // Itera attraverso gli ingredienti della ricetta
        for (int i = 0; i < associatedRecipe->num_ingredienti; i++) {
            Ingrediente *ingredienteCorrente = &associatedRecipe->ingredienti[i];

            // Trova l'ingrediente nel magazzino utilizzando l'albero rosso-nero
            ResourceNode *ingredienteMagazzino = cercaIngredienteMagazzino(magazzino.root, ingredienteCorrente->ingrediente);

            // Calcola la quantità richiesta per l'ordine
            int grammiRichiesti = ingredienteCorrente->quantita * nuovoOrdine->ammount;

            // Consuma lotti in ordine di scadenza
            consumaLottiPerScadenza(ingredienteMagazzino->lotti,ingredienteMagazzino->num_lotti, &grammiRichiesti, &ingredienteMagazzino->maxGrammi);
        }
    // Se tutto è andato a buon fine, incrementa il numero di ordinazioni pronte
    ordinazioni.num_ordinazioni_pronte++;
}

//TODO OK

int processaOrdiniSospeso(OrderNode *node) {
    int trovato = 0;  // Variabile per memorizzare se un ordine realizzabile è stato trovato

    if (node == NULL) {
        return 0; // Restituisce 0 se il nodo è nullo
    }

    // Visita del sottoalbero sinistro (più piccolo arrivalTime)
    trovato |= processaOrdiniSospeso(node->left);

    // Ricontrolla la fattibilità dell'ordine
    if (node->ordine.status == 1) {

        node->ordine.status = calcolaFattibilita(node->ordine.associatedRecipe, node->ordine.ammount);
        if (node->ordine.status == 1) {
            trovato = 1; // Segnala che è stato trovato un ordine con fattibilità ricalcolata = 1
        } else if (node->ordine.status == 0) {
            preparaOrdine(&node->ordine, node->ordine.associatedRecipe);
        }
    }

    // Visita del sottoalbero destro (più grande arrivalTime)
    trovato |= processaOrdiniSospeso(node->right);

    return trovato; // Restituisce 1 se è stato trovato almeno un ordine con fattibilità ricalcolata = 1, altrimenti 0
}



//=============================FUNZIONI=DI=STAMPA=TESTING==================================

// Funzione per fare una traversata in ordine dell'albero degli ingredienti
void inOrderIngredientTraversal(const IngredientNode *root) {
    if (root != NULL) {
        inOrderIngredientTraversal(root->left);
        printf("    - %s: %d\n", root->ingrediente, root->quantita);
        inOrderIngredientTraversal(root->right);
    }
}

// Funzione per fare una traversata in ordine dell'albero delle ricette
void inOrderRecipeTraversal(const RecipeNode *root) {
    if (root != NULL) {
        inOrderRecipeTraversal(root->left);

        // Stampa il nome della ricetta
        printf("Ricetta: %s (%d Ord sospesi)\n", root->ricetta->nome, root->ricetta->activeOrders);
        printf("Ingredienti:\n");

        // Stampa gli ingredienti della ricetta
        inOrderIngredientTraversal(root->ricetta->root);
        printf("\n");

        inOrderRecipeTraversal(root->right);
    }
}

// Funzione per stampare tutte le ricette nel catalogo
void printCatalogo(const Catalogo *catalogo) {

    printf("\n\n>CATALOGO (%d recipes)<\n",catalogo->num_ricette);

    if (catalogo->root != NULL) {
        inOrderRecipeTraversal(catalogo->root);
    } else {
        printf("Il catalogo è vuoto.\n");
    }
}

// Funzione per stampare tutti i lotti di un ingrediente
void stampaLotti(const Lotto *lotto) {
    if (lotto == NULL) {
        return;
    }

    // Traversata in-order dell'albero dei lotti
    stampaLotti(lotto->left);
    printf("  Lotto - Quantita: %d grammi, Scadenza: %d\n", lotto->ammount, lotto->scadenza);
    stampaLotti(lotto->right);
}

// Funzione per stampare gli ingredienti con i loro relativi lotti
void stampaIngredienti(ResourceNode *nodo) {
    if (nodo == NULL) {
        return;
    }

    // Traversata in-order dell'albero degli ingredienti
    stampaIngredienti(nodo->left);
    printf("Ingrediente: %s\n", nodo->nome);
    printf("  Totale: %d grammi, Numero di lotti: %d\n", nodo->maxGrammi, nodo->num_lotti);

    // Stampa dei lotti per l'ingrediente corrente
    stampaLotti(nodo->lotti);

    stampaIngredienti(nodo->right);
}

// Funzione principale per stampare tutti gli ingredienti nel magazzino
void stampaMagazzino(const Magazzino *magazzino) {

    if (magazzino == NULL || magazzino->ingredienti == NULL) {
        printf("Il magazzino è vuoto.\n");
        return;
    }

    printf("\n\n>MAGAZZINO (%d ingredients)<\n",magazzino->num_ingredienti);

    stampaIngredienti(magazzino->ingredienti);
}

// Funzione ricorsiva per stampare l'albero
void printOrderNode(const OrderNode *node) {
    if (node == NULL) {
        return;
    }

    // Visita del sottoalbero sinistro
    printOrderNode(node->left);

    // Stampa del nodo corrente
    printf("Nome Ordine: %s\n", node->ordine.nome);
    printf("Quantita: %d\n", node->ordine.ammount);
    printf("Peso: %d\n", node->ordine.peso);
    printf("Status: %d\n", node->ordine.status);
    printf("Tempo di Arrivo: %d\n", node->ordine.arrivalTime);
    printf("----------------------------\n");

    // Visita del sottoalbero destro
    printOrderNode(node->right);
}

// Funzione principale per stampare tutte le ordinazioni
void printOrdinazioni(Ordinazioni *ordinazioni) {
    if (ordinazioni->ordinazioni == NULL) {
        printf("Nessuna ordinazione disponibile.\n");
        return;
    }
    printf("\n\n>Ordinazioni (%d ordini)<\n",ordinazioni->num_ordinazioni_pronte);
    printOrderNode(ordinazioni->ordinazioni);
}

void stampaOrdini(Ordine* ordiniCaricare, int numPacchi) {
    for (int i = 0; i < numPacchi; i++) {
        printf("%d %s %d\n", ordiniCaricare[i].arrivalTime,ordiniCaricare[i].nome,ordiniCaricare[i].ammount);

    }
}

//=========================================CORRIERE=====================================================================


void caricaOrdiniInOrder(Ordinazioni *T, OrderNode *nodo, OrderNode *nil, int *caricoCorrente, int caricoMassimo) {
    if (nodo == nil || *caricoCorrente + nodo->ordinePronto->peso > caricoMassimo) {
        return;
    }

    //Visita il sottoalbero sinistro
    caricaOrdiniInOrder(T, nodo->left, nil, caricoCorrente, caricoMassimo);

    // Gestisci il nodo corrente
    if (*caricoCorrente + nodo->ordinePronto->peso <= caricoMassimo) {
        *caricoCorrente += nodo->ordinePronto->peso;
        printf("%d %s %d\n",nodo->ordinePronto->arrivalTime,nodo->ordinePronto->nome,nodo->ordinePronto->peso);
        removeLoadedOrder(&ordinazioni, nodo);
    } else {
        return;  // Se il prossimo ordine supera il carico massimo, interrompi
    }

    // Visita il sottoalbero destro
    caricaOrdiniInOrder(T, nodo->right, nil, caricoCorrente, caricoMassimo);
}

void caricaOrdiniSuCorriere() {
    caricaOrdiniInOrder(&ordinazioni, ordinazioni.root, ordinazioni.nil, &corriere.caricoParziale, corriere.caricoMax);
}

//=========================================INPUT========================================================================
// Funzione per processare l'input
void processInput(const char *input) {
    char *input_copy = strdup(input); // Copia dell'input originale
    char *input_copy2 = strdup(input_copy);
    char *token = strtok(input_copy, " ");

    //comando non valido
    if ((strcmp(token, "aggiungi_ricetta") != 0) &&
        (strcmp(token, "rimuovi_ricetta") != 0) &&
        (strcmp(token, "rifornimento") != 0) &&
        (strcmp(token, "ordine") != 0)) {
        printf("time: %d -> Comando non riconosciuto! (>%s<)\n", tempo, token);
        free(input_copy);
        free(input_copy2);
        return;  // Usa return invece di abort per assicurarti di liberare memoria
    }

    // Testing
    //printf("time: %d -> %s\n", time, token);

    // Identifica il comando (4 comandi)
    if (strcmp(token, "aggiungi_ricetta") == 0) {
        // Recupera il nome della ricetta
        char *nome_ricetta = strtok(NULL, " ");

        // Verifica ricetta già esistente
        if (cercaRicetta(catalogo.root, nome_ricetta) != catalogo.nil) {  // Usa NULL per puntatori
            printf("ignorato\n");
            free(input_copy);
            free(input_copy2);
            return;
        }

        // Conta il numero di ingredienti e quantità
        int num_ingredienti = 0;
        char *nome_ingrediente = NULL;
        char *ammount_str = NULL;

        // Itera attraverso il resto della stringa per ottenere gli ingredienti
        while ((nome_ingrediente = strtok(NULL, " ")) != NULL &&
               (ammount_str = strtok(NULL, " ")) != NULL) {
            num_ingredienti++;
        }

        // Creazione di una nuova ricetta
        RecipeNode *nuovaRicetta = createRecipeNode(nome_ricetta);


        // Resetta `strtok` per rileggere gli ingredienti e quantità
        strtok(input_copy2, " ");  // Resetta strtok sulla copia
        strtok(NULL, " ");         // Salta il nome della ricetta

        // Aggiungi gli ingredienti alla ricetta
        for (int i = 0; i < num_ingredienti; i++) {
            nome_ingrediente = strtok(NULL, " ");
            ammount_str = strtok(NULL, " ");
            int ammount = atoi(ammount_str);

            // Aggiungi l'ingrediente alla ricetta
            inserisciIngredienteRicetta(nuovaRicetta, nome_ingrediente, ammount);
        }

        // Aggiunta ricetta al ricettario
        InserisciRicetta(&catalogo, nuovaRicetta);
        printf("aggiunta\n");

        // Libera memoria temporanea
        free(input_copy);
        free(input_copy2);
        return;
    }


    if (strcmp(token, "rimuovi_ricetta") == 0) {
        // Recupera il nome della ricetta
        char *nome_ricetta = strtok(NULL, " ");

        RecipeNode *recipeToKill = cercaRicetta(catalogo.root, nome_ricetta);

        if(recipeToKill==catalogo.nil)
        {
           printf("non presente\n");
           free(input_copy);
           return;
        }

        if(recipeToKill->activeOrders!=0){
           printf("ordini in sospeso\n");
           free(input_copy);
           return;
        }

        RimuoviRicetta(recipeToKill);
        updateOrderRecipeIndex(&ordinazionicatalogo.root); //TODO modificare
        free(input_copy);
        printf("rimossa\n");
        return;
    }

    if (strcmp(token, "rifornimento") == 0) {

    // Conta il numero di lotti da immagazzinare
    int num_lotti = 0;
    char *lotto_ingrediente = NULL;
    const char *ammount_str = NULL;
    const char *scadenza_str = NULL;

    // Itera attraverso il resto della stringa per ottenere il numero dei nuovi lotti da immagazzinare
    while ((lotto_ingrediente = strtok(NULL, " ")) != NULL && (ammount_str = strtok(NULL, " ")) != NULL && (scadenza_str = strtok(NULL, " ")) != NULL) {
        num_lotti++;
    }

    strtok(input_copy2, " ");  // Resetta strtok sulla copia

    for (int i = 0; i < num_lotti; i++) {
        lotto_ingrediente = strtok(NULL, " ");
        ammount_str = strtok(NULL, " ");
        scadenza_str = strtok(NULL, " ");
        int ammount = atoi(ammount_str);
        int scadenza = atoi(scadenza_str);

        if(scadenza>tempo) {
           // Aggiungi i nuovi lotti al magazzino
           ResourceNode *newLottoToAdd = cercaIngredienteMagazzino(magazzino.root,lotto_ingrediente);
           if(newLottoToAdd==magazzino.nil)
           {
               newLottoToAdd =  createResourceNode(lotto_ingrediente);
               aggiungiIngredienteMagazzino(&magazzino,newLottoToAdd);
           }
           inserisciLottoMagazzino(newLottoToAdd,ammount,scadenza);
        }
    }
        free(input_copy);
        free(input_copy2);
        printf("rifornito\n");

        processaOrdiniSospeso(ordinazioni.ordinazioni); //TODO

        return;
    }

    if (strcmp(token, "ordine") == 0) {
        // Recupera il nome della ricetta associata all'ordine
        char *nome_ricetta = strtok(NULL, " ");

        RecipeNode *associatedRecipe = cercaRicetta(catalogo.root, nome_ricetta);

        if (associatedRecipe == catalogo.nil) {
            printf("rifiutato\n");
        } else {
            printf("accettato\n");
            const int ammount = atoi(strtok(NULL, " "));
            Ordine *nuovoOrdine = createOrdine(nome_ricetta, ammount, associatedRecipe);
            if(calcolaFattibilita(nuovoOrdine)) {
                preparaOrdine(nuovoOrdine, associatedRecipe);
                InserisciOrdinePronto(&ordinazioni,createOrderNode(nuovoOrdine));
            }else
            {
                inserisciOrdineSospeso(nuovoOrdine);
            }

            associatedRecipe->activeOrders++;

            // Libera la memoria allocata per `nuovoOrdine`
            free(nuovoOrdine);
        }

        // Libera la memoria allocata per `input_copy`
        free(input_copy);
        return;
    }


}


int main() {

    //inizializzazione degli alberi
    initCatalogo(&catalogo);
    initMagazzino(&magazzino);
    initOrdinazioni(&ordinazioni);


    clock_t start = clock();
    char buffer[BUFFER_SIZE];

    //TODO
    // Usa stdin come file di input
    //  FILE *file = stdin;
    // //
    // // Usa stdout come file di output
    // FILE *file2 = stdout;
    //


    FILE *file = fopen("example.txt", "r");
    FILE *file2 = fopen("TestOut.txt", "w");

    if (file == NULL) {
        perror("Errore nell'apertura del file");
        return EXIT_FAILURE;
    }


    if (file2 == NULL) {
        printf("Errore nell'apertura del file!\n");
        return 1;
    }



    int timeSpedizioni = 0;

     // Lettura della prima riga
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        const size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        corriere.timeCorriere = atoi(strtok(buffer, " "));
        corriere.caricoMax = atoi(strtok(NULL, " "));

        corriere.caricoParziale = 0;

        timeSpedizioni = corriere.timeCorriere;
    }

    // Processa le righe rimanenti
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        const size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        if (tempo % timeSpedizioni == 0 && tempo != 0) {
            // processa il corriere
            caricaOrdiniSuCorriere();
        }

        //printf("Verifico la scadenza di tutti gli ingredienti)
        verificaScadenza(magazzino.ingredienti,tempo);
        verificaQuantita(magazzino.ingredienti);

        // Processa input
        processInput(buffer);

        tempo++;
    }

    verificaScadenza(magazzino.ingredienti,tempo);
    verificaQuantita(magazzino.ingredienti);

    if (tempo % timeSpedizioni == 0 && tempo != 0) {
        // processa il corriere
        caricaOrdiniSuCorriere();
    }

    // Chiusura del file di output
    fclose(file);

    // Stampa in ordine del ricettario

    //printCatalogo(&catalogo);
   // stampaMagazzino(&magazzino);


    //Stampa tutte le ordinazioni
    //printOrdinazioni(&ordinazioni);

    fclose(file);

    // Ottieni il tempo di fine
    clock_t end = clock();

    // Calcola il tempo trascorso in secondi
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Mostra il tempo di esecuzione
    printf("Tempo di esecuzione: %f secondi\n", time_spent);

    return 0;
}

