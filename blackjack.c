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
    float bank;
    int score;
    bool dealer;
    int index;
    card *hand;
} player;

void play(void);
void initializeDeck(card *deckPtr, int deckSize);
void shuffleDeck(card *deckPtr, int deckSize);
void printDeck(card *deckPtr, int deckSize);
void printCard(card card);
void printHand(player player, bool withHoleCard);
card drawCard(card **deckPtr, int *deckSize);
bool deal(card **deckPtr, int *deckSize, player *player);

int main(void){
    bool playing = true;
    char action;

    srand(time(NULL));
    
    while (playing){
        play();
        printf("Play again ([y]es/[n]o)? ");
        scanf(" %c", &action);
        action = tolower(action);
        playing = action == 'y';
    }
    
    return 0;
}

void play(void){
    int deckSize = 52;
    card *deckPtr;
    player p = {10000, 0, false, 0};
    player dealer = {0, 0, true, 0};
    float bet = 0;
    char action;

    deckPtr = malloc(sizeof(card) * deckSize);

    if (deckPtr == NULL){
        printf("malloc() failed!\n");
    }
    else{
        initializeDeck(deckPtr, deckSize);
        shuffleDeck(deckPtr, deckSize);        
        
        while (deckSize > 0 && p.bank > 0){
            p.score = 0;
            p.index = 0;
            card playerHand[11];
            p.hand = playerHand;
            
            dealer.score = 0;
            dealer.index = 0;
            card dealerHand[11];
            dealer.hand = dealerHand;
            bet = 0;
            
            while (bet == 0 || bet > p.bank){
                printf("Place your bet (bank: $%'.2f): ", p.bank);
                scanf("%f", &bet);
            }

            for (int i = 0; i < 4; i++){
                player *players[2] = {&p, &dealer};
                if (!deal(&deckPtr, &deckSize, players[i % 2])){
                    printf("Deck is empty!\n");
                    return;
                }
            }

            if (dealer.score != 21){
                printHand(dealer, true);
            }
            else{
                printHand(dealer, false);
            }
            printHand(p, false);

            while (1){
				action = '\0';

                if (p.score == dealer.score && dealer.score >= 17){
                    printf("Push! (+$0.00)\n");
                    break;
                }
                else if (p.score == 21){
                    printf("Blackjack! (+$%'.2f)\n", bet * 1.5);
                    p.bank += bet * 1.5;
                    break;
                }
                else if (dealer.score == 21){
                    printf("Dealer wins! (-$%'.2f)\n", bet);
                    p.bank -= bet;
                    break;
                }
                else if (p.score > 21){
                    printf("Bust! (-$%'.2f)\n", bet);
                    p.bank -= bet;
                    break;
                }
                else if (dealer.score > 21){
                    printf("Player wins! (+$%'.2f)\n", bet);
                    p.bank += bet;
                    break;
                }
                else if (dealer.score > p.score && dealer.score >= 17 && action == 's'){
                    printf("Dealer wins! (-$%'.2f)\n", bet);
                    p.bank -= bet;
                    break;                   
                }
                else if (p.score > dealer.score && dealer.score >= 17){
                    printf("Player wins! (+$%'.2f)\n", bet);
                    p.bank += bet;
                    break;
                }

                while (action != 'h' && action != 's'){
                    printf("Would you like to [h]it or [s]tand? ");
                    scanf(" %c", &action);
                    action = tolower(action);
                }
                
                if (action == 'h'){
                    if (!deal(&deckPtr, &deckSize, &p)){
                        printf("Deck is empty!\n");
                        return;
                    }
                }

                if (dealer.score < 17){
                    if (!deal(&deckPtr, &deckSize, &dealer)){
                        printf("Deck is empty!\n");
                        return;
                    }
                }
                
                printHand(dealer, false);
                printHand(p, false);
            }
        }
        if (deckSize == 0){
            printf("Deck is empty!\n");
        }
        else if (p.bank == 0){
            printf("Out of money!\n");
        }
    }
    free(deckPtr);
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
    char cardFormatCopy[84];
    char *token;

    if (player.dealer){
        if (withHoleCard){
            printf("Dealer (?)\n");
        }
        else{
            printf("Dealer (%d)\n", player.score);
        }
    }
    else{
        printf("Player (%d)\n", player.score);
    }

    strcpy(cardFormatCopy, cardFormat);
    token = strtok(cardFormatCopy, "\n");

    while (token != NULL){
        for (int i = 0; i < player.index; i++){
            if (strstr(token, "%-2s") != NULL){
                if (withHoleCard && i == 0){
                    printf(token, "?");
                }
                else{
                    printf(token, (player.hand + i)->symbol);
                }
            }
            else if (strstr(token, "%s") != NULL){
                if (withHoleCard && i == 0){
                    printf(token, "?");
                }
                else{
                    printf(token, suits[(player.hand + i)->color][(player.hand + i)->suit]);
                }
            }
            else if (strstr(token, "%2s") != NULL){
                if (withHoleCard && i == 0){
                    printf(token, "?");
                }
                else{
                    printf(token, (player.hand + i)->symbol);
                }
            }
            else{
                printf("%s", token);
            }
            printf("\t");
        }
        puts("");
        token = strtok(NULL, "\n");
    }
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
    
    return topCard;
}

bool deal(card **deckPtr, int *deckSize, player *player){
    card dealCard;

    dealCard = drawCard(deckPtr, deckSize);
    
    if (*deckSize == 0){
        return false;
    }
    
    player->hand[player->index++] = dealCard;
    
    if (strcmp(dealCard.symbol, "A") == 0 && player->score >= 11){
        player->score++;
    }
    else{
        player->score += dealCard.value;
    }
    
    return true;
}
