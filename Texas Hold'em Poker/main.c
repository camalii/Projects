#include <stdio.h>
#include <stdlib.h>

// definition of structures
typedef struct card
{
    char value;
    char color;
} card;

typedef struct countedCards
{
    int cardsValue;
    int quantity;
} countedCards;

int input(int *notEndOfFile, int position, card *listOfCards);
void numbering(card *listOfCards);
int areCardsUnique(card *cards);
int compare(const void *s1, const void *s2);
void countTheSameValues(card *player, countedCards *countedListOfCards);
int compareCountedCards(const void *s1, const void *s2);
int cardsPosition(card card, countedCards *listOfCards);
int whoWins(int score1, int score2, card *fiveCardsResult1, card *fiveCardsResult2);
int isItADraw(int score, card *fiveCardsResult1, card *fiveCardsResult2);

// winning combinations
int straightFlush(card *playersCards, card *fiveCardsResult);
int fourOfAKind(countedCards *countedPlayersCards, card *fiveCardsResult);
int fullHouse(countedCards *countedPlayersCards, card *fiveCardsResult);
int flush(char *color, card *playersCards, card *fiveCardsResult);
int straight(countedCards *countedPlayersCards, card *fiveCardsResult);
int threeOfAKind(card *playersCards, countedCards *countedPlayersCards, card *fiveCardsResult);
int twoPair(card *playersCards, countedCards *countedPlayersCards, card *fiveCardsResult);
int pair(card *playersCards, countedCards *countedPlayersCards, card *fiveCardsResult);
int highCard(card *playersCards, card *fiveCardsResult);

int main(void)
{
    int notEndOfFile = 1;

    do {
        int score1 = 0, score2 = 0;
        char color = 'n';
        card allTheCards[9] = { { .value = 0, .color = 0 } };
        // cards of each player + cards on the table
        card player1[8] = { { 0 } }, player2[8] = { { 0 } };
        // strongest combinations of each player
        card fiveCardsResult1[5] = { { 0 } }, fiveCardsResult2[5] = { { 0 } };
        // values of cards and quantity of each value (of each player)
        countedCards countedCardsOfPlayer1[8] = { { 0 } }, countedCardsOfPlayer2[8] = { { 0 } };

        // input, creation of all the cards
        if (input(&notEndOfFile, 0, allTheCards) != 0)
            exit(0); // player1
        if (input(&notEndOfFile, 2, allTheCards) != 0)
            exit(0); // player 2
        if (input(&notEndOfFile, 4, allTheCards) != 0)
            exit(0); // table

        numbering(allTheCards); // numbering of each card (from 1 (2) to 13 (A))

        if (areCardsUnique(allTheCards) != 0)
            exit(0);

        // assigning cards to players
        player1[0] = allTheCards[0];
        player1[1] = allTheCards[1];
        player2[0] = allTheCards[2];
        player2[1] = allTheCards[3];

        for (int i = 0; i < 5; i++) {
            player1[2 + i] = allTheCards[4 + i];
            player2[2 + i] = allTheCards[4 + i];
        }

        // sorting cards by values
        qsort(player1, 7, sizeof(card), compare);
        qsort(player2, 7, sizeof(card), compare);

        // counting the number of cards of the same values
        countTheSameValues(player1, countedCardsOfPlayer1);
        countTheSameValues(player2, countedCardsOfPlayer2);

        // ACTUAL GAME / winning combinations
        if (highCard(player1, fiveCardsResult1))
            score1 = 1;
        if (highCard(player2, fiveCardsResult2))
            score2 = 1;
        if (pair(player1, countedCardsOfPlayer1, fiveCardsResult1))
            score1 = 2;
        if (pair(player2, countedCardsOfPlayer2, fiveCardsResult2))
            score2 = 2;
        if (twoPair(player1, countedCardsOfPlayer1, fiveCardsResult1))
            score1 = 3;
        if (twoPair(player2, countedCardsOfPlayer2, fiveCardsResult2))
            score2 = 3;
        if (threeOfAKind(player1, countedCardsOfPlayer1, fiveCardsResult1))
            score1 = 4;
        if (threeOfAKind(player2, countedCardsOfPlayer2, fiveCardsResult2))
            score2 = 4;
        if (straight(countedCardsOfPlayer1, fiveCardsResult1))
            score1 = 5;
        if (straight(countedCardsOfPlayer2, fiveCardsResult2))
            score2 = 5;
        if (flush(&color, player1, fiveCardsResult1))
            score1 = 6;
        if (flush(&color, player2, fiveCardsResult2))
            score2 = 6;
        if (fullHouse(countedCardsOfPlayer1, fiveCardsResult1))
            score1 = 7;
        if (fullHouse(countedCardsOfPlayer2, fiveCardsResult2))
            score2 = 7;
        if (fourOfAKind(countedCardsOfPlayer1, fiveCardsResult1))
            score1 = 8;
        if (fourOfAKind(countedCardsOfPlayer2, fiveCardsResult2))
            score2 = 8;
        if (straightFlush(player1, fiveCardsResult1))
            score1 = 9;
        if (straightFlush(player2, fiveCardsResult2))
            score2 = 9;

        whoWins(score1, score2, fiveCardsResult1, fiveCardsResult2); // writes the final result

    } while (notEndOfFile);
    return 0;
}

// input of one card or EOF
int input(int *notEndOfFile, int position, card *listOfCards)
{
    int scanf1 = 0, scanf2 = 0, scanf3 = 0;
    char whiteSpaces[9] = { 0 }, max;
    char value[10] = { 0 }, color[10] = { 0 };
    card oneCard;

    if (position == 0 || position == 2) {
        max = 2;
    } else {
        max = 5;
    }

    for (int i = 0; i < max; i++) {
        scanf1 = scanf(" %1[2-9,T,J,Q,K,A]", value);
        scanf2 = scanf("%1[h,d,s,c]", color);

        if (i < (max - 1)) {
            scanf3 = scanf("%1[ \n\v\f\r\t]", whiteSpaces);
        } else {
            scanf3 = scanf("%1[\n]", whiteSpaces);
        }

        if (scanf1 + scanf2 + scanf3 != 3 && scanf1 != -1) {
            fprintf(stderr, "Invalid card set delimiter or invalid card\n");
            return 1;
        }

        if ((position != 0 && scanf1 == -1) || (position == 0 && i == 1 && scanf1 == -1)) {
            fprintf(stderr, "Unexpected end of file\n");
            return 1;
        }

        if (position == 0 && scanf1 == -1) {
            *notEndOfFile = 0; // expected end of file
            return 1;
        }

        oneCard.value = value[0];
        oneCard.color = color[0];
        listOfCards[i + position] = oneCard;
    }
    return 0;
}

void numbering(struct card *listOfCards)
{
    for (int i = 0; i < 9; i++) {

        char num = listOfCards[i].value;

        if (num > 49 && num < 58)
            num -= 49;
        if (num == 84)
            num = 9; // T
        if (num == 74)
            num = 10; // J
        if (num == 81)
            num = 11; // Q
        if (num == 75)
            num = 12; // K
        if (num == 65)
            num = 13; // A

        listOfCards[i].value = num;
    }
}

int areCardsUnique(card *cards)
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8 - i; j++) {
            if (cards[i].value == cards[j + i + 1].value && cards[i].color == cards[j + i + 1].color) {
                fprintf(stderr, "Duplicate card detected\n");
                return 1;
            }
        }
    }
    return 0;
}

int compare(const void *s1, const void *s2)
{
    card *k1 = (card *) s1;
    card *k2 = (card *) s2;

    if (k1->value < k2->value)
        return 1;
    if (k1->value > k2->value)
        return -1;

    return 0;
}

void countTheSameValues(card *player, countedCards *countedListOfCards)
{
    int position = 0;

    for (int i = 0; i < 7; i++) {
        card card = player[i];
        position = cardsPosition(card, countedListOfCards);
        countedListOfCards[position].cardsValue = card.value;
        countedListOfCards[position].quantity++;
    }

    qsort(countedListOfCards, 7, sizeof(countedCards), compareCountedCards);
}

int compareCountedCards(const void *s1, const void *s2)
{
    countedCards *sk1 = (countedCards *) s1;
    countedCards *sk2 = (countedCards *) s2;

    if (sk1->cardsValue > sk2->cardsValue)
        return -1;
    if (sk1->cardsValue < sk2->cardsValue)
        return 1;

    return 0;
}


int cardsPosition(card card, countedCards *listOfCards)
{
    for (int i = 0; i < 7; i++) {
        if (listOfCards[i].quantity == 0)
            return i;
        if (listOfCards[i].cardsValue == card.value)
            return i;
    }

    return 0;
}

int straightFlush(card *playersCards, card *fiveCardsResult)
{
    int count = 0, isThereA = 0, position = 0;
    char color = 'n';
    card sameColor[8] = { { 0 } }; // array of cards of the same color

    flush(&color, playersCards, fiveCardsResult);

    for (int j = 0; j < 8; j++) {
        if (playersCards[j].color == color) {
            sameColor[position].value = playersCards[j].value;
            position++;
        }
    }

    if (sameColor[0].value == 13) {
        isThereA = 1;
    }

    for (int k = 0; k < 8; k++) {
        if (sameColor[k].value == 0 && isThereA == 0) {
            sameColor[k].value = -2;
        }
    }

    if (isThereA) {
        sameColor[7].value = 0;
    }

    for (int i = 0; i < 7; i++) {
        if (sameColor[i].value - 1 == sameColor[i + 1].value && count != 4) {
            count += 1;
        } else if (count != 4) {
            count = 0;
        }

        if (count == 4) {
            fiveCardsResult[0].value = sameColor[i - 3].value;
            return 1;
        }
    }
    return 0;
}

int fourOfAKind(countedCards *countedPlayersCards, card *fiveCardsResult)
{
    for (int i = 0; i < 4; i++) {
        if (countedPlayersCards[i].quantity == 4) {
            for (int j = 0; j < 4; j++) {
                fiveCardsResult[j].value = countedPlayersCards[i].cardsValue;
            }
            for (int k = 0; k < 4; k++) {
                if (countedPlayersCards[k].cardsValue != countedPlayersCards[i].cardsValue) {
                    fiveCardsResult[4].value = countedPlayersCards[k].cardsValue;
                    return 1;
                }
            }
        }
    }
    return 0;
}

int fullHouse(countedCards *countedPlayersCards, card *fiveCardsResult)
{
    for (int i = 0; i < 7; i++) {
        if (countedPlayersCards[i].quantity == 3) {
            for (int j = 0; j < 7; j++) {
                if (countedPlayersCards[j].quantity >= 2 && countedPlayersCards[j].cardsValue != countedPlayersCards[i].cardsValue) {
                    for (int k = 0; k < 3; k++) {
                        fiveCardsResult[k].value = countedPlayersCards[i].cardsValue;
                    }

                    for (int l = 3; l < 5; l++) {
                        fiveCardsResult[l].value = countedPlayersCards[j].cardsValue;
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}

int flush(char *color, card *playersCards, card *fiveCardsResult)
{
    int flushPosition = 0, h = 0, d = 0, s = 0, c = 0;

    for (int i = 0; i < 7; i++) {
        switch (playersCards[i].color) {
        case 'h':
            h++;
            if (h == 5) {
                *color = 'h';
            }
            break;
        case 'd':
            d++;
            if (d == 5) {
                *color = 'd';
            }
            break;
        case 's':
            s++;
            if (s == 5) {
                *color = 's';
            }
            break;
        case 'c':
            c++;
            if (c == 5) {
                *color = 'c';
            }
            break;
        }
    }

    if (*color != 'n') {
        for (int j = 0; j < 7; j++) {
            if (playersCards[j].color == *color) {
                fiveCardsResult[flushPosition].value = playersCards[j].value;
                flushPosition++;
            }

            if (flushPosition == 5) {
                return 1;
            }
        }
    }
    return 0;
}

int straight(countedCards *countedPlayersCards, card *fiveCardsResult)
{
    int count = 0, isThereA = 0;

    if (countedPlayersCards[0].cardsValue == 13) {
        isThereA = 1;
    }

    for (int k = 0; k < 8; k++) {
        if (countedPlayersCards[k].cardsValue == 0 && isThereA == 0) {
            countedPlayersCards[k].cardsValue = -2;
        }
    }

    if (isThereA) {
        countedPlayersCards[7].cardsValue = 0;
    }

    for (int i = 0; i < 7; i++) {
        if (countedPlayersCards[i].cardsValue - 1 == countedPlayersCards[i + 1].cardsValue && count != 4) {
            count += 1;
        } else if (count != 4) {
            count = 0;
        }

        if (count == 4) {
            fiveCardsResult[0].value = countedPlayersCards[i - 3].cardsValue;
            return 1;
        }
    }
    return 0;
}

int threeOfAKind(card *playersCards, countedCards *countedPlayersCards, card *fiveCardsResult)
{
    int additionalCardCount = 3;

    for (int i = 0; i < 6; i++) {
        if (countedPlayersCards[i].quantity == 3) {
            for (int k = 0; k < 3; k++) {
                fiveCardsResult[k].value = countedPlayersCards[i].cardsValue;
            }

            for (int j = 0; j < 7; j++) {
                if (playersCards[j].value != countedPlayersCards[i].cardsValue) {
                    fiveCardsResult[additionalCardCount].value = playersCards[j].value;
                    additionalCardCount++;
                    if (additionalCardCount == 5) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int twoPair(card *playersCards, countedCards *countedPlayersCards, card *fiveCardsResult)
{
    for (int i = 0; i < 5; i++) {
        if (countedPlayersCards[i].quantity == 2) {
            for (int j = i + 1; j < 5; j++) {
                if (countedPlayersCards[j].quantity == 2) {
                    fiveCardsResult[0].value = countedPlayersCards[i].cardsValue;
                    fiveCardsResult[1].value = countedPlayersCards[i].cardsValue;
                    fiveCardsResult[2].value = countedPlayersCards[j].cardsValue;
                    fiveCardsResult[3].value = countedPlayersCards[j].cardsValue;

                    for (int k = 0; k < 7; k++) {
                        if (playersCards[k].value != fiveCardsResult[0].value && playersCards[k].value != fiveCardsResult[2].value) {
                            fiveCardsResult[4].value = playersCards[k].value;
                            return 1;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

int pair(card *playersCards, countedCards *countedPlayersCards, card *fiveCardsResult)
{
    int additionalCardCount = 2;
    for (int i = 0; i < 7; i++) {
        if (countedPlayersCards[i].quantity == 2) {
            fiveCardsResult[0].value = countedPlayersCards[i].cardsValue;
            fiveCardsResult[1].value = countedPlayersCards[i].cardsValue;
            for (int j = 0; j < 7; j++) {
                if (playersCards[j].value != countedPlayersCards[i].cardsValue) {
                    fiveCardsResult[additionalCardCount].value = playersCards[j].value;
                    additionalCardCount++;
                    if (additionalCardCount == 5) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int highCard(card *playersCards, card *fiveCardsResult)
{
    for (int i = 0; i < 5; i++) {
        fiveCardsResult[i].value = playersCards[i].value;
    }
    return 1;
}

int whoWins(int score1, int score2, card *fiveCardsResult1, card *fiveCardsResult2)
{
    if (score1 > score2) {
        printf("W\n"); // player1 is the winner
        return 1;
    }

    if (score1 < score2) {
        printf("L\n"); // player2 is the winner
        return 1;
    }

    if (score1 == score2) {
        isItADraw(score1, fiveCardsResult1, fiveCardsResult2);
    }
    return 0;
}

int isItADraw(int score, card *fiveCardsResult1, card *fiveCardsResult2)
{
    if (score == 5) {
        score = 9; // compering just the first card
    }

    if (score == 1 || score == 2 || score == 3 || score == 4 || score == 6 || score == 7) {
        score = 8; // compering all of the cards
    }

    switch (score) {
    case 9:
        if (fiveCardsResult1[0].value > fiveCardsResult2[0].value) {
            printf("W\n");
        } else if (fiveCardsResult1[0].value < fiveCardsResult2[0].value) {
            printf("L\n");
        } else {
            printf("D\n");
        }
        break;
    case 8:
        for (int i = 0; i < 4; i++) {
            if (fiveCardsResult1[i].value > fiveCardsResult2[i].value) {
                printf("W\n");
                return 0;
            }

            if (fiveCardsResult1[i].value < fiveCardsResult2[i].value) {
                printf("L\n");
                return 0;
            }
        }

        if (fiveCardsResult1[4].value > fiveCardsResult2[4].value) {
            printf("W\n");
        } else if (fiveCardsResult1[4].value < fiveCardsResult2[4].value) {
            printf("L\n");
        } else {
            printf("D\n");
        }
        break;
    }
    return 0;
}
