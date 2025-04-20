
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <SFML/Graphics.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <iostream>
#include <memory>
#include <ostream>
#include <queue>
#include <stack>
#include "Algo.h"
#include "GameSate.h"
#include "GameBoard.h"

// Class managing the overall game logic, rendering, and user interaction
class GameManager
{
private:
// Struct holding game configuration settings
struct GameSettings
{
size_t size;          // Board size (width and height)
size_t maxTokens;     // Maximum tokens per player
float cellSize;       // Size of each cell in pixels
sf::VideoMode videoMode; // Window video mode
};


GameSettings settings;       // Game settings instance
sf::RenderWindow window;     // SFML window for rendering
GameState state;             // Current game state
bool tokenSelected;          // Flag indicating if a token is selected
sf::Vector2i selectedPosition; // Position of the selected token
sf::Vector2i possibleMove;   // Position of the possible move for selected token

bool Won = false;        // Flag indicating if the game has been won
sf::Text winText;            // Text to display when a player wins
sf::RectangleShape winwindow; // Overlay to darken screen on win
sf::Font font;               // Font used for win text

std::string player1Name;     // Name of player 1
std::string player2Name;     // Name of player 2
std::stack<Algorithm::MoveStep> history; // Stack to keep move history for undo/visualization

// Handles selection of a token at the given grid position
void TokenSelection(const sf::Vector2i &gridPos)
{
    try
    {
        if (auto *token = state.getBoard().getTokenAt(gridPos.x, gridPos.y))
        {
            // Only allow selecting tokens belonging to current player
            if (token->getPlayer() == state.getCurrentPlayer().getPlayerNumber())
            {
                tokenSelected = true;
                selectedPosition = gridPos;
                findPossibleMove(gridPos);
                return;
            }
        }
        resetSelection();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Selection error: " << ex.what() << std::endl;
        resetSelection();
    }
}

// Calculates the possible move for the selected token
void findPossibleMove(const sf::Vector2i &gridPos)
{
    const int player = state.getCurrentPlayer().getPlayerNumber();
    const sf::Vector2i direction(player == 0 ? 1 : 0, player == 1 ? 1 : 0);
    auto pairMove = state.getBoard().getTokenMove(
        gridPos.x, gridPos.y,
        gridPos.x + direction.x,
        gridPos.y + direction.y);
    possibleMove = sf::Vector2i(pairMove.first, pairMove.second);
}

// Handles moving the selected token to the given grid position
void TokenMove(const sf::Vector2i &gridPos)
{
    try
    {
        state.moveToken(
            selectedPosition.x, selectedPosition.y,
            gridPos.x, gridPos.y);

        checkWinCondition();
        checkOtherPlayerMoves();

        resetSelection();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Move error: " << ex.what() << std::endl;
        resetSelection();
    }
}

// Checks if the current player has won the game
void checkWinCondition()
{
    if (state.getCurrentPlayer().getScore() >= settings.maxTokens)
    {
        Won = true;
        setupWinScreen();
    }
}

// Sets up the win screen overlay and text
void setupWinScreen()
{
    // Load font if not already loaded
    if (!font.openFromFile("arial.ttf"))
    {
        std::cerr << "Error loading font for win screen!\n";
        // Handle error
    }

    // Create dark overlay covering the window
    winwindow.setSize(sf::Vector2f(window.getSize()));
    winwindow.setFillColor(sf::Color(0, 0, 0, 200));

    // Setup win text properties
    winText.setFont(font);
    winText.setCharacterSize(60);
    winText.setFillColor(sf::Color::Yellow);
    winText.setStyle(sf::Text::Bold);

    // Use player names instead of numbers for display
    std::string winnerName = state.getCurrentPlayer().getPlayerNumber() == 0
                                 ? player1Name
                                 : player2Name;
    winText.setString(winnerName + " wins!");

    // Center the win text in the window
    sf::FloatRect textRect = winText.getLocalBounds();
    winText.setOrigin(sf::Vector2f(textRect.size.x / 2.0f,
                                   textRect.size.y / 2.0f));
    winText.setPosition(sf::Vector2f(
        window.getSize().x / 2.0f, window.getSize().y / 2.0f));
}

// Checks if the other player has any valid moves, switches turn if so
void checkOtherPlayerMoves()
{
    if (state.getOtherPlayer().getMovableTokens() == 0)
    {
        std::cout << "Other player has no valid moves!\n";
        return;
    }
    state.switchPlayer();
}

// Handles SFML window events such as closing and mouse clicks
void handleEvents()
{
	// If current player is bot, handle bot turn
	if (state.getCurrentPlayer().getPlayerNumber() == 1)  {
		handleAlgoTurn();
		return;
	}
	
    while (auto event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
        {
            const auto mousePos = sf::Mouse::getPosition(window);
            const sf::Vector2i gridPos(
                static_cast<int>(mousePos.x / settings.cellSize),
                static_cast<int>(mousePos.y / settings.cellSize));

            if (tokenSelected && gridPos == possibleMove)
            {
                TokenMove(gridPos);
            }
            else
            {
                TokenSelection(gridPos);
            }
        }
    }
}

// Handles the bot player's turn by calculating and visualizing moves
void handleAlgoTurn() {
	std::queue<Algorithm::MoveStep> visualizeMoves;
	Algorithm::playNextMove(state, state.getCurrentPlayer(), history, visualizeMoves, 0);	

	Algorithm::MoveStep nextStep;

	const int base_delay_ms = 500;
	const int base_grid = 3;
	const float delay = (base_delay_ms * base_grid) / static_cast<float>(settings.size);

	GameBoard fakeBoard(state.getBoard());
	while (!visualizeMoves.empty()) {
		auto i = visualizeMoves.front();
		visualizeMoves.pop();
		fakeBoard.moveTokenRaw(i.from.first, i.from.second, i.to.first, i.to.second);

        window.clear(sf::Color::White);
		state.getBoard().draw(window, settings.cellSize, settings.cellSize, false);
		fakeBoard.draw(window, settings.cellSize, settings.cellSize, true);
		window.display();
		sf::sleep(sf::microseconds(static_cast<int>(delay)));
	}

	while (!history.empty()) {
		auto i = history.top();
		history.pop();
		nextStep = i;
	}

	state.getBoard().draw(window, settings.cellSize, settings.cellSize, false);

	state.moveToken(nextStep.from.first, nextStep.from.second, nextStep.to.first, nextStep.to.second);

	checkWinCondition();
	checkOtherPlayerMoves();
	return;
}



// Renders visual indicators for selected token and possible move
void renderSelection()
{
    if (!tokenSelected)
        return;

    // Draw rectangle around selected token
    sf::RectangleShape selection({settings.cellSize, settings.cellSize});
    selection.setPosition(sf::Vector2f(
        selectedPosition.x * settings.cellSize,
        selectedPosition.y * settings.cellSize));
    selection.setFillColor(sf::Color::Transparent);
    selection.setOutlineColor(sf::Color::Yellow);
    selection.setOutlineThickness(3);
    window.draw(selection);

    // Draw circle indicator for possible move
    if (possibleMove.x >= 0 && possibleMove.y >= 0)
    {
        sf::CircleShape indicator(settings.cellSize / 4);
        indicator.setPosition(sf::Vector2f(
            possibleMove.x * settings.cellSize + settings.cellSize / 4,
            possibleMove.y * settings.cellSize + settings.cellSize / 4));
        indicator.setFillColor(sf::Color(128, 128, 128, 180));
        window.draw(indicator);
    }
}
// Resets the token selection state
void resetSelection()
{
    tokenSelected = false;
    selectedPosition = {-1, -1};
    possibleMove = {-1, -1};
}
public:
// Constructor initializing game manager with board size and player names
GameManager(size_t gameSize, const std::string &player1, const std::string &player2)
: settings{
gameSize,
gameSize - 2,
static_cast<float>(600) / gameSize, // Cell size calculated from known window size
sf::VideoMode({600, 600})},
window(settings.videoMode, "Token Game"), state(settings.cellSize, settings.cellSize, gameSize), tokenSelected(false), winText(font, "", 30)
{
player1Name = player1;
player2Name = player2;
window.setFramerateLimit(60);
}


// Main game loop running until window is closed
void run()
{
    while (window.isOpen())
    {
		std::cout << "RUNNING NEW LOOOP" << std::endl;
        handleEvents();
		std::cout << "FINSIHED EVENTS" << std::endl;
		
        window.clear(sf::Color::White);
        state.getBoard().draw(window, settings.cellSize, settings.cellSize, false);
        renderSelection();

		std::cout << "FINSIHED DRAWING" << std::endl;

        if (Won)
        {
            window.draw(winwindow);
            window.draw(winText);
        }

        window.display();
        if (Won)
        {
            sf::sleep(sf::seconds(3)); // Pause for 3 seconds before closing
            window.close();
        }
    }
}
};

#endif // GAMEMANAGER_H
