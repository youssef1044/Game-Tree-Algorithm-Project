#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Player.h"
#include "Algo.h"
#include "GameBoard.h"
#include <iostream>
#include <stdexcept>

// Class representing the overall state of the game, including players, board, and turn management
class GameState
{
private:
size_t MaxTokensPerPlayer; // Maximum tokens each player can have
GameBoard board;           // The game board instance
Player player1;            // Player 1 instance
Player player2;            // Player 2 instance
int currentPlayer;         // Index of the current player (0 or 1)


// Initializes tokens for both players and places them on the board
void initializeTokens(float cellW, float cellH)
{
    for (size_t i = 0; i < MaxTokensPerPlayer; ++i)
    {
        // Create tokens for player 0 and player 1 with initial positions and textures
        Token *token1 = new Token(0, i + 1, 0, "rtoken.png", cellW, cellH);
        Token *token2 = new Token(i + 1, 0, 1, "gtoken.png", cellW, cellH);

        // Add tokens to respective players
        player1.addToken(token1);
        player2.addToken(token2);

        // Place tokens on the board
        board.placeToken(token1);
        board.placeToken(token2);
    }
}
public:
// Constructor initializing game state with cell sizes and board size
GameState(float cellW, float cellH, size_t gameSize)
: MaxTokensPerPlayer(gameSize - 2),
board(gameSize, gameSize),
player1(0, MaxTokensPerPlayer),
player2(1, MaxTokensPerPlayer),
currentPlayer(0)
{
initializeTokens(cellW, cellH);
}


// Copy constructor performing deep copy of players and board
GameState(const GameState& other) 
	: MaxTokensPerPlayer(other.MaxTokensPerPlayer),
	  board(other.board),  // Assumes GameBoard has a proper copy constructor
	  player1(other.player1.getPlayerNumber(), other.MaxTokensPerPlayer),
	  player2(other.player2.getPlayerNumber(), other.MaxTokensPerPlayer),
	  currentPlayer(other.currentPlayer)
{
	// Deep copy tokens for both players
	for (size_t i = 0; i < other.player1.getTokens().size(); ++i) {
		Token* original = other.player1.getTokens()[i];
		Token* copy = new Token(*original);  // Assumes Token has a copy constructor
		player1.addToken(copy);
		board.placeToken(copy);  // Register with board
	}

	for (size_t i = 0; i < other.player2.getTokens().size(); ++i) {
		Token* original = other.player2.getTokens()[i];
		Token* copy = new Token(*original);  // Assumes Token has a copy constructor
		player2.addToken(copy);
		board.placeToken(copy);  // Register with board
	}
}

// Returns reference to the current player
Player &getCurrentPlayer() { return currentPlayer == 0 ? player1 : player2; }
// Returns reference to the other player
Player &getOtherPlayer() { return currentPlayer == 0 ? player2 : player1; }
// Returns reference to the game board
GameBoard &getBoard() { return board; }

// Switches the current player to the other player
void switchPlayer()
{
    currentPlayer = 1 - currentPlayer;
}

// Moves a token on the board and updates player states accordingly
void moveToken(int fromX, int fromY, int toX, int toY)
{
    board.moveToken(fromX, fromY, toX, toY);
    player1.updateMovableTokens();
    player2.updateMovableTokens();

    if (auto token = board.getTokenAt(toX, toY))
    {
        if (token->hasReachedEnd())
        {
			std::cout << "JOE MAMA" << std::endl;
            getCurrentPlayer().setScore(getCurrentPlayer().getScore() + 1);
        }
    }
}
};

#endif // GAMESTATE_H