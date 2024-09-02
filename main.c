#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    T->num_ricette++;
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
    T->num_ricette--;
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

Ordine* createOrdine(char *nome, int ammount, RecipeNode *associatedRecipe) {
    Ordine *newOrder = (Ordine *)malloc(sizeof(Ordine));

    // Alloca memoria e copia il nome della ricetta
    newOrder->nome = strdup(nome);

    newOrder->ammount = ammount;
    newOrder->arrivalTime = tempo;
    newOrder->associatedRecipe = associatedRecipe;
    newOrder->peso = calcolaPeso(associatedRecipe) * ammount;

    return newOrder;
}


OrderNode* createOrderNode(Ordine* ordineOriginale) {
    // Allocazione della memoria per il nuovo nodo dell'ordine
    OrderNode* orderNode = (OrderNode*)malloc(sizeof(OrderNode));

    // Assegnazione della chiave (tempo arrivo)
    orderNode->key = ordineOriginale->arrivalTime;

    // Creazione di una copia profonda dell'ordine
    Ordine* ordineCopia = (Ordine*)malloc(sizeof(Ordine));

    // Copia dei campi dell'ordine
    ordineCopia->nome = strdup(ordineOriginale->nome); // Duplica il nome della ricetta
    ordineCopia->ammount = ordineOriginale->ammount;
    ordineCopia->arrivalTime = ordineOriginale->arrivalTime;
    ordineCopia->associatedRecipe = ordineOriginale->associatedRecipe;
    ordineCopia->peso = ordineOriginale->peso;

    // Assegnazione dell'ordine copiato al nodo
    orderNode->ordinePronto = ordineCopia;

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

    while (x != T->nil) {        // Trova la posizione corretta per il nuovo nodo z
        y = x;

        // Confronta solo il tempo di arrivo (in ordine decrescente)
        if (z->ordinePronto->arrivalTime < x->ordinePronto->arrivalTime) {
            x = x->left;         // Se z ha un arrivo successivo, va a sinistra
        } else {
            x = x->right;        // Altrimenti, va a destra
        }
    }

    z->parent = y;                // Collega il genitore al nuovo nodo z
    if (y == T->nil) {
        T->root = z;              // Se l'albero è vuoto, z diventa la radice
    } else if (z->ordinePronto->arrivalTime < y->ordinePronto->arrivalTime) {
        y->left = z;              // Se z ha un arrivo successivo, diventa figlio sinistro
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
    T->num_ordinazioni_pronte--;
}


// Dichiarazione della funzione helper

// Definizione della funzione helper fuori dalla funzione principale
void aggiornaRicetteAlbero(OrderNode *nodo, Catalogo *catalogo, OrderNode *nil) {
    if (nodo != nil) {
        // Aggiorna la ricetta associata all'ordine corrente
        nodo->ordinePronto->associatedRecipe = cercaRicetta(catalogo->root, nodo->ordinePronto->nome);

        // Procedi con il sottoalbero sinistro
        aggiornaRicetteAlbero(nodo->left, catalogo, nil);

        // Procedi con il sottoalbero destro
        aggiornaRicetteAlbero(nodo->right, catalogo, nil);
    }
}

void aggiornaRicetteAssociate(Ordinazioni *ordinazioni, Catalogo *catalogo) {
    // Aggiorna le ricette degli ordini sospesi
    for (int i = 0; i < ordinazioni->num_ordinazioni_sospese; i++) {
        Ordine *ordine = &ordinazioni->sospesi[i];
        ordine->associatedRecipe = cercaRicetta(catalogo->root, ordine->nome);
    }

    // Aggiorna le ricette associate agli ordini pronti nell'albero
    aggiornaRicetteAlbero(ordinazioni->root, catalogo, ordinazioni->nil);
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

//TODO rimozione degli ingredienti vuoti

//============================PREPARA=ORDINI===========================

void clean_lots(ResourceNode *node, ResourceNode *nil, int current_time) {
    if (node == nil) {
        return;
    }

    int i = 0;
    while (i < node->num_lotti) {
        if (node->lotti[i].scadenza <= current_time || node->lotti[i].ammount == 0) {
            // Rimuove il lotto spostando gli elementi successivi indietro
            node->maxGrammi -= node->lotti[i].ammount;
            for (int j = i; j < node->num_lotti - 1; j++) {
                node->lotti[j] = node->lotti[j + 1];
            }
            node->num_lotti--;

            // Dealloca l'array dei lotti se non ci sono più lotti rimasti
            if (node->num_lotti == 0) {
                free(node->lotti);
                node->lotti = NULL;
                break;
            } else {
                // Ridimensiona l'array dei lotti
                node->lotti = realloc(node->lotti, node->num_lotti * sizeof(Lotto));
            }
        } else {
            // Lotto valido trovato, esce dal ciclo
            break;
        }
    }

    // Itera ricorsivamente sui nodi figli sinistro e destro
    clean_lots(node->left, nil, current_time);
    clean_lots(node->right, nil, current_time);
}



void verificaScadenza() {
    clean_lots(magazzino.root, magazzino.nil, tempo);
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
        if (ingredienteMagazzino->maxGrammi <= ingredienteNecessario.quantita * orderToProcess->ammount) {
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
            break;
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

// Funzione per rimuovere un ordine sospeso dalla lista
void rimuoviOrdineSospeso(Ordinazioni *ordinazioni, int index) {
    // Sposta tutti gli ordini successivi a sinistra di una posizione
    for (int j = index; j < ordinazioni->num_ordinazioni_sospese - 1; j++) {
        ordinazioni->sospesi[j] = ordinazioni->sospesi[j + 1];
    }
    // Riduci il numero di ordini sospesi
    ordinazioni->num_ordinazioni_sospese--;
}

void processaOrdiniSospeso(Ordinazioni *ordinazioni) {
    int i = 0;

    while (i < ordinazioni->num_ordinazioni_sospese) {
        Ordine *ordineCorrente = &ordinazioni->sospesi[i];

        if (calcolaFattibilita(ordineCorrente)) {

            printf("t: %d Ordine %s in preparazione qta: %d\n", ordineCorrente->arrivalTime ,ordineCorrente->nome,ordineCorrente->ammount);

            // L'ordine è fattibile, quindi procediamo con la preparazione
            preparaOrdine(ordineCorrente, ordineCorrente->associatedRecipe);

            // Inserisci l'ordine nell'albero degli ordini pronti
            InserisciOrdinePronto(ordinazioni, createOrderNode(ordineCorrente));

            // Rimuovi l'ordine dalla lista degli ordini sospesi
            rimuoviOrdineSospeso(ordinazioni, i);
        } else {
            // Se l'ordine non è fattibile, passa all'ordine successivo
            i++;
        }
    }
}

//=============================FUNZIONI=DI=STAMPA=TESTING==================================

// Funzione di testing per iterare nell'albero e stampare il contenuto
void stampaRicetta(RecipeNode *node, RecipeNode *nil) {
    if (node != nil) {
        // Visita ricorsiva del sottoalbero sinistro
        stampaRicetta(node->left, nil);

        // Stampa delle informazioni del nodo corrente
        printf("Nome della ricetta: %s\n", node->key);
        printf("Numero di ingredienti: %d\n", node->num_ingredienti);
        printf("Ingredienti:\n");
        for (int i = 0; i < node->num_ingredienti; i++) {
            printf("  - %s: %d\n", node->ingredienti[i].ingrediente, node->ingredienti[i].quantita);
        }
        printf("Ordini attivi: %d\n", node->activeOrders);
        printf("\n");

        // Visita ricorsiva del sottoalbero destro
        stampaRicetta(node->right, nil);
    }
}

void stampaCatalogo(Catalogo *catalogo) {
    // Avvia la visita in-order dell'albero a partire dalla radice
    printf("\n\n>CATALOGO< (%d recipes)\n",catalogo->num_ricette);
    stampaRicetta(catalogo->root, catalogo->nil);
}

// Funzione di testing per iterare nell'albero e stampare il contenuto
void stampaRisorsa(ResourceNode *node, ResourceNode *nil) {
    if (node != nil) {
        // Visita ricorsiva del sottoalbero sinistro
        stampaRisorsa(node->left, nil);

        // Stampa delle informazioni del nodo corrente
        printf("Ingrediente: %s MaxGrammi: %d\n", node->key,node->maxGrammi);
        printf("Numero di lotti: %d\n", node->num_lotti);
        printf("Lotti:\n");
        for (int i = 0; i < node->num_lotti; i++) {
            printf("  - Lotto %d: %d grammi, Scadenza: %d\n", i + 1, node->lotti[i].ammount, node->lotti[i].scadenza);
        }
        printf("\n");

        // Visita ricorsiva del sottoalbero destro
        stampaRisorsa(node->right, nil);
    }
}

void stampaMagazzino(Magazzino *magazzino) {
    // Avvia la visita in-order dell'albero a partire dalla radice
    printf("\n\n>MAGAZZINO< (%d risorse)\n",magazzino->num_risorse);
    stampaRisorsa(magazzino->root, magazzino->nil);
}

// Funzione per stampare gli ordini sospesi
void stampaOrdiniSospesi(Ordinazioni *ordinazioni) {
    printf("Ordini sospesi:\n");
    for (int i = 0; i < ordinazioni->num_ordinazioni_sospese; i++) {
        Ordine *ordine = &ordinazioni->sospesi[i];
        printf("Nome dell'ordine: %s\n", ordine->nome);
        printf("Quantita richiesta: %d\n", ordine->ammount);
        printf("Peso: %d\n", ordine->peso);
        printf("Tempo di arrivo: %d\n", ordine->arrivalTime);
        printf("Stato: %d\n", ordine->arrivalTime); // Supponendo che il tempo di arrivo rappresenti anche lo stato
        printf("Ricetta associata: %s\n", ordine->associatedRecipe->key);
        printf("\n");
    }
}

// Funzione ricorsiva per stampare gli ordini pronti (in-order traversal)
void stampaOrdiniPronti(OrderNode *node, OrderNode *nil) {
    if (node != nil) {
        // Visita ricorsiva del sottoalbero sinistro
        stampaOrdiniPronti(node->left, nil);

        // Stampa delle informazioni dell'ordine pronto
        printf("Nome dell'ordine: %s\n", node->ordinePronto->nome);
        printf("Quantita richiesta: %d\n", node->ordinePronto->ammount);
        printf("Peso: %d\n", node->ordinePronto->peso);
        printf("Tempo di arrivo: %d\n", node->ordinePronto->arrivalTime);
        printf("Ricetta associata: %s\n", node->ordinePronto->associatedRecipe->key);
        printf("\n");

        // Visita ricorsiva del sottoalbero destro
        stampaOrdiniPronti(node->right, nil);
    }
}

// Funzione per stampare tutte le ordinazioni
void stampaOrdinazioni(Ordinazioni *ordinazioni) {
    printf("\n\n>ORDINAZIONI< (%d sospesi, %d pronti)\n",ordinazioni->num_ordinazioni_sospese,ordinazioni->num_ordinazioni_pronte);
    stampaOrdiniSospesi(ordinazioni);
    printf("Ordini pronti:\n");
    stampaOrdiniPronti(ordinazioni->root, ordinazioni->nil);
}



//=========================================CORRIERE=====================================================================


void caricaOrdiniInOrder(Ordinazioni *T, OrderNode *nodo, OrderNode *nil, int *caricoCorrente, int caricoMassimo, OrderNode **toRemoveList, int *toRemoveCount, int *terminaEsecuzione) {
    // Se il nodo è nullo, abbiamo raggiunto una foglia e ritorniamo
    if (nodo == nil) {
        return;
    }

    // Se il flag di terminazione è impostato, interrompiamo l'esecuzione
    if (*terminaEsecuzione == 1) {
        return;
    }

    // Visita il sottoalbero sinistro prima
    caricaOrdiniInOrder(T, nodo->left, nil, caricoCorrente, caricoMassimo, toRemoveList, toRemoveCount, terminaEsecuzione);

    // Controllo dopo la ricorsione sul sottoalbero sinistro per verificare se il flag di terminazione è stato impostato
    if (*terminaEsecuzione == 1) {
        return;
    }

    // Controlla se aggiungendo il peso del nodo corrente si supera il carico massimo
    if (*caricoCorrente + nodo->ordinePronto->peso > caricoMassimo) {
        // Imposta il flag per terminare l'esecuzione
        *terminaEsecuzione = 1;
        return;
    }

    // Gestisci il nodo corrente (solo se non è stato superato il carico massimo)
    *caricoCorrente += nodo->ordinePronto->peso;
    toRemoveList[(*toRemoveCount)++] = nodo;

    // Visita il sottoalbero destro dopo aver gestito il nodo corrente
    caricaOrdiniInOrder(T, nodo->right, nil, caricoCorrente, caricoMassimo, toRemoveList, toRemoveCount, terminaEsecuzione);
}





int compareOrders(const void *a, const void *b) {
    // Cast dei puntatori ai tipi corretti
    const OrderNode *orderA = *(const OrderNode **)a;
    const OrderNode *orderB = *(const OrderNode **)b;

    // Confronta i pesi in ordine decrescente
    if (orderA->ordinePronto->peso != orderB->ordinePronto->peso) {
        return orderB->ordinePronto->peso - orderA->ordinePronto->peso;
    }

    // Confronta i tempi di arrivo in ordine crescente
    return orderA->ordinePronto->arrivalTime - orderB->ordinePronto->arrivalTime;
}


void rimuoviOrdini(Ordinazioni *T, OrderNode **toRemoveList, int toRemoveCount) {
    // Ordina toRemoveList secondo il criterio richiesto
    qsort(toRemoveList, toRemoveCount, sizeof(OrderNode *), compareOrders);

    // Stampa gli ordini ordinati prima di rimuoverli
    for (int i = 0; i < toRemoveCount; i++) {
        OrderNode *nodo = toRemoveList[i];
        printf("%d %s %d peso: %d\n", nodo->ordinePronto->arrivalTime, nodo->ordinePronto->nome, nodo->ordinePronto->ammount, nodo->ordinePronto->peso);
        nodo->ordinePronto->associatedRecipe->activeOrders--;
    }

    // Rimuovi i nodi ordinati dall'albero
    for (int i = 0; i < toRemoveCount; i++) {
        removeLoadedOrder(T, toRemoveList[i]);
    }
}


void gestisciOrdini(Ordinazioni *T, int caricoMassimo) {
    int caricoCorrente = 0;
    OrderNode *toRemoveList[2000];  // Array per accumulare nodi da rimuovere
    int toRemoveCount = 0;

    // Carica ordini in base al tempo di arrivo (crescente) e accumula i nodi da rimuovere
    int terminaEsecuzione = 0;
    caricaOrdiniInOrder(T, T->root, T->nil, &caricoCorrente, caricoMassimo, toRemoveList, &toRemoveCount, &terminaEsecuzione);

    // Ordina, stampa e rimuovi i nodi accumulati
    rimuoviOrdini(T, toRemoveList, toRemoveCount);
}



void caricaOrdiniSuCorriere() {
    if(ordinazioni.num_ordinazioni_pronte==0)
    {
        printf("camioncino vuoto\n");
        return;
    }

    gestisciOrdini(&ordinazioni, corriere.caricoMax);


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
        aggiornaRicetteAssociate(&ordinazioni,&catalogo);
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
        aggiornaRicetteAssociate(&ordinazioni,&catalogo);
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

        processaOrdiniSospeso(&ordinazioni);

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
                printf("preparato\n");
            }else
            {
                inserisciOrdineSospeso(nuovoOrdine);
                printf("sospeso\n");
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
     //FILE *file = stdin;

    // // Usa stdout come file di output
     //FILE *file2 = stdout;

    FILE *file = fopen("open4.txt", "r");
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
            printf("Carico Max: %d\n",corriere.caricoMax);
            caricaOrdiniSuCorriere();

        }

        verificaScadenza();

        // Processa input
        processInput(buffer);

        tempo++;
    }

    verificaScadenza();

    if (tempo % timeSpedizioni == 0 && tempo != 0) {
        // processa il corriere
      caricaOrdiniSuCorriere();
    }

    // Chiusura del file di output
    fclose(file);

    // Stampa in ordine del ricettario

    //stampaCatalogo(&catalogo);
    //stampaMagazzino(&magazzino);
    //stampaOrdinazioni(&ordinazioni);

    fclose(file);

    // Ottieni il tempo di fine
    clock_t end = clock();

    // Calcola il tempo trascorso in secondi
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    // Mostra il tempo di esecuzione
    printf("Tempo di esecuzione: %f secondi\n", time_spent);

    return 0;
}

