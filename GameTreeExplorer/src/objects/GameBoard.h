#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <utility>
#include "Algo.h"
#include "Token.h"

// Class representing the game board grid and managing tokens on it
class GameBoard
{
private:
size_t Width;  // Width of the board (number of columns)
size_t Height; // Height of the board (number of rows)
std::vector<std::vector<Token *>> board; // 2D grid storing pointers to tokens
sf::Color borderColor = sf::Color::Black; // Color used for grid borders
unsigned borderThickness = 2; // Thickness of grid borders


// Checks if the given (x, y) position is within the board boundaries
bool isValidPosition(int x, int y) const
{
    return x >= 0 && y >= 0 &&
           static_cast<size_t>(x) < Width &&
           static_cast<size_t>(y) < Height;
}

// Returns the color of a cell based on its position and opacity
// Edge rows and columns have special colors for visual distinction
sf::Color getCellColor(size_t row, size_t col, std::uint8_t opacity) const
{
    if (row >= Height || col >= Width)
        return sf::Color::Black;

    const bool isEdgeRow = (row == 0 || row == Height - 1);
    const bool isEdgeCol = (col == 0 || col == Width - 1);

    if (isEdgeRow && isEdgeCol)
        return {184, 176, 170, opacity}; // Gray corners
    if (isEdgeRow)
        return {210, 241, 210, opacity}; // Green borders
    if (isEdgeCol)
        return {250, 210, 210, opacity}; // Red borders
    return {255, 255, 255, opacity};    // White cells
}

// Draws a single cell at (row, col) with given width, height, and opacity
void drawCell(sf::RenderWindow &window, size_t row, size_t col,
              float cellW, float cellH, int opacity) const
{
    sf::RectangleShape cell({cellW - borderThickness, cellH - borderThickness});
    cell.setPosition(sf::Vector2f(
        col * cellW + borderThickness / 2.0f,
        row * cellH + borderThickness / 2.0f));
    cell.setFillColor(getCellColor(row, col, opacity));
    window.draw(cell);
}

// Draws the grid lines for the board on the window
void drawGridLines(sf::RenderWindow &window, float cellW, float cellH) const
{
    // Vertical lines
    for (size_t col = 0; col <= Width; ++col)
    {
        sf::RectangleShape line({static_cast<float>(borderThickness), Height * cellH});
        line.setPosition(sf::Vector2f(col * cellW - borderThickness / 2.0f, 0.0f));
        line.setFillColor(borderColor);
        window.draw(line);
    }

    // Horizontal lines
    for (size_t row = 0; row <= Height; ++row)
    {
        sf::RectangleShape line({Width * cellW, static_cast<float>(borderThickness)});
        line.setPosition(sf::Vector2f(
            0.0f, row * cellH - borderThickness / 2.0f));
        line.setFillColor(borderColor);
        window.draw(line);
    }
}

// Draws all tokens on the board with given cell dimensions and opacity
void placeTokens(sf::RenderWindow &window, float cellW, float cellH, int opacity) const
{
    for (size_t row = 0; row < Height; ++row)
    {
        for (size_t col = 0; col < Width; ++col)
        {
            if (board[row][col])
            {
                board[row][col]->draw(window, cellW, cellH, opacity);
            }
        }
    }
}
public:
// Constructor initializing board with given width and height, empty tokens
GameBoard(size_t width, size_t height)
: Width(width), Height(height),
board(height, std::vector<Token *>(width, nullptr)) {}


// Copy constructor performing deep copy of tokens
GameBoard(const GameBoard& other) 
	: Width(other.Width),
	  Height(other.Height),
	  board(other.Height, std::vector<Token*>(other.Width, nullptr)),
	  borderColor(other.borderColor),
	  borderThickness(other.borderThickness)
{
	// Deep copy all tokens
	for (size_t row = 0; row < Height; ++row) {
		for (size_t col = 0; col < Width; ++col) {
			if (other.board[row][col] != nullptr) {
				// Create a new copy of the token
				board[row][col] = new Token(*other.board[row][col]);
			}
		}
	}
}

// Destructor clears token pointers (does not delete tokens)
~GameBoard()
{
    for (auto &row : board)
    {
        for (auto &cell : row)
        {
            cell = nullptr;
        }
    }
}

// Places a token on the board at its current position
void placeToken(Token *token)
{
    const auto [x, y] = token->getPosition();
    if (!isValidPosition(x, y))
    {
        throw std::out_of_range("Invalid token position");
    }
    board[y][x] = token;
}

// Moves a token from (fromX, fromY) to (toX, toY) without validation
void moveTokenRaw(int fromX, int fromY, int toX, int toY)
{
    size_t fX = fromX, fY = fromY;
    size_t tX = toX, tY = toY;

    if (!board[fY][fX])
        throw std::runtime_error("No token at source position");
    Token *movingToken = board[fY][fX];

    board[tY][tX] = movingToken;
    board[fY][fX] = nullptr;
    movingToken->move(tX, tY);
	movingToken->undoReachedEnd();
    updateTokenMoveStatus();

    // Mark token as reached end if moved to board edge
    if (tX == 0 || tX == Width - 1 || tY == 0 || tY == Height - 1)
    {
        movingToken->tokenReachedEnd();
    }    
}

// Moves a token with validation and jump handling
void moveToken(int fromX, int fromY, int toX, int toY)
{
    if (!isValidPosition(fromX, fromY) || !isValidPosition(toX, toY))
    {
        throw std::out_of_range("Move coordinates out of bounds");
    }

    size_t fX = fromX, fY = fromY;
    size_t tX = toX, tY = toY;

    if (!board[fY][fX])
        throw std::runtime_error("No token at source position");
    Token *movingToken = board[fY][fX];

    if (!movingToken->isMovable())
    {
        throw std::runtime_error("Token is immovable");
    }

    // Handle potential jumps over other tokens
    if (board[tY][tX])
    {
        const int player = movingToken->getPlayer();
        if (player == 0)
        {
            if (tX >= Width - 1 || board[tY][tX + 1])
                throw std::runtime_error("Can't jump");
            tX++;
        }
        else if (player == 1)
        {
            if (tY >= Height - 1 || board[tY + 1][tX])
                throw std::runtime_error("Can't jump");
            tY++;
        }
    }

    // Validate final position after jump
    if (!isValidPosition(tX, tY))
    {
        throw std::out_of_range("Jump moves out of bounds");
    }

    // Perform the move
    board[tY][tX] = movingToken;
    board[fY][fX] = nullptr;
    movingToken->move(tX, tY);
    updateTokenMoveStatus();

    // Mark token as reached end if moved to board edge
    if (tX == 0 || tX == Width - 1 || tY == 0 || tY == Height - 1)
    {
        movingToken->tokenReachedEnd();
    }
}

// Updates the movable status of all tokens on the board
void updateTokenMoveStatus()
{
    for (size_t row = 0; row < Height; ++row)
    {
        for (size_t col = 0; col < Width; ++col)
        {
            if (board[row][col])
            {
                if (canTokenMove(board[row][col]))
                {
                    board[row][col]->setMovable(true);
                }
                else
                {
                    board[row][col]->setMovable(false);
                }
            }
        }
    }
}

// Determines the valid move for a token from (fromX, fromY) to (toX, toY)
// Returns the actual move coordinates or (-1, -1) if invalid
std::pair<int, int> getTokenMove(int fromX, int fromY, int toX, int toY) const
{
    try
    {
        if (!isValidPosition(fromX, fromY) || !isValidPosition(toX, toY))
        {
            return {-1, -1};
        }

        const Token *token = board[fromY][fromX];
        if (!token || !token->isMovable())
            return {-1, -1};

        // Check direct move
        if (!board[toY][toX])
            return {toX, toY};

        // Handle jump possibility
        const int player = token->getPlayer();
        if (player == 0 && toX < Width - 1 && !board[toY][toX + 1])
        {
            return {toX + 1, toY};
        }
        if (player == 1 && toY < Height - 1 && !board[toY + 1][toX])
        {
            return {toX, toY + 1};
        }

        return {-1, -1};
    }
    catch (...)
    {
        return {-1, -1};
    }
}

// Checks if a token can move from its current position
bool canTokenMove(const Token *token) const
{
    const auto [x, y] = token->getPosition();
    if (!isValidPosition(x, y))
        return false;

    const int player = token->getPlayer();
    int dx = (player == 0) ? 1 : 0;
    int dy = (player == 1) ? 1 : 0;

    // Check basic move
    if (!isValidPosition(x + dx, y + dy))
        return false;
    if (!board[y + dy][x + dx])
        return true;

    // Check jump possibility
    if (player == 0 && x + 2 < Width)
    {
        return !board[y][x + 2];
    }
    if (player == 1 && y + 2 < Height)
    {
        return !board[y + 2][x];
    }

    return false;
}

// Draws the entire board including cells, grid lines, and tokens
void draw(sf::RenderWindow &window, float cellW, float cellH, bool preview) const
{
    // Draw cells with opacity based on preview flag
    for (size_t row = 0; row < Height; ++row)
    {
        for (size_t col = 0; col < Width; ++col)
        {
            drawCell(window, row, col, cellW, cellH, (preview ? 100 : 255 ));
        }
    }

    // Draw grid lines
    drawGridLines(window, cellW, cellH);

    // Draw tokens with opacity based on preview flag
    placeTokens(window, cellW, cellH, ( preview ? 150 : 255 ));
}

// Prints the board state to console with indentation
void printBoard(std::string indent) 
{
    for (const auto &row : board)
    {
		cout << indent;
        for (const auto &cell : row)
        {
            std::cout << (cell ? std::to_string(cell->getPlayer()) : ".") << " ";
        }
        std::cout << "\n";
    }
}

// Prints the board state to console without indentation
void printBoard() 
{
    for (const auto &row : board)
    {
        for (const auto &cell : row)
        {
            std::cout << (cell ? std::to_string(cell->getPlayer()) : ".") << " ";
        }
        std::cout << "\n";
    }
}

// Returns the token pointer at position (x, y) or nullptr if invalid
Token *getTokenAt(int x, int y) const
{
    if (!isValidPosition(x, y))
        return nullptr;
    return board[y][x];
}
};

#endif // GAMEBOARD_H
