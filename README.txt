Contributors:
-------------
Name: Yuval Namir Barr  
ID: 207004615  

Name: Roni Segal  
ID: 322270851  

Input and Output Format:
-------------------------
Each input file describes:
- A map name/description (line 1, ignored by the program).
- Game configuration lines:
    - MaxSteps = <number>
    - NumShells = <number>
    - Rows = <number>
    - Cols = <number>
- A game board of the specified dimensions.
- Initial tank positions with default cannon directions (Left for Player 1, Right for Player 2).
- Terrain elements like walls (#) and mines (@).
- Empty cells are denoted by spaces.

Legend:
- '1' = Tank belonging to Player 1  
- '2' = Tank belonging to Player 2  
- '#' = Wall  
- '@' = Mine  
- ' ' = Empty space  

Each output file (output_<input>.txt) contains:
- A line per game turn, listing the action taken by each tank in order of appearance.
- "killed" tags indicate when a tank is destroyed.
- "(ignored)" tags indicate illegal or unperformed actions.
- The final line indicates the game result.

Input Files:
------------
input_a.txt   — Maze scenario with walls, tie expected.  
input_b.txt  — Many tanks per player, Player 2 wins.  
input_c.txt  — Many mines, Player 1 wins by outlasting.

Each input_<N>.txt generates a corresponding output_<N>.txt file:
output_a.txt, output_b.txt, output_c.txt

How to Run:
-----------
To compile and run:

    make
    ./tanks_game input_a.txt

This will generate:

    output_a.txt

Notes:
------
- If the board file is invalid, error messages will be printed and the game will not run.