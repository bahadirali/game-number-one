# CLAUDE.md

Guidance for Claude (and any other contributor) working in this repository.

## Goals

- Make a small, polished **grid puzzle game** in the Sokoban family.
- Grow a **small reusable engine** alongside it, shaped by what the game actually needs.
- Claude writes the code; Bahadir reviews it in Visual Studio Community. Keep the code
  readable for review: small files, clear names, comments that explain *why*.

## Stack

- **C++20**, **SDL3** (window, rendering, input), **CMake** with presets, **Ninja**.
- **doctest** for unit tests, run through **CTest**.
- Dependencies are fetched by CMake (`cmake/Dependencies.cmake`) from pinned release
  archives with SHA-256 hashes. To upgrade one, change the URL *and* the hash.
- Ships on **Windows only**, but stays **Mac-ready**: everything platform-specific goes
  through SDL3. No Windows-only APIs, headers (`<windows.h>`), or hard-coded paths in
  source code. Compiler- or platform-specific settings belong in CMake, behind
  `if(MSVC)` and similar checks.

## Layout

```
engine/            static library `engine`: knows nothing about the game
  include/engine/  public headers (no SDL types beyond forward declarations)
  src/             implementation (the only place that includes SDL headers)
game/src/          `game_logic` library (pure rules, no SDL, no engine)
                   + `game` executable (plugs the logic into the engine)
tests/             doctest tests (`tests` executable, registered with CTest)
cmake/             dependency and compiler-option helpers
```

## The engine/game separation rule

- `engine/` must **never** include a game header or mention game concepts
  (levels, players, boxes, tiles). Dependencies point one way only:
  `game → engine`, never `engine → game`.
- SDL is a **PRIVATE** dependency of `engine`. Game code talks to the engine API
  (`engine::Game`, `engine::Input`, `engine::Renderer`) and never includes SDL directly.
- Game rules live in `game_logic`, which depends on neither SDL nor the engine, so
  they can be unit-tested headlessly.
- **Add engine modules only when the game needs them.** No speculative systems (ECS,
  scripting, audio, asset pipelines, …) until a concrete game feature requires them.
  When something is needed, write it for the game first, then move the reusable part
  into `engine/`.

## Build, run, test

In Visual Studio: **File → Open → Folder…**, pick this folder, choose the `debug` or
`release` preset in the toolbar, select `game.exe` as the startup item and press F5.
Test Explorer lists the doctest cases.

From a **Developer PowerShell / Command Prompt for VS** (MSVC must be on `PATH`):

```
cmake --preset debug          # configure (first run downloads SDL3 and doctest)
cmake --build --preset debug  # build
ctest --preset debug          # run the tests
out\build\debug\bin\game.exe  # run the game
```

Swap `debug` for `release` for an optimized build. Build output goes to
`out/build/<preset>/` (git-ignored); executables land in its `bin/` folder.

- **debug**: AddressSanitizer on (`GAME_ENABLE_ASAN`), console window for logs.
- **release**: optimized, no console window.

## Conventions

- Namespaces: `engine::` for the engine, `game::` for the game.
- Naming: `PascalCase` types, `snake_case` functions and variables, `trailing_`
  underscore for private members, `kPascalCase` constants.
- Warnings are on at a high level (`/W4`, `-Wall -Wextra …`); keep builds warning-free.
- Every change to game rules comes with tests in `tests/`.
- Before committing: build the `debug` preset and make sure `ctest --preset debug` passes.

## Git workflow

- Work in small steps, one milestone or fix at a time.
- Commit directly on `main` and push to `origin main`
  (github.com/bahadirali/game-number-one), but **only after Bahadir approves**:
  make the change, build and test it, explain it, then wait for an explicit OK
  before committing or pushing.
- No branches or pull requests unless Bahadir asks for them.
