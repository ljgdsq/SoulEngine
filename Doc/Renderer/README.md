These files are reference implementations for the new cross-API renderer abstraction.

Purpose
- Provide a reviewable, minimal set of headers and skeletons under Doc/Renderer.
- You can later move them into Engine/Renderer/Gfx and Engine/Renderer/OpenGL.

Contents
- Gfx.h: Backend-agnostic interfaces and enums (IBuffer, IVertexInputLayout, IDevice, IContext, descriptors).
- OpenGL/GLBackendSkeleton.cpp: Notes and class signatures for a minimal OpenGL backend.

Notes
- No build integration here; these are for design review and copy-over.
- Naming and exact API can be adjusted to match project conventions.
