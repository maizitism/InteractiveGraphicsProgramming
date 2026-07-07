# Assignment 2 — Transformations

Loads a 3D model from an `.obj` file, uploads its vertices to the GPU, and draws
them as a point cloud that you can orbit, zoom, and inspect. Vertices are shaded
by their distance from the camera (near = bright, far = dark).

Implements: vertex buffers (VAO/VBO), GLSL shaders, model-view-projection
transforms, perspective projection, bounding-box centering, an interactive
camera, and a depth-based color ramp.

## You must supply your own `.obj` file

The program takes the model path as its **first command-line argument** — it
does not ship with one baked in:

```sh
Assignment02.exe <model.obj>
```

A `teapot.obj` is included in this folder to get you started. To use a different
model, drop the `.obj` next to the executable's working directory (see below)
and pass its filename.

> **Tips for other models**
> - Only Wavefront **`.obj`** is supported (parsed by `cyTriMesh`). Convert
>   `.fbx`/`.glb`/`.stl` in Blender first (`File → Export → Wavefront .obj`).
> - The model is auto-centered via its bounding box, but **scale** is up to the
>   file. If it loads blank, it's almost certainly too big/small for the camera
>   distance range — widen `minCameraDistance` / `maxCameraDistance` in
>   `main.cpp`.
> - It renders as **points**, not a solid surface.

## Running it

### From VS Code
Pick a launch configuration in the Run & Debug panel and press play. The
included configs are in the repo's `.vscode/launch.json`:

- **Debug Assignment02 (teapot)** — runs with `teapot.obj`
- **Debug Assignment02 (nautilus)** — runs with `nautilus.obj`

### Point a launch config at your own model

Open `.vscode/launch.json` and edit (or copy) the Assignment02 entry:

```jsonc
{
  "name": "Debug Assignment02 (my model)",
  "type": "cppvsdbg",
  "request": "launch",
  "program": "${workspaceFolder}/Assignments/out/build/x64-debug/Assignment02/Assignment02.exe",
  "args": ["my_model.obj"],                       // <-- your file name
  "cwd": "${workspaceFolder}/Assignments/Assignment02", // <-- your file must live here
  "console": "integratedTerminal",
  "preLaunchTask": "CMake: build"
}
```

Two fields matter:
- **`args`** — the filename passed as `argv[1]`.
- **`cwd`** — the working directory the program runs in. Your `.obj` must be in
  this folder (or give a path in `args` relative to it).

### From a terminal

```sh
cd Assignments/Assignment02
../out/build/x64-debug/Assignment02/Assignment02.exe teapot.obj
```

## Camera controls

| Input | Action |
|-------|--------|
| **Left mouse + drag** | Orbit the model — horizontal = yaw, vertical = roll |
| **Middle mouse + drag** | Pitch (tilt) — horizontal drag |
| **Right mouse + drag** | Zoom — vertical drag changes camera distance |
| **F6** | Recompile the GLSL shaders from disk |
| **Esc** | Quit |

Zoom is clamped to a sensible range so the model can't pass through the camera.

## How it works (quick tour)

- **`main.cpp`** — window/GLEW setup, loads the mesh, builds the VAO/VBO, and
  computes the model-view-projection matrices each frame from the accumulated
  camera angles and distance.
- **`shader.vert`** — transforms each vertex by `mvp`, and also computes its
  camera-space depth (`-viewPos.z` from a separate `mv` matrix) to pass down.
- **`shader.frag`** — maps that depth into a grayscale ramp using `nearRange` /
  `farRange` uniforms that track the model's bounding box.

## Building

See the [root README](../../README.md#building). In short: CMake `x64-debug`
preset, dependencies (`freeglut`, `glew`) auto-installed by vcpkg.
