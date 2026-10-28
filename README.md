# game-number-one

A small grid puzzle game (Sokoban family), written in C++20 with SDL3, plus a small
engine that grows with it.

Right now it's a two-player, turn-based duel on one keyboard (a prototype to test the
idea). Players take turns making one move each: a step, which may push a block, or a
pass. Pushing a block makes it yours, even if your opponent owned it; blocks start out
neutral. Wherever the pushed block lands, any of your opponent's blocks it touches
side by side (not diagonally) become yours too. Each owned block controls the floor cells among its 8 neighbours, and a cell
near both players' blocks goes to whoever has more blocks next to it (nobody on a
tie). After 50 moves (25 each), whoever controls more floor cells wins.

Owned blocks and the floor they control are tinted in their owner's colour; the
pips above the board show the moves left, then the winner's colour (grey for a
draw), and the bar below it shows the current score as a tug of war (yellow from
the left, pink from the right).
The final scores are also printed to the console in debug builds.

The one-player puzzle room from milestone 2 is still there: run `game.exe --puzzle`.

## Controls

| Key        | Action |
|------------|--------|
| WASD       | Player 1 (yellow) moves |
| Arrow keys | Player 2 (pink) moves |
| Space      | Pass the turn |
| Tab        | Switch player 2 between AI (default) and human |
| R          | Restart the round |
| Esc        | Quit   |

In `--puzzle` mode, arrow keys or WASD move the single player.

## Build and run in Visual Studio

Requires Visual Studio with the **Desktop development with C++** workload (includes
MSVC, CMake and Ninja). SDL3 and doctest are downloaded automatically on the first
configure, so you need an internet connection the first time.

1. **File → Open → Folder…** and pick this folder.
2. In the toolbar, choose the **debug** preset (or **release**). Visual Studio
   configures the project; watch the Output window until it finishes.
3. In the startup item dropdown (the green ▶ button), choose **game.exe**.
4. Press **F5** to build and run with the debugger, or **Ctrl+F5** without it.

To run the tests: **Test → Test Explorer → Run All**, or build and run `tests.exe`.

## Build from the command line

From a *Developer PowerShell for VS*:

```
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
out\build\debug\bin\game.exe
```

## Project layout

- `engine/`: reusable engine library (window, game loop, input, drawing). Knows nothing about the game.
- `game/`: the game, built on the engine.
- `tests/`: unit tests (doctest + CTest).

See [CLAUDE.md](CLAUDE.md) for the design rules and conventions.
