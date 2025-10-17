# Ray Tracing in One Weekend Series

A C++ implementation of Peter Shirley’s *Ray Tracing in One Weekend* series.  
This project explores the fundamentals of ray tracing — generating images through light simulation, material interaction, and recursive rendering techniques.  

It forms part of a broader learning path toward real-time rendering, with the long-term goal of integrating ray tracing into a custom **DirectX 12** engine for showcasing advanced graphics concepts.

---

## 🎥 Preview

![Preview](https://raw.githubusercontent.com/MoAgilah/Ray-Tracing-in-One-Weekend-series/master/10000Samples(13289.3s).bmp)

---

## 🎯 Purpose & Vision

- Understand the **mathematical and conceptual foundations** of ray tracing.  
- Implement features such as diffuse and metallic reflections, refraction, and anti-aliasing.  
- Build from a CPU-based path tracer to a **GPU-accelerated renderer** in the future.  
- Eventually integrate ray tracing techniques into a **DirectX 12 real-time rendering engine**, alongside projects like *Hierarchical Animation* and *Real-Time Collision Handling*.

---

## 📁 Project Structure

```
RayTracing/        → Source code implementing the book’s concepts
Assets/            → Output renders and sample images
Docs/              → Notes and study references
```
*(Folder names may vary slightly depending on iteration.)*

---

## ⚙️ Features (Current & Planned)

### ✅ Implemented
- Vector math and ray classes  
- Scene representation and hittable objects  
- Lambertian (diffuse), metal, and dielectric materials  
- Recursive ray-color calculation  
- Anti-aliasing and gamma correction  
- Simple multithreading for faster rendering  

### 🚧 Planned / Future
- Real-time GPU implementation using **DirectX 12**  
- Dynamic camera and scene manipulation  
- Texture mapping and procedural materials  
- Depth of field and motion blur  
- BVH (Bounding Volume Hierarchy) acceleration structures  
- Integration into the standalone DX12 engine for real-time visualization  

---

## 🧱 Building & Running

**Requirements**
- Visual Studio 2019 / 2022  
- C++17 (or later)  
- Windows 10/11 SDK  

**Steps**
1. Open the solution or project file.  
2. Set build configuration to **x64 → Release** for faster CPU rendering.  
3. Run the program — rendered image files will be generated in the output directory.  

*(Render times depend on resolution and sample count.)*

---

## 🧩 Future Integration Plan

Once the standalone **DirectX 12 engine** (from *Introduction to Game Programming with DirectX 12*) is complete:
1. Integrate ray-tracing logic via compute shaders or DXR (DirectX Raytracing).  
2. Enable **real-time hybrid rendering** (rasterization + ray tracing).  
3. Use this to demonstrate learning progress in **GPU-based rendering**, **lighting**, and **real-time global illumination**.

---

## 📚 References

- *Ray Tracing in One Weekend* by Peter Shirley ([series link](https://raytracing.github.io/))  
- *Introduction to 3D Game Programming with DirectX 12* by Frank Luna  
- Microsoft DirectX Raytracing (DXR) documentation — [learn.microsoft.com/directx](https://learn.microsoft.com/en-us/windows/win32/direct3d12/directx-12-programming-guide)  

---

## 🙏 Acknowledgements

- Inspired by Peter Shirley’s seminal educational series.  
- Built as part of a continuous learning roadmap leading toward a **DirectX 12-based rendering engine**.  
- Serves as both a study tool and a future module for real-time ray-tracing integration.

---

## 👤 Author
**Mohamed Agilah**  
🎓 Games Programmer & AI Developer  
🌐 [Portfolio Website](https://moagilah.com/)  
📧 Contact: agilahmohamed@gmail.com  

---
