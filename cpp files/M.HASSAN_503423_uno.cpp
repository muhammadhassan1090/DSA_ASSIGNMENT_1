#include "uno.h"
#include <map>
#include <sstream>
#include <iostream>
#include <random>
#include <algorithm>
using namespace std;

class CardNode {
public:
    string color;
    string value;
    CardNode* next;
    
    CardNode(string c, string v) : color(c), value(v), next(nullptr) {}
};

struct Player {
    CardNode* hand;
    int cardCount;
    
    Player() : hand(nullptr), cardCount(0) {}
};

struct GameData {
    CardNode* deck;
    CardNode* discardPile;
    Player* players;
    int numPlayers;
    int currentPlayer;
    bool clockwise;
    int winner;
    
    GameData(int n) : deck(nullptr), discardPile(nullptr), numPlayers(n), 
                      currentPlayer(0), clockwise(true), winner(-1) {
        players = new Player[n];
    }
    
    ~GameData() {
        while (deck) {
            CardNode* temp = deck;
            deck = deck->next;
            delete temp;
        }
        
        while (discardPile) {
            CardNode* temp = discardPile;
            discardPile = discardPile->next;
            delete temp;
        }
        
        for (int i = 0; i < numPlayers; i++) {
            while (players[i].hand) {
                CardNode* temp = players[i].hand;
                players[i].hand = players[i].hand->next;
                delete temp;
            }
        }
        
        delete[] players;
    }
};

static map<const UNOGame*, GameData*> gameData;


GameData* getGameData(const UNOGame* game) {
    return gameData[game];
}

void addCardToHand(Player& player, string color, string value) {
    CardNode* newCard = new CardNode(color, value);
    newCard->next = player.hand;
    player.hand = newCard;
    player.cardCount++;
}

void removeCardFromHand(Player& player, CardNode* prev, CardNode* card) {
    if (prev == nullptr) {
        player.hand = card->next;
    } else {
        prev->next = card->next;
    }
    player.cardCount--;
    delete card;
}

void addCardToDeck(CardNode*& deck, string color, string value) {
    CardNode* newCard = new CardNode(color, value);
    newCard->next = deck;
    deck = newCard;
}

CardNode* drawCardFromDeck(CardNode*& deck) {
    if (!deck) return nullptr;
    
    CardNode* card = deck;
    deck = deck->next;
    card->next = nullptr;
    return card;
}

UNOGame::UNOGame(int numPlayers) {
    gameData[this] = new GameData(numPlayers);
}

void UNOGame::initialize() {
    GameData* data = getGameData(this);
    
    string colors[] = {"Red", "Green", "Blue", "Yellow"};

    for (int i = 0; i < 4; i++) {
        addCardToDeck(data->deck, colors[i], "0");
        for (int num = 1; num <= 9; num++) {
            addCardToDeck(data->deck, colors[i], to_string(num));
            addCardToDeck(data->deck, colors[i], to_string(num));
        }
    }

    string actions[] = {"Skip", "Reverse", "Draw Two"};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            addCardToDeck(data->deck, colors[i], actions[j]);
            addCardToDeck(data->deck, colors[i], actions[j]);
        }
    }

    vector<CardNode*> cards;
    CardNode* current = data->deck;
    while (current) {
        cards.push_back(current);
        current = current->next;
    }
    
    mt19937 rng(1234);
    shuffle(cards.begin(), cards.end(), rng);
    
    data->deck = nullptr;
    for (int i = cards.size() - 1; i >= 0; i--) {
        cards[i]->next = data->deck;
        data->deck = cards[i];
    }

    for (int i = 0; i < 7; i++) {
        for (int p = 0; p < data->numPlayers; p++) {
            CardNode* card = drawCardFromDeck(data->deck);
            if (card) {
                addCardToHand(data->players[p], card->color, card->value);
                delete card;
            }
        }
    }

    CardNode* topCard = drawCardFromDeck(data->deck);
    if (topCard) {
        data->discardPile = topCard;
    }
}

void UNOGame::playTurn() {
    GameData* data = getGameData(this);
    
    if (data->winner != -1) return;
    
    Player& player = data->players[data->currentPlayer];
    
    CardNode* current = player.hand;
    CardNode* prev = nullptr;
    CardNode* matchedCard = nullptr;
    CardNode* matchedPrev = nullptr;
    
    string topColor = data->discardPile->color;
    string topValue = data->discardPile->value;

    current = player.hand;
    prev = nullptr;
    while (current) {
        if (current->color == topColor && 
            current->value != "Skip" && 
            current->value != "Reverse" && 
            current->value != "Draw Two") {
            matchedCard = current;
            matchedPrev = prev;
            break;
        }
        prev = current;
        current = current->next;
    }
    
    if (!matchedCard) {
        current = player.hand;
        prev = nullptr;
        while (current) {
            if (current->value == topValue) {
                matchedCard = current;
                matchedPrev = prev;
                break;
            }
            prev = current;
            current = current->next;
        }
    }
    
    if (!matchedCard) {
        current = player.hand;
        prev = nullptr;
        while (current) {
            if (current->color == topColor) {
                matchedCard = current;
                matchedPrev = prev;
                break;
            }
            prev = current;
            current = current->next;
        }
    }
    
    if (matchedCard) {

        string playedColor = matchedCard->color;
        string playedValue = matchedCard->value;
        

        if (matchedPrev == nullptr) {
            player.hand = matchedCard->next;
        } else {
            matchedPrev->next = matchedCard->next;
        }
        player.cardCount--;
        

        matchedCard->next = data->discardPile;
        data->discardPile = matchedCard;

        if (player.cardCount == 0) {
            data->winner = data->currentPlayer;
            return;
        }

        if (player.cardCount == 1) {
        }
      
        if (playedValue == "Skip") {
            if (data->clockwise) {
                data->currentPlayer = (data->currentPlayer + 2) % data->numPlayers;
            } else {
                data->currentPlayer = (data->currentPlayer - 2 + data->numPlayers) % data->numPlayers;
            }
            return;
        } else if (playedValue == "Reverse") {
            data->clockwise = !data->clockwise;
        } else if (playedValue == "Draw Two") {
            int nextPlayer = data->clockwise ? 
                (data->currentPlayer + 1) % data->numPlayers :
                (data->currentPlayer - 1 + data->numPlayers) % data->numPlayers;
            
            for (int i = 0; i < 2; i++) {
                CardNode* card = drawCardFromDeck(data->deck);
                if (card) {
                    addCardToHand(data->players[nextPlayer], card->color, card->value);
                    delete card;
                }
            }
            
            if (data->clockwise) {
                data->currentPlayer = (data->currentPlayer + 2) % data->numPlayers;
            } else {
                data->currentPlayer = (data->currentPlayer - 2 + data->numPlayers) % data->numPlayers;
            }
            return;
        }
    } else {
        CardNode* drawnCard = drawCardFromDeck(data->deck);
        if (drawnCard) {
            if (drawnCard->color == topColor || drawnCard->value == topValue) {

                drawnCard->next = data->discardPile;
                data->discardPile = drawnCard;
                
                if (drawnCard->value == "Skip") {
                    if (data->clockwise) {
                        data->currentPlayer = (data->currentPlayer + 2) % data->numPlayers;
                    } else {
                        data->currentPlayer = (data->currentPlayer - 2 + data->numPlayers) % data->numPlayers;
                    }
                    return;
                } else if (drawnCard->value == "Reverse") {
                    data->clockwise = !data->clockwise;
                } else if (drawnCard->value == "Draw Two") {
                    int nextPlayer = data->clockwise ? 
                        (data->currentPlayer + 1) % data->numPlayers :
                        (data->currentPlayer - 1 + data->numPlayers) % data->numPlayers;
                    
                    for (int i = 0; i < 2; i++) {
                        CardNode* card = drawCardFromDeck(data->deck);
                        if (card) {
                            addCardToHand(data->players[nextPlayer], card->color, card->value);
                            delete card;
                        }
                    }
                    
                    if (data->clockwise) {
                        data->currentPlayer = (data->currentPlayer + 2) % data->numPlayers;
                    } else {
                        data->currentPlayer = (data->currentPlayer - 2 + data->numPlayers) % data->numPlayers;
                    }
                    return;
                }
            } else {
                addCardToHand(player, drawnCard->color, drawnCard->value);
                delete drawnCard;
            }
        }
    }
    
    if (data->clockwise) {
        data->currentPlayer = (data->currentPlayer + 1) % data->numPlayers;
    } else {
        data->currentPlayer = (data->currentPlayer - 1 + data->numPlayers) % data->numPlayers;
    }
}

string UNOGame::getState() const {
    GameData* data = getGameData(this);
    ostringstream oss;
    
    oss << "Player " << data->currentPlayer << "'s turn, ";
    oss << "Direction: " << (data->clockwise ? "Clockwise" : "Counter-clockwise") << ", ";
    oss << "Top: " << data->discardPile->color << " " << data->discardPile->value << ", ";
    oss << "Players cards: ";
    
    for (int i = 0; i < data->numPlayers; i++) {
        oss << "P" << i << ":" << data->players[i].cardCount;
        if (i < data->numPlayers - 1) {
            oss << ", ";
        }
    }
    
    return oss.str();
}

bool UNOGame::isGameOver() const {
    GameData* data = getGameData(this);
    return data->winner != -1;
}

int UNOGame::getWinner() const {
    GameData* data = getGameData(this);
    return data->winner;
}
