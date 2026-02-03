#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <SFML/Graphics.hpp>
#include "Network.h"
#include <stack>
#include "Action.h"


class MainWindow {
public:
    MainWindow(sf::RenderWindow& window);
    void run();
    int getNodeAt(sf::Vector2f mousePos);
private:
    sf::RenderWindow& window;
    Network network;
    std::stack<Action> undoStack;
    void undoLastAction();
    void handleEvents();
    void update();
    void render();
    void showLoadingScreen();
};

#endif
