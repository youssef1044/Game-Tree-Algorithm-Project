#ifndef MAINMENU_H
#define MAINMENU_H

#include "GameManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <string>
#include <sstream>

// Class representing the main menu UI for the game setup
class MainMenu
{
private:
// Struct representing an input field with rectangle, label, content, and active state
struct InputField
{
sf::RectangleShape rect; // Rectangle shape for input box
sf::Text label;          // Label text for the input field
sf::Text content;        // Content text entered by the user
bool isActive = false;   // Whether the input field is currently active/focused
};


sf::RenderWindow window;  // SFML window for rendering the menu
sf::Font font;            // Font used for text rendering

// Text objects for title and buttons
sf::Text title;
sf::Text playButton;
sf::Text exitButton;

// Input fields for player names and board size
InputField player1Field;
InputField player2Field;
InputField boardSizeField;

sf::RectangleShape inputBackground; // Background rectangle for input area
sf::Clock cursorClock;               // Clock to manage blinking cursor
bool showCursor = true;              // Flag to toggle cursor visibility

// Initializes a text object with string, position, and color
void initializeText(sf::Text &text, const std::string &str, float yPos)
{
    text.setString(str);
    text.setOrigin(sf::Vector2f(text.getLocalBounds().size.x / 2, text.getLocalBounds().size.y / 2));
    text.setPosition(sf::Vector2f(window.getSize().x / 2.0f, yPos));
    text.setFillColor(sf::Color::White);
}

// Creates and configures an input field at a given vertical position with a label
void createInputField(InputField &field, float yPos, const std::string &label)
{
    // Label configuration
    field.label.setString(label);
    field.label.setPosition(sf::Vector2f(50, yPos - 35));

    // Input box configuration
    field.rect.setSize(sf::Vector2f(500, 40));
    field.rect.setPosition(sf::Vector2f(50, yPos));
    field.rect.setFillColor(sf::Color(70, 70, 70));
    field.rect.setOutlineThickness(2);

    // Content text position
    field.content.setPosition(sf::Vector2f(60, yPos + 5));
}

// Loads the font from file, throws if loading fails
sf::Font loadFont()
{
    sf::Font localFont;
    if (!localFont.openFromFile("arial.ttf"))
    {
        throw std::runtime_error("Failed to load font!");
    }
    return localFont;
}
public:
// Constructor initializes window, font, texts, and input fields
MainMenu() : window(sf::VideoMode({600, 600}), "Main Menu"),
font(loadFont()),
title(font, "", 40),
playButton(font, "", 30),
exitButton(font, "", 30),
player1Field{
sf::RectangleShape{},
sf::Text(font, "", 24),
sf::Text(font, "", 24)},
player2Field{
sf::RectangleShape{},
sf::Text(font, "", 24),
sf::Text(font, "", 24)},
boardSizeField{
sf::RectangleShape{},
sf::Text(font, "", 24),
sf::Text(font, "", 24)}
{
// Title configuration
initializeText(title, "Game Setup", 50);


    // Input fields setup
    createInputField(player1Field, 150, "Player 1 Name:");
    createInputField(player2Field, 250, "Player 2 Name:");
    createInputField(boardSizeField, 350, "Board Size:");

    // Buttons configuration
    initializeText(playButton, "Start Game", 450);
    initializeText(exitButton, "Exit", 520);

    // Input background rectangle configuration
    inputBackground.setSize(sf::Vector2f(580, 300));
    inputBackground.setPosition(sf::Vector2f(10, 100));
    inputBackground.setFillColor(sf::Color(50, 50, 50, 200));
}

// Main loop running the menu until window is closed
void run()
{
    while (window.isOpen())
    {
        handleEvents();
        update();
        render();
    }
}
private:
// Returns the entered player 1 name as string
std::string getPlayer1Name() const
{
return player1Field.content.getString().toAnsiString();
}


// Returns the entered player 2 name as string
std::string getPlayer2Name() const
{
    return player2Field.content.getString().toAnsiString();
}

// Returns the entered board size as integer, with default fallback
int getBoardSize() const
{
    try
    {
        return std::stoi(boardSizeField.content.getString().toAnsiString()) + 2;
    }
    catch (...)
    {
        return 3; // Default fallback
    }
}

// Handles SFML events such as window close, mouse clicks, and text input
void handleEvents()
{
    while (auto event = window.pollEvent())
    {
        if (!event.has_value())
            break;

        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
        {
            handleMouseClick(sf::Vector2f(mousePress->position.x, mousePress->position.y));
        }

        if (auto *textEvent = event->getIf<sf::Event::TextEntered>())
        {
            handleTextInput(*textEvent);
        }
    }
}

// Handles mouse click events, including input field focus and button clicks
void handleMouseClick(sf::Vector2f mousePos)
{
    auto checkField = [&](InputField &field)
    {
        if (field.rect.getGlobalBounds().contains(mousePos))
        {
            field.isActive = true;
            return true;
        }
        field.isActive = false;
        return false;
    };

    checkField(player1Field);
    checkField(player2Field);
    checkField(boardSizeField);

    if (playButton.getGlobalBounds().contains(mousePos))
    {
        if (validateInput())
        {
            const size_t bSize = getBoardSize();
            const std::string player1Name = getPlayer1Name();
            const std::string player2Name = getPlayer2Name();

            GameManager gameManager(bSize, player1Name, player2Name);
            gameManager.run();
        }
    }
    else if (exitButton.getGlobalBounds().contains(mousePos))
    {
        window.close();
    }
}

// Handles text input for active input fields, supports backspace and character input
void handleTextInput(const sf::Event::TextEntered &event)
{
    auto processField = [&](InputField &field, bool numbersOnly = false)
    {
        if (!field.isActive)
            return;

        if (event.unicode == 8)
        { // Backspace
            std::string str = field.content.getString();
            if (!str.empty())
            {
                str.pop_back();
                field.content.setString(str);
            }
        }
        else if (event.unicode >= 32 && event.unicode < 128)
        {
            char c = static_cast<char>(event.unicode);
            if (!numbersOnly || (c >= '0' && c <= '9'))
            {
                field.content.setString(field.content.getString() + c);
            }
        }
    };

    processField(player1Field);
    processField(player2Field);
    processField(boardSizeField, true);
}

// Validates the input fields, highlighting errors and returning overall validity
bool validateInput()
{
    bool valid = true;
    auto setError = [&](InputField &field, bool error)
    {
        field.rect.setOutlineColor(error ? sf::Color::Red : sf::Color::Transparent);
        if (error)
            valid = false;
    };

    setError(player1Field, player1Field.content.getString().isEmpty());
    setError(player2Field, player2Field.content.getString().isEmpty());

    try
    {
        int size = std::stoi(boardSizeField.content.getString().toAnsiString());
        setError(boardSizeField, size < 3);
    }
    catch (...)
    {
        setError(boardSizeField, true);
    }

    return valid;
}

// Updates the blinking cursor visibility based on elapsed time
void update()
{
    if (cursorClock.getElapsedTime().asSeconds() > 0.5)
    {
        showCursor = !showCursor;
        cursorClock.restart();
    }
}

// Renders the menu UI including background, input fields, and buttons
void render()
{
    window.clear(sf::Color(30, 30, 30));

    // Draw background rectangle behind input fields
    window.draw(inputBackground);

    // Draw each input field with label and content, including blinking cursor if active
    auto drawField = [&](const InputField &field)
    {
        window.draw(field.rect);
        window.draw(field.label);

        sf::Text content = field.content;
        if (field.isActive && showCursor)
        {
            content.setString(content.getString() + "_");
        }
        window.draw(content);
    };

    drawField(player1Field);
    drawField(player2Field);
    drawField(boardSizeField);

    // Draw title and buttons
    window.draw(title);
    window.draw(playButton);
    window.draw(exitButton);

    window.display();
}
};

#endif // MAINMENU_H