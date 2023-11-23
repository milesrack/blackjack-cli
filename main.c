#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

typedef struct Player{
    int score;
    bool dealer;
    int index;
    card hand[11];
} player;

void initializeDeck(card *deckPtr, int deckSize);
void shuffleDeck(card *deckPtr, int deckSize);
void printDeck(card *deckPtr, int deckSize);
void printCard(card card);
void printHand(player player, bool withHoleCard);
card drawCard(card **deckPtr, int *deckSize);
void deal(card **deckPtr, int *deckSize, player *player, int amount);

int main(void){
    int deckSize = 52;
    card *deckPtr;
    player p = {0, false, 0};
    player dealer = {0, true, 0};
    char action;

    srand(time(NULL));
    deckPtr = malloc(sizeof(card) * deckSize);
    
    if (deckPtr == NULL){
        printf("malloc() failed!\n");
    }
    else{
        // create deck and shuffle
        initializeDeck(deckPtr, deckSize);
        shuffleDeck(deckPtr, deckSize);        
        // deal 2 cards each to dealer and player
        deal(&deckPtr, &deckSize, &dealer, 2);
        printHand(dealer, true);
        deal(&deckPtr, &deckSize, &p, 2);
        printHand(p, false);

        while (1){
            if (p.score == 21 && dealer.score == 21){
                printf("Push!\n");
                break;
            }
            else if (p.score == 21){
                printf("Blackjack!\n");
                break;
            }
            else if (p.score > 21){
                printf("Bust!\n");
                break;
            }
            else if (dealer.score > 21){
                printf("Player wins!\n");
                break;
            }

            action = '\0';

            while (action != 'h' && action != 's'){
                printf("Your current score is %d, [h]it or [s]tand? ", p.score);
                scanf(" %c", &action);
                action = tolower(action);
            }
            
            if (action == 'h'){
                deal(&deckPtr, &deckSize, &p, 1);
            }

            if (dealer.score < 17){
                deal(&deckPtr, &deckSize, &dealer, 1);
            }
            
            printHand(dealer, false);
            printHand(p, false);

            if (dealer.score > 21){
                printf("Player wins!\n");
                break;
            }
            if ((dealer.score == 21 || dealer.score > p.score) && action == 's'){
                printf("Dealer wins!\n");
                break;
            }
            else if (p.score > dealer.score && action == 's'){
                printf("Player wins!\n");
                break;
            }
            else if (p.score == dealer.score && action == 's'){
                printf("Push!\n");
                break;
            }
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
    }
}

void printCard(card card){
    printf(cardFormat, card.symbol, suits[card.color][card.suit], card.symbol);
}

void printHand(player player, bool withHoleCard){
    if (player.dealer){
        printf("Dealer:\n");
    }
    else{
        printf("Player:\n");
    }
    for (int i = 0; i < player.index; i++){
        if (withHoleCard && i == 0){
            printf(cardFormat, "?", "?", "?");
        }
        else{
            printCard(player.hand[i]);
        }
    }
    // puts("");
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

void deal(card **deckPtr, int *deckSize, player *player, int amount){
    card dealCard;

    if (amount == 0 || deckSize == 0){
        return;
    }

    dealCard = drawCard(deckPtr, deckSize);
    player->hand[player->index++] = dealCard;
    // printCard(dealCard);
    
    if (strcmp(dealCard.symbol, "A") == 0 && player->score >= 11){
        player->score++;
    }
    else{
        player->score += dealCard.value;
    }
    
    deal(deckPtr, deckSize, player, amount-1);
}