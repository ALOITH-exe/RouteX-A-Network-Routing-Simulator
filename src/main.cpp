#include <SFML/Graphics.hpp>
#include "MainWindow.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "RouteX - Network Simulator");

    MainWindow app(window); // pass reference to window
    app.run();              // loading screen will show first inside run()

    return 0;
}
