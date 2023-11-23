#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const char *cardFormat = "+---------+\n\
| %-2s      |\n\
|         |\n\
|    %s    |\n\
|         |\n\
|      %2s |\n\
+---------+\n";

char *suits[2][4] = {
    {"♠", "♥", "♣", "♦"},
    {"♤", "♡", "♧", "♢"}
};

typedef struct Card{
    char symbol[3];
    int value;
    int suit;
    int color;
} card;

void initializeDeck(card *deckPtr, int deckSize);
void shuffleDeck(card *deckPtr, int deckSize);
void printDeck(card *deckPtr, int deckSize);
void printCard(card card);
card drawCard(card **deckPtr, int *deckSize);

int main(void){
    int deckSize = 52;
    card *deckPtr;
    card draw;

    srand(time(NULL));
    deckPtr = malloc(sizeof(card) * deckSize);
    
    if (deckPtr == NULL){
        printf("malloc() failed!\n");
    }
    else{
        initializeDeck(deckPtr, deckSize);
        shuffleDeck(deckPtr, deckSize);        
        for (int i = 0; i < 52; i++){
            printf("Deck size: %d\n", deckSize);
            draw = drawCard(&deckPtr, &deckSize);
            printCard(draw);
            puts("");
        }
    }

    free(deckPtr);

    return 0;
}

void initializeDeck(card *deckPtr, int deckSize){
    int value;

    for (int i = 0; i < deckSize; i++){
        value = i % 13 + 1;
        switch (value){
            case 1:
                strcpy(deckPtr[i].symbol, "A");
                deckPtr[i].value = 11;
                break;
            case 11:
                strcpy(deckPtr[i].symbol, "J");
                deckPtr[i].value = 10;
                break;
            case 12:
                strcpy(deckPtr[i].symbol, "Q");
                deckPtr[i].value = 10;
                break;
            case 13:
                strcpy(deckPtr[i].symbol, "K");
                deckPtr[i].value = 10;
                break;
            default:
                sprintf(deckPtr[i].symbol, "%d", value);
                deckPtr[i].value = value;
        }
        deckPtr[i].suit = i % 4;
        deckPtr[i].color = i % 2;
    }
}

void shuffleDeck(card *deckPtr, int deckSize){
    int j;
    card temp;

    for (int i = 0; i < deckSize - 2; i++){
        j = i + rand() % (deckSize - i);
        temp = deckPtr[i];
        deckPtr[i] = deckPtr[j];
        deckPtr[j] = temp;
    }
}

void printDeck(card *deckPtr, int deckSize){
    for (int i = 0; i < deckSize; i++){
        printCard(deckPtr[i]);
        puts("");
    }
}

void printCard(card card){
    printf(cardFormat, card.symbol, suits[card.color][card.suit], card.symbol);
}

card drawCard(card **deckPtr, int *deckSize){
    card topCard;
    
    if (*deckSize > 0){
        topCard = (*deckPtr)[--(*deckSize)];
    }

    if (*deckSize > 0){
        *deckPtr = realloc(*deckPtr, sizeof(card) * (*deckSize));
        if (*deckPtr == NULL){
            printf("realloc() failed!\n");
        }
    }
    else{
        *deckPtr = NULL;
    }
    
    return topCard;
}