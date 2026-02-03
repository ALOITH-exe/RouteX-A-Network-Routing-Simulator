#include "MainWindow.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

// Particle struct for loading animation
struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
};

MainWindow::MainWindow(sf::RenderWindow& win)
    : window(win) {}

// ---------------- RUN ----------------
void MainWindow::run() {
    showLoadingScreen(); // Show loading + instructions first

    while (window.isOpen()) {
        handleEvents();
        network.update();

        window.clear(sf::Color::Black);
        network.draw(window);
        window.display();
    }
}

// ---------------- EVENTS ----------------
void MainWindow::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {

        // CTRL+Z → Undo
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Z &&
            event.key.control) {
            undoLastAction();
        }

        if (event.type == sf::Event::Closed)
            window.close();

        // LEFT CLICK → Add node or select
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            int nodeId = getNodeAt(mousePos);

            if (nodeId == -1) {
                network.addNode(mousePos);
                undoStack.push({ ActionType::AddNode, network.nodes.back().id, -1, -1 });

            } else {
                if (network.selectedSource == -1) {
                    network.selectedSource = nodeId;
                    undoStack.push({ ActionType::SelectNode, nodeId, -1, -1 });
                } else if (network.selectedDest == -1 && nodeId != network.selectedSource) {
                    network.selectedDest = nodeId;
                    undoStack.push({ ActionType::SelectNode, nodeId, -1, -1 });
                }
            }
        }

        // RIGHT CLICK → Add edge
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Right) {

            if (network.selectedSource != -1 &&
                network.selectedDest != -1) {

                auto& a = network.nodes[network.selectedSource];
                auto& b = network.nodes[network.selectedDest];

                int weight = static_cast<int>(std::hypot(
                    a.position.x - b.position.x,
                    a.position.y - b.position.y
                ));

                network.addEdge(a.id, b.id, weight);
                undoStack.push({ ActionType::AddEdge, -1, a.id, b.id });

                network.selectedSource = -1;
                network.selectedDest = -1;
            }
        }

        // PRESS P → Send packet along shortest path
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::P) {

            if (network.selectedSource != -1 &&
                network.selectedDest != -1) {

                auto pathIds = network.dijkstra(network.selectedSource,
                                                network.selectedDest);
                network.shortestPath = pathIds; // highlight path

                std::vector<sf::Vector2f> path;
                for (int id : pathIds)
                    path.push_back(network.nodes[id].position);

                network.packets.emplace_back(path);
            }
        }
    }
}

// ---------------- UPDATE ----------------
void MainWindow::update() {
    network.update();
}

void MainWindow::render() {
    window.clear(sf::Color::Black);
    network.draw(window);
    window.display();
}

// ---------------- NODE CHECK ----------------
int MainWindow::getNodeAt(sf::Vector2f mousePos) {
    for (auto& n : network.nodes) {
        if (std::hypot(mousePos.x - n.position.x,
                       mousePos.y - n.position.y) <= 12.f) {
            return n.id;
        }
    }
    return -1;
}

// ---------------- UNDO ----------------
void MainWindow::undoLastAction() {
    if (undoStack.empty()) return;

    Action action = undoStack.top();
    undoStack.pop();

    switch (action.type) {
        case ActionType::AddNode: network.removeLastNode(); break;
        case ActionType::AddEdge: network.removeEdge(action.u, action.v); break;
        case ActionType::SelectNode:
            if (network.selectedDest == action.nodeId) network.selectedDest = -1;
            else if (network.selectedSource == action.nodeId) network.selectedSource = -1;
            break;
    }
}

// ---------------- LOADING SCREEN ----------------
void MainWindow::showLoadingScreen() {
    window.clear(sf::Color::Black);

    // --- Load Font ---
    sf::Font font;
    if (!font.loadFromFile("../resources/Orbitron.ttf")) {
        std::cout << "Failed to load font\n";
        return;
    }

    float windowWidth = window.getSize().x;
    float windowHeight = window.getSize().y;

    // --- Progress bar ---
    const float barWidth = 400.f;
    const float barHeight = 20.f;
    sf::RectangleShape barBackground(sf::Vector2f(barWidth, barHeight));
    barBackground.setFillColor(sf::Color(20, 30, 60));
    barBackground.setPosition((windowWidth - barWidth)/2, windowHeight/2);

    sf::RectangleShape barFill(sf::Vector2f(0, barHeight));
    barFill.setFillColor(sf::Color(0, 180, 255));
    barFill.setPosition((windowWidth - barWidth)/2, windowHeight/2);

    // --- Particles ---
    struct Particle {
        sf::CircleShape shape;
        sf::Vector2f velocity;
    };
    std::vector<Particle> particles(15);
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (auto &p : particles) {
        p.shape = sf::CircleShape(3.f);
        p.shape.setFillColor(sf::Color(0, 180, 255));
        p.shape.setPosition((windowWidth - barWidth)/2 + std::rand() % (int)barWidth,
                            windowHeight/2 - 20 + std::rand() % 40);
        p.velocity = sf::Vector2f((std::rand()%3 - 1)*0.5f, (std::rand()%3 - 1)*0.5f);
    }

    // --- Loading Title ---
    sf::Text loadingTitle("RouteX Loading", font, 40);
    loadingTitle.setFillColor(sf::Color::White);
    loadingTitle.setPosition((windowWidth - loadingTitle.getLocalBounds().width)/2,
                             windowHeight/2 - 100);

    sf::Text titleGlow = loadingTitle;
    titleGlow.setFillColor(sf::Color(0,255,100,100));
    titleGlow.setPosition(loadingTitle.getPosition() + sf::Vector2f(2,2));

    // --- LOADING LOOP (progress bar + particles only) ---
    float progress = 0.f;
    const float progressSpeed = 2.f; // speed
    const sf::Time frameDelay = sf::milliseconds(12);

    while (progress < barWidth) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) return;
        }

        progress += progressSpeed;
        if (progress > barWidth) progress = barWidth;
        barFill.setSize(sf::Vector2f(progress, barHeight));

        // Animate particles
        for (auto &p : particles) {
            p.shape.move(p.velocity);
            if (p.shape.getPosition().x < (windowWidth - barWidth)/2 ||
                p.shape.getPosition().x > (windowWidth + barWidth)/2) p.velocity.x *= -1;
            if (p.shape.getPosition().y < windowHeight/2 - 20 ||
                p.shape.getPosition().y > windowHeight/2 + 20) p.velocity.y *= -1;
        }

        // Draw loading only
        window.clear(sf::Color::Black);
        window.draw(titleGlow);
        window.draw(loadingTitle);
        window.draw(barBackground);
        window.draw(barFill);
        for (auto &p : particles) window.draw(p.shape);
        window.display();

        sf::sleep(frameDelay);
    }

    // --- INSTRUCTIONS AFTER LOADING ---
    // --- INSTRUCTIONS AFTER LOADING ---
std::vector<std::string> instrLines = {
    " Controls:",
    "  Left Click: Add / Select Node",
    "  Right Click: Connect Nodes",
    "  P Key: Send Packet",
    "  Ctrl + Z: Undo Last Action",
    "",
    "  Press any key to continue..."
};

// Neon colors for each line
std::vector<sf::Color> colors = {
    sf::Color(0,255,255),    // cyan
    sf::Color(255,0,255),    // magenta
    sf::Color(0,255,0),      // green
    sf::Color(255,255,0),    // yellow
    sf::Color(0,150,255),    // blueish
    sf::Color(255,255,255),  // white
    sf::Color(255,100,0)     // orange
};

// Compute starting Y to center the whole block
float totalHeight = instrLines.size() * 30.f; // line height = 30
float startY = (window.getSize().y - totalHeight) / 2;

// Draw each line with glow
std::vector<sf::Text> instrText, glowText;
for (size_t i = 0; i < instrLines.size(); i++) {
    sf::Text line(instrLines[i], font, 25);
    line.setFillColor(colors[i]);
    float xPos = (window.getSize().x - line.getLocalBounds().width) / 2;
    line.setPosition(xPos, startY + i*30);
    instrText.push_back(line);

    // Glow
    sf::Text glow = line;
    glow.setFillColor(sf::Color(colors[i].r/2, colors[i].g/2, colors[i].b/2, 150));
    glow.setPosition(line.getPosition() + sf::Vector2f(2,2));
    glowText.push_back(glow);
}

// Clear loading screen
window.clear(sf::Color::Black);

// Draw all instruction lines with glow
for (size_t i = 0; i < instrText.size(); i++) {
    window.draw(glowText[i]);
    window.draw(instrText[i]);
}
window.display();

// Wait for key press
while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) return;
        if (event.type == sf::Event::KeyPressed) return;
    }
}

}
