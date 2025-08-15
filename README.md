# Voxel Engine

A voxel engine built from the ground up using C++ and OpenGL. 

## Features
- World
  - 32x32x32 chunks
  - Infinite, procedurally generated terrain
  - Breaking and placing blocks
  - Biomes: Ocean, Beach, Forest, Mountains, Snowy Mountains, Plains, Desert
  - Blocks: Grass, Dirt, Stone, Log, Leaves, Sand, Water, Snow
  - Terrain features: Trees
- Lighting
  - Baked ambient occlusion
  - Phong lighting
  - Cascaded shadow mapping
  - Transparency support
- Optimizations
  - Deferred rendering
  - Face culling
  - Backface culling
  - Frustum culling
  - Compressed vertex layout (8 bytes each)
- Entity Component System
  - Currently barebones, but strong foundation laid for future entities to be added
- UI Overlay
  - Minimal configurable settings using ImGui 

## Screenshots 
![Screenshot 1](Screenshots/Screenshot1.png)
![Screenshot 2](Screenshots/Screenshot2.png)
![Screenshot 3](Screenshots/Screenshot3.png)

## Controls
- WASD: Move
- Space: Jump/ascend
- Ctrl: Crouch/descend
- Left click: Break block
- Right click: Place block
- Shift: Sprint
- Esc: Toggle debug overlay

## Building

### Prerequisites

You will need CMake, a C++20 compatible compiler, and a system that supports the OpenGL 3.3 core profile.
The application has been tested on Windows with MSVC and GCC. With Clang, the logging system is currently broken. A fix is on the way for that.

### Setup

First, clone the repository:
```
git clone --recursive https://github.com/shanebarrios/Voxels.git
cd Voxels
```
Make sure you clone recursively, as there are submodules within the project.
Then run
```
cmake -Bbuild -S. -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
It is HIGHLY recommended to build in release mode, as the debug mode is (quite literally) unplayable. 
