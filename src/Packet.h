#ifndef PACKET_H
#define PACKET_H

#include <SFML/Graphics.hpp>
#include <vector>

class Packet {
public:
    Packet(const std::vector<sf::Vector2f>& path);
    void move();
    bool finished() const;
    void draw(sf::RenderWindow& window);
    sf::CircleShape shape;
private:
    std::vector<sf::Vector2f> path;
    size_t currentIndex = 0;

};

#endif
