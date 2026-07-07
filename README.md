# Interactive Graphics Programming

My work-in-progress as I learn **modern OpenGL** by following Cem Yuksel's
*Interactive Computer Graphics* course (University of Utah, CS 5610 / 6610 — the
lectures are freely available on YouTube). Each assignment is a small,
self-contained C++ program that builds up from an empty window toward a real
rasterizer.

This repo is a learning journal as much as a codebase — expect the style to get
cleaner as the assignments progress.

## What's here

| Path | What it is |
|------|------------|
| `Assignments/Assignment01/` | First window + GLUT event loop (animated clear color) |
| `Assignments/Assignment02/` | **Transformations** — load an `.obj`, orbit camera, perspective, depth-based coloring. See its [README](Assignments/Assignment02/README.md). |
| `lib/` | [cyCodeBase](https://github.com/cemyuksel/cyCodeBase) headers by Cem Yuksel (`cyTriMesh`, `cyMatrix`, `cyVector`, `cyGL`) used for OBJ parsing, matrices, and GLSL handling |
| `Assignments/CMakeLists.txt` | Top-level build; add one `add_subdirectory` line per new assignment |

## Tech stack

- **C++20**
- **OpenGL 3.3 core** with GLSL shaders
- **freeglut** for windowing/input, **GLEW** for extension loading
- **CMake** (presets) + **vcpkg** (manifest mode) for the build and dependencies
- Developed in **VS Code** with the CMake Tools extension

## Building

Dependencies (`freeglut`, `glew`) are installed automatically by vcpkg from
`Assignments/vcpkg.json`. You need:

- CMake ≥ 3.21
- A C++20 compiler (MSVC on Windows) and Ninja
- vcpkg, with the `VCPKG_ROOT` environment variable pointing at it

### VS Code (recommended)

Open the folder, let CMake Tools pick the **`x64-debug`** preset, and build.
Debug/run configurations live in `.vscode/launch.json`.

### Command line

```sh
cd Assignments
cmake --preset x64-debug
cmake --build out/build/x64-debug
```

Binaries land in `Assignments/out/build/x64-debug/<AssignmentXX>/`.

> **Note:** `.vscode/settings.json` hardcodes a `VCPKG_ROOT` path that matches my
> machine. If your vcpkg lives elsewhere, adjust it (or just set the
> `VCPKG_ROOT` environment variable).

## License / credits

The `cy*` headers in `lib/` are © Cem Yuksel and distributed under his own
license — see [cyCodeBase](https://github.com/cemyuksel/cyCodeBase). Everything
else here is my coursework.
