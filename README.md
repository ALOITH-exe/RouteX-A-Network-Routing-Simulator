# RouteX — Network Routing Simulator

RouteX is a **visual network routing simulator** built using **C++ and SFML**.  
It demonstrates **graph construction, Dijkstra’s shortest path algorithm**, and animated packet routing with a modern UI.

---

## ✨ Features
- Interactive node & edge creation
- Dijkstra’s shortest path visualization
- Animated glowing packet flow
- Neon-styled UI with loading screen
- Undo support (Ctrl + Z)
- Path weight visualization
- Cross-platform (Linux / Windows)

---

## 🧠 Algorithms Used
- Dijkstra’s Algorithm
- Graph adjacency list
- Real-time animation loop

---

## 🖱️ Controls
| Action | Key |
|------|----|
Add Node | Left Click |
Connect Nodes | Right Click |
Send Packet | P |
Undo | Ctrl + Z |

---

## 🛠️ Built With
- **C++17**
- **SFML**
- **MinGW / GCC**
- **Linux & Windows compatible**

---

## 📸 Preview
*(Add screenshots or GIFs here)*

---

## 📦 Build Instructions (Windows)

```bash
g++ -std=c++17 main.cpp MainWindow.cpp Network.cpp Packet.cpp RouteX_res.o \
-I SFML/include -L SFML/lib \
-lsfml-graphics -lsfml-window -lsfml-system -mwindows \
-o RouteX.exe

👨‍💻 Author

ALOITH
Cyber Security | Networking | Systems Programming