#include "Network.h"
#include <cmath>
#include <algorithm>
#include <queue>
#include <limits>
#include <iostream>

//////////////// Node & Edge management //////////////////

void Network::addNode(sf::Vector2f pos) {
    int id = nodes.size();
    nodes.push_back({id, pos});
    adjList.push_back({});
}

void Network::addEdge(int u, int v, int weight) {
    // avoid duplicates
    for (auto& e : adjList[u])
        if (e.to == v) return;

    adjList[u].push_back({v, weight});
    adjList[v].push_back({u, weight}); // undirected
}

void Network::removeLastNode() {
    if (nodes.empty()) return;

    int id = nodes.back().id;
    nodes.pop_back();
    adjList.pop_back();

    // remove edges pointing to this node
    for (auto& edges : adjList) {
        edges.erase(std::remove_if(edges.begin(), edges.end(),
                                   [id](auto& e){ return e.to == id; }),
                    edges.end());
    }
}

void Network::removeEdge(int u, int v) {
    adjList[u].erase(std::remove_if(adjList[u].begin(), adjList[u].end(),
                                    [v](auto& e){ return e.to == v; }),
                     adjList[u].end());

    adjList[v].erase(std::remove_if(adjList[v].begin(), adjList[v].end(),
                                    [u](auto& e){ return e.to == u; }),
                     adjList[v].end());
}

//////////////// Dijkstra //////////////////

std::vector<int> Network::dijkstra(int src, int dest) {
    int n = nodes.size();
    std::vector<float> dist(n, std::numeric_limits<float>::infinity());
    std::vector<int> parent(n, -1);
    std::priority_queue<std::pair<float,int>,
                        std::vector<std::pair<float,int>>,
                        std::greater<>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (u == dest) break;

        for (auto& e : adjList[u]) {
            int v = e.to;
            int w = e.weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    std::vector<int> path;
    for (int v = dest; v != -1; v = parent[v])
        path.push_back(v);
    std::reverse(path.begin(), path.end());
    return path;
}

//////////////// Update packets //////////////////

void Network::update() {
    for (auto it = packets.begin(); it != packets.end();) {
        it->move();
        if (it->finished())
            it = packets.erase(it);
        else
            ++it;
    }
}

//////////////// Draw //////////////////

void Network::draw(sf::RenderWindow& window) {
    // Load font once
    static sf::Font font;
    static bool loaded = false;
    if (!loaded) {
        if (!font.loadFromFile("../resources/arial.ttf"))
            std::cout << "Failed to load font\n";
        loaded = true;
    }
    if (!bgLoaded) {
    if (!backgroundTexture.loadFromFile("../resources/background.jpg")) {
        std::cout << "Failed to load background image\n";
    } else {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
    window.getSize().x / float(backgroundTexture.getSize().x),
    window.getSize().y / float(backgroundTexture.getSize().y)
);

    }
    bgLoaded = true;
}

// Draw the background first
window.draw(backgroundSprite);

    // --- Draw edges ---
    for (int u = 0; u < adjList.size(); ++u) {
        for (auto& e : adjList[u]) {
            if (u < e.to) { // draw each undirected edge only once
                sf::Color edgeColor = sf::Color::White;

                // Highlight if edge is part of shortest path
                for (size_t i = 0; i + 1 < shortestPath.size(); ++i) {
                    int from = shortestPath[i];
                    int to = shortestPath[i + 1];
                    if ((u == from && e.to == to) || (u == to && e.to == from)) {
                        edgeColor = sf::Color::Cyan;
                        break;
                    }
                }

                // Draw edge line
                sf::Vertex line[] = {
                    sf::Vertex(nodes[u].position, edgeColor),
                    sf::Vertex(nodes[e.to].position, edgeColor)
                };
                window.draw(line, 2, sf::Lines);

                // Draw edge weight
                sf::Text text;
                text.setFont(font);
                text.setString(std::to_string(e.weight));
                text.setCharacterSize(14);
                text.setFillColor(sf::Color::White);
                text.setPosition((nodes[u].position + nodes[e.to].position) / 2.f);
                window.draw(text);
            }
        }
    }

    // --- Draw nodes with glow ---
    for (auto &n : nodes) {
        // Glow size
        float glowRadius = 18.f;
        sf::CircleShape glow(glowRadius);
        glow.setOrigin(glowRadius, glowRadius);
        glow.setPosition(n.position);

        if (n.id == selectedSource)
            glow.setFillColor(sf::Color(50, 100, 255, 100)); // blue glow
        else if (n.id == selectedDest)
            glow.setFillColor(sf::Color(255, 50, 50, 100));  // red glow
        else
            glow.setFillColor(sf::Color(50, 255, 50, 80));   // green glow

        window.draw(glow);

        // Actual node
        float nodeRadius = 12.f;
        sf::CircleShape shape(nodeRadius);
        shape.setOrigin(nodeRadius, nodeRadius);
        shape.setPosition(n.position);

        if (n.id == selectedSource)
            shape.setFillColor(sf::Color::Blue);
        else if (n.id == selectedDest)
            shape.setFillColor(sf::Color::Red);
        else
            shape.setFillColor(sf::Color::Green);

        window.draw(shape);
    }

    // --- Draw packets ---
    for (auto &p : packets) {
        // Optional: add glow around packet
        sf::CircleShape glow(8.f);
        glow.setOrigin(8.f, 8.f);
        glow.setPosition(p.shape.getPosition());
        glow.setFillColor(sf::Color(0, 255, 255, 100)); // cyan glow
        window.draw(glow);

        p.draw(window); // actual packet
    }
}

