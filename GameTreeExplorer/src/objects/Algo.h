#ifndef ALGO_H
#define ALGO_H

#include <queue>
#include <stack>
#include <utility>
class GameState;
class Player;
class GameBoard;

namespace Algorithm {
	// Checks if the given player is currently in a winning state
bool isWinningState(Player &player);

// Returns a reference to the opponent player given the current game state and player
Player &getOpponent(GameState &state, Player &player);

// Calculates a possible move for the player from position (x, y) within the given game state
// Returns a pair of integers representing the coordinates of the possible move
std::pair<int, int> calculatePossibleMove(int x, int y, Player &player,
										  GameState &state);

// Struct representing a single move step in the game
struct MoveStep {
	std::pair<int, int> from;      // Starting coordinates of the move
	std::pair<int, int> to;        // Ending coordinates of the move
	int playerNumber;              // Identifier for the player making the move
};

// Attempts to play the next move in the game for the given player
// Uses a history stack to keep track of moves and a visual queue for move visualization
// maxDepth limits the depth of move exploration, moveNum tracks the current move number
// Returns true if a move was successfully played, false otherwise
bool playNextMove(GameState &gameState, Player &player, std::stack<MoveStep> &history, std::queue<MoveStep> &visual, int maxDepth, int moveNum = 0);

// Determines the next best move for the player given the current game state
// Returns the coordinates of the best move as a pair of integers
std::pair<int, int> getNextBestMove(GameState &gameState, Player &player);


} // namespace algo

#endif // ALGO_H
