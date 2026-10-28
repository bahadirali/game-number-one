# game-number-one

A small grid puzzle game (Sokoban family), written in C++20 with SDL3, plus a small
engine that grows with it.

Right now (milestone 2) it's a walled room where the yellow player pushes blue blocks
around. A block moves only into an empty floor cell; you can't push two blocks at once.

## Controls

| Key                 | Action |
|---------------------|--------|
| Arrow keys / WASD   | Move (hold to keep moving) |
| Esc                 | Quit   |

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
- `game/`: the puzzle game, built on the engine.
- `tests/`: unit tests (doctest + CTest).

See [CLAUDE.md](CLAUDE.md) for the design rules and conventions.
