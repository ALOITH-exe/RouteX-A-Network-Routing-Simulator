#include "Packet.h"
#include <cmath>

Packet::Packet(const std::vector<sf::Vector2f>& path) : path(path) {
    shape.setRadius(6.f);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin(6.f, 6.f);
    if (!path.empty())
        shape.setPosition(path[0]);
}

void Packet::move() {
    if (currentIndex + 1 < path.size()) {
        sf::Vector2f dir = path[currentIndex + 1] - shape.getPosition();
        float dist = std::hypot(dir.x, dir.y);
        if (dist < 2.f) {
            shape.setPosition(path[++currentIndex]);
        } else {
            dir /= dist;
            shape.move(dir * 0.5f);
        }
    }
}

bool Packet::finished() const {
    return currentIndex + 1 >= path.size();
}

void Packet::draw(sf::RenderWindow& window) {
    // Glow
    sf::CircleShape glow(shape.getRadius() * 2.f); // double radius
    glow.setOrigin(shape.getRadius() * 2.f, shape.getRadius() * 2.f);
    glow.setPosition(shape.getPosition());
    glow.setFillColor(sf::Color(0, 255, 255, 100)); // cyanish glow
    window.draw(glow);

    // Actual packet
    window.draw(shape);
}

