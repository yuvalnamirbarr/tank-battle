
import streamlit as st
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import re
from io import StringIO

class Tank:
    def __init__(self, idx, player, y, x, direction):
        self.idx = idx
        self.player = player
        self.y = y
        self.x = x
        self.dir = direction
        self.alive = True

    def rotate_left(self):
        self.dir = {'N': 'W', 'W': 'S', 'S': 'E', 'E': 'N'}[self.dir]

    def move_forward(self):
        if self.dir == 'N': self.y -= 1
        elif self.dir == 'S': self.y += 1
        elif self.dir == 'E': self.x += 1
        elif self.dir == 'W': self.x -= 1

st.title("Tank Game Visualizer")

input_file = st.file_uploader("Upload input.txt", type="txt")
output_file = st.file_uploader("Upload output.txt", type="txt")
board_file = st.file_uploader("Upload boards.txt", type="txt")

if input_file and output_file and board_file:
    input_data = input_file.read().decode()
    output_data = output_file.read().decode()
    boards_data = board_file.read().decode()

    try:
        rows = int(re.search(r"Rows=(\d+)", input_data).group(1))
        cols = int(re.search(r"Cols=(\d+)", input_data).group(1))
    except:
        st.error("Error: Could not extract board size from input.txt")
        st.stop()

    initial_board_lines = [line.rstrip() for line in input_data.splitlines() if line.startswith("#")]
    tanks = []
    for y, line in enumerate(initial_board_lines):
        for x, ch in enumerate(line):
            if ch == '1':
                tanks.append(Tank(len(tanks), 1, y, x, 'W'))
            elif ch == '2':
                tanks.append(Tank(len(tanks), 2, y, x, 'E'))

    actions_per_step = []
    for line in output_data.strip().splitlines():
        if "tie" in line.lower():
            continue
        actions = [act.strip() for act in line.split(",")]
        while len(actions) < len(tanks):
            actions.append("GetBattleInfo")
        actions_per_step.append(actions)

    raw_boards = [blk.strip().splitlines() for blk in boards_data.strip().split("===\n")]
    if not raw_boards:
        st.error("No board frames found in boards.txt")
        st.stop()

    while len(raw_boards) < len(actions_per_step):
        raw_boards.append(raw_boards[-1])

    dir_arrows = {'N': '↑', 'E': '→', 'S': '↓', 'W': '←'}
    tile_colors = {
        "#": (0, 0, 0), " ": (1, 1, 1), ".": (1, 1, 1),
        "1": (0.1, 0.4, 0.9), "2": (0.9, 0.1, 0.1),
        "*": (1.0, 0.55, 0.0), "@": (0.5, 0.5, 0.5),
    }

    tanks_snapshots = []
    for step_idx, actions in enumerate(actions_per_step):
        for t_idx, action_token in enumerate(actions):
            tank = tanks[t_idx]
            if not tank.alive:
                continue
            killed = "(killed)" in action_token.lower()
            ignored = "(ignored)" in action_token.lower()
            action = re.sub(r"\s*\(.*?\)", "", action_token).strip()
            if not ignored:
                if action == "MoveForward":
                    tank.move_forward()
                elif action == "RotateLeft90":
                    tank.rotate_left()
            if killed:
                tank.alive = False
        tanks_snap = [Tank(t.idx, t.player, t.y, t.x, t.dir) for t in tanks if t.alive]
        tanks_snapshots.append(tanks_snap)

    if len(raw_boards) == 0:
        st.warning("No frames to display.")
        st.stop()

    fig, ax = plt.subplots(figsize=(cols / 2, rows / 2))
    ax.set_xticks([])
    ax.set_yticks([])
    im = ax.imshow(np.ones((rows, cols, 3)))
    arrow_texts = []

    def update(frame_idx):
        for txt in arrow_texts:
            txt.remove()
        arrow_texts.clear()
        board = raw_boards[frame_idx]
        img = np.ones((rows, cols, 3))
        for y in range(rows):
            for x in range(cols):
                ch = board[y][x]
                img[y, x] = tile_colors.get(ch, (1, 1, 1))
        im.set_data(img)
        ax.set_title(f"Step {frame_idx + 1}/{len(raw_boards)}")
        for tank in tanks_snapshots[frame_idx]:
            arrow = dir_arrows[tank.dir]
            txt = ax.text(tank.x, tank.y, arrow, ha='center', va='center',
                          fontsize=12, color='white', weight='bold')
            arrow_texts.append(txt)
        return [im] + arrow_texts

    ani = animation.FuncAnimation(fig, update, frames=len(raw_boards), interval=1000, blit=False)

    gif_path = "animation.gif"
    try:
        ani.save(gif_path, writer='pillow', fps=1)
        st.image(gif_path, caption="Tank Game Animation")
    except Exception as e:
        st.error(f"Failed to create animation: {e}")
