# Ray Tracing in One Weekend Series

A C++ implementation inspired by Peter Shirley’s *Ray Tracing in One Weekend* series, focused on learning the foundations of ray tracing, physically based rendering, and recursive light simulation.

The project explores ray-object interaction, shading, reflections, refraction, and image generation through CPU-based rendering techniques, forming part of a broader learning path toward modern real-time rendering and DirectX 12 graphics programming.

---

## 🎥 Preview

<p align="center">
  <img src="https://raw.githubusercontent.com/MoAgilah/Ray-Tracing-in-One-Weekend-series/master/10000Samples(13289.3s).bmp" width="700">
</p>

---

## 🧠 Overview

This project implements core ray tracing concepts including recursive ray generation, material interaction, and lighting simulation using C++.

The repository focuses on understanding how physically based rendering systems operate at a low level before transitioning toward GPU-accelerated and real-time rendering workflows.

---

## ⚙️ Key Features

### Implemented

- Ray generation and tracing systems
- Camera and viewport implementation
- Sphere and object intersection logic
- Lambertian, metallic, and dielectric material models
- Recursive ray bounce handling
- Reflection and refraction calculations
- Anti-aliasing and gamma correction
- Scene rendering and image output
- Vector mathematics utilities
- Simple multithreaded rendering support

### Planned

- Bounding Volume Hierarchy acceleration structures
- Texture mapping and procedural materials
- Depth of field and motion blur
- GPU-based rendering experiments
- DirectX 12 and DXR integration
- Hybrid rasterization and ray tracing workflows

---

## 🧱 Architecture

The project is structured around reusable rendering concepts and mathematical abstractions used in ray tracing systems.

```text
RayTracing/
├── Source/        → Ray tracing implementation and rendering logic
├── Assets/        → Render outputs and sample images
├── Docs/          → Notes and study references
└── README.md
```

The implementation focuses on separating scene representation, ray generation, material behaviour, and rendering workflows into modular systems for experimentation and future extension.

---

## 🛠 Technologies

- C++
- Visual Studio
- Windows SDK
- Ray tracing algorithms
- Recursive rendering techniques
- Mathematical modelling
- Physically based rendering concepts

---

## 🚀 Build & Run

### Requirements

- Visual Studio 2019 or 2022
- C++17 or later
- Windows 10/11 SDK

### Steps

1. Clone the repository.
2. Open the solution or project file.
3. Select **x64 → Release** for improved rendering performance.
4. Build and run the project.

Rendered images will be generated in the output directory.

---

## 🔭 Future Work

- Integrate GPU-based rendering workflows
- Experiment with DirectX Raytracing (DXR)
- Add acceleration structures for performance optimisation
- Expand scene complexity and material systems
- Explore hybrid rasterization and ray tracing approaches
- Integrate rendering experiments into a broader DirectX 12 engine architecture

---

## 📚 References

- *Ray Tracing in One Weekend* — Peter Shirley
- *Introduction to 3D Game Programming with DirectX 12* — Frank Luna
- Microsoft DirectX Raytracing (DXR) documentation

---
