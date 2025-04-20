#include "Algo.h"
#include "GameSate.h"
#include "Player.h"
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <cstdio>
#include <iostream>
#include <queue>
#include <stack>
#include <utility>
// Enum representing possible outcomes of a game state evaluation
enum Outcome {
	WON,
	LOSS,
};
// Checks if the given player has reached a winning state
bool Algorithm::isWinningState(Player &player) {
	bool reachedEnd = true;
	int n = player.getTokenCount();
// For player 0, check if all tokens have reached position (n+1) on the first coordinate
	if (player.getPlayerNumber() == 0) {
		for (auto token : player.getTokens()) {
			if (token->getPosition().first != n + 1) {
				reachedEnd = false;
				break;
			}
		}
	} else {
		// For player 1, check if all tokens have reached position (n+1) on the second coordinate
		for (auto token : player.getTokens()) {
			if (token->getPosition().second != n + 1) {
				reachedEnd = false;
				break;
			}
		}
	}

	return reachedEnd;
}
// Returns the opponent player given the current game state and player
Player &Algorithm::getOpponent(GameState &state, Player &player) {
	if (state.getOtherPlayer().getPlayerNumber() == player.getPlayerNumber())
		return state.getCurrentPlayer();
	else
		return state.getOtherPlayer();
}
// Calculates a possible move for the player from position (x, y) within the game state
std::pair<int, int> Algorithm::calculatePossibleMove(int x, int y, Player &player, GameState &state) {
	// Determine direction vector based on player number
	const sf::Vector2i direction(
			player.getPlayerNumber() == 0 ? 1 : 0,
			player.getPlayerNumber() == 1 ? 1 : 0
			);
// Get the move from current position to the next position in the direction
	auto pairMove = state.getBoard().getTokenMove(
			x, y,
			x + direction.x,
			y + direction.y);
	return pairMove;
}
// Recursive function to evaluate moves and outcomes for the player
Outcome recusionMove(GameState &state, Player &player, std::stack<Algorithm::MoveStep> &history, std::queue<Algorithm::MoveStep> &visual, int depth, int moveNum, bool &hasWon) {
	// If a winning path has already been found, return outcome accordingly
	if (hasWon)
		return player.getPlayerNumber() == 1 ? WON : LOSS;

// Check if current player is in winning state
if (Algorithm::isWinningState(player)) {
	if (player.getPlayerNumber() == 1)
		hasWon = true;
	return WON;
}
// Check if opponent is in winning state, meaning current player lost
if (Algorithm::isWinningState(Algorithm::getOpponent(state, player))) {
	return LOSS;
}

int childMoveNum = 0;
// Iterate over all tokens of the player
for (auto token : player.getTokens()) {
	// Skip tokens that cannot move
	if (!token->isMovable())
		continue;

	// Calculate possible move for the token
	std::pair<int, int> newMove =
		Algorithm::calculatePossibleMove(token->getPosition().first,
				token->getPosition().second, player, state);

	// If no valid move, continue to next token
	if (newMove.first == -1) {
		continue;
	}

	// Store old position of the token
	std::pair<int, int> oldMove =
		make_pair(token->getPosition().first, token->getPosition().second);

	// Add move to visual queue for visualization
	visual.push(Algorithm::MoveStep {oldMove , newMove, player.getPlayerNumber()});

	// Apply the move on the board
	state.getBoard().moveTokenRaw(oldMove.first, oldMove.second, newMove.first, newMove.second);

	// Add move to history stack for backtracking
	history.push(Algorithm::MoveStep {oldMove, newMove, player.getPlayerNumber()});

	// Recursively evaluate opponent's moves after this move
	Outcome result = recusionMove(state, Algorithm::getOpponent(state, player), history, visual, depth + 1, childMoveNum++, hasWon);

	// Undo the move to restore state
	state.getBoard().moveTokenRaw(newMove.first, newMove.second, oldMove.first, oldMove.second);

	// Add revert move to visual queue for visualization
	visual.push(Algorithm::MoveStep {newMove , oldMove, player.getPlayerNumber()});

	// If opponent lost or winning path found, return WON for current player
	if (result == LOSS || hasWon) {
		if (player.getPlayerNumber() == 1)
			hasWon = true;
		return WON;
	}

	// Remove move from history as it did not lead to a win
	history.pop();
}

// No winning path found, return LOSS
return LOSS;

}
// Attempts to play the next move for the player in the game state
bool Algorithm::playNextMove(GameState &state, Player &player, std::stack<MoveStep> &history, std::queue<MoveStep> &visual, int maxDepth = 0, int moveNum) {
	// Create a copy of the game state to explore moves
	GameState newState = state;
	bool hasFoundWin = false;
	// Start recursive move evaluation
	recusionMove(newState, player, history, visual, 0, 1, hasFoundWin);
	// Currently always returns false (not implemented fully)
	return false;
}

