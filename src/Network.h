#ifndef NETWORK_H
#define NETWORK_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "Packet.h"

struct Node {
    int id;
    sf::Vector2f position;
};

struct Edge {
    int to;
    int weight;  // integer weight
};

class Network {
public:
    std::vector<Node> nodes;
    std::vector<std::vector<Edge>> adjList;
    std::vector<Packet> packets;

    int selectedSource = -1;
    int selectedDest = -1;
    std::vector<int> shortestPath; // for highlighting Dijkstra path

    void addNode(sf::Vector2f pos);
    void addEdge(int u, int v, int weight);
    std::vector<int> dijkstra(int src, int dest);
    void update();
    void draw(sf::RenderWindow& window);
    
    void removeLastNode();
    void removeEdge(int u, int v);
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool bgLoaded = false;

};

#endif
