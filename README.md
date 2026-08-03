# RayTracingInOneWeekendSeries

A modern C++ rendering project that demonstrates physically based rendering, progressive ray tracing implementation and modular rendering architecture.

RayTracingInOneWeekendSeries is a C++ implementation of Peter Shirley's *Ray Tracing in One Weekend* book series, exploring the mathematical and software engineering principles behind physically based image generation. Rather than focusing solely on producing rendered images, the project develops reusable rendering systems for rays, cameras, materials, geometry, acceleration structures and light transport while maintaining a modular architecture that can evolve over time.

The project focuses on rendering engineering rather than engine development, demonstrating how physically based rendering algorithms can be organised into reusable systems while providing the research foundation for future integration into the DirectX 12 Engine.

---

## The Engineering Problem

Modern graphics applications frequently rely on rasterisation, making it difficult to understand the mathematical principles behind physically based rendering and global illumination. While educational ray tracing implementations demonstrate rendering algorithms, they often remain standalone examples without considering maintainable software architecture or future integration into larger rendering systems.

RayTracingInOneWeekendSeries addresses this challenge by progressively implementing the techniques presented throughout the book series while organising the renderer into modular components. Geometry, materials, textures, cameras and rendering algorithms remain separated into focused systems, allowing the renderer to evolve naturally towards future integration within the DirectX 12 Engine.

---

## Framework Architecture

<p align="center">
  <img width="500" alt="RayTracing" src="https://github.com/user-attachments/assets/65fa36b6-b894-4e55-9917-c711aab121b4" />
</p>

The renderer is organised into layered rendering systems that progressively build a complete physically based rendering pipeline. High-level application components configure scenes and rendering parameters, while specialised subsystems manage ray generation, scene traversal, material evaluation, recursive light transport and image output. The modular organisation allows individual rendering techniques to evolve independently while supporting future integration into the DirectX 12 Engine architecture.

---

## Architecture Goals

The project was designed to demonstrate:

- Progressive renderer development
- Physically based rendering
- Recursive ray tracing
- Modular rendering architecture
- Computational geometry
- Separation of rendering systems
- Future engine integration

Although demonstrated through offline ray tracing, these engineering principles are directly transferable to real-time rendering engines, simulation software, graphics research and modern rendering pipelines.

---

## Key Features

- Progressive implementation of the Ray Tracing in One Weekend series
- Physically based material and light transport systems
- Modular scene, geometry and rendering architecture
- Recursive ray evaluation and global illumination
- Acceleration structures for scalable rendering
- Designed for future integration into the DirectX 12 Engine

---

## Technologies & Engineering Practices

### Development

- C++20
- Visual Studio 2022
- Git
- Git Submodules

### Engineering Practices

- Physically Based Rendering
- Recursive Ray Tracing
- Monte Carlo Sampling
- Computational Geometry
- Bounding Volume Hierarchies
- Modular Architecture
- Separation of Concerns
- Progressive System Design
- Object-Oriented Design

---

## Architecture Highlights

- Modular rendering pipeline organisation
- Reusable mathematical and geometry systems
- Separated material, texture and light transport systems
- Progressive implementation of advanced rendering techniques
- Designed for future DirectX 12 Engine integration

---

## Engineering Outcome

RayTracingInOneWeekendSeries demonstrates how physically based rendering algorithms can be implemented through a clean, modular C++ architecture rather than as isolated rendering demonstrations.

By separating mathematical foundations, geometry, materials, sampling and rendering systems, the project creates a maintainable renderer that not only reproduces the techniques presented throughout the *Ray Tracing in One Weekend* series but also establishes the rendering foundation for future integration into the DirectX 12 Engine. This approach enables physically based ray tracing and real-time rasterisation to eventually coexist within a shared engine architecture.

---
