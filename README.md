# tank-battle
# 🎮 Tank Battle Simulator

This C++ project simulates a battle between two autonomous players, each controlling multiple tanks on a shared board.  
It was developed as part of an academic assignment in advanced programming and focuses on object-oriented design, simulation logic, and strategy implementation.

---

## 🧠 Project Description

The simulation consists of tanks moving on a 2D board, attempting to eliminate enemy tanks or complete objectives.  
Each tank is controlled by a player-defined algorithm, implemented through derived classes of an abstract interface.  
The GameManager handles simulation steps, player updates, and resolution of actions.

Two distinct algorithms were implemented for the two competing players:
- **Chasing-Algorithm**: uses Breadth-First Search (BFS) to navigate the board, pursue enemies, and optimize decision-making.
- **Basic-Algorithm**: avoids enemy tanks and incoming shells, and only fires when another tank is directly in its line of sight.

---

## 📁 Repository Structure

```
📄 src/           # Core implementation: Tank, Shell, Cell, GameManager, etc.
📄 common/        # Provided headers (ActionRequest, SatelliteView, etc.)
📄 mycommon/      # Custom implementations of interfaces (e.g., MyTankAlgorithm)
📄 include/       # Additional headers (optional)
📄 inputs/        # Sample input files for running simulations
📄 outputs/       # Expected output files for validation
📄 boards/        # Board layout files
📄 visualizer/    # Python-based visual tool to view game state (optional)
📄 Makefile       # Build file
📄 README.md      # Project description (you are here)
```

---

## 💪 Build Instructions

### Requirements:
- `g++` compiler with C++20 support
- `make` utility

### To compile:
```bash
make
```

This compiles all relevant `.cpp` files and creates an executable (e.g., `tank_game`).

---

## ▶️ How to Run

Use the executable with an input file from the `inputs/` folder:

```bash
./tank_game inputs/input_a.txt
```

You can compare the output to the reference files in `outputs/`.

---

## 🧩 Key Features

- Object-oriented design using inheritance and polymorphism
- Two distinct tank strategies (smart and basic)
- Modular structure for extending new behaviors
- Separation between interface (common) and implementation (mycommon)
- Text-based input/output for reproducible testing

---

## 📊 Visualizer (Optional)

A Python-based visualizer (`tank_visualizer_app.py`) is available in the `visualizer/` folder.  
You may run it as:

```bash
python3 visualizer/tank_visualizer_app.py
```

---
