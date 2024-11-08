# 3DCGA Final Project

## 1. Implemented Features

### 1.1. Minimal Features

- **Multiple Viewpoints**
- **Cube Mapping (Scene Loading)**
  - Implemented WASDRF movement in Camera class
- **Normal Mapping**
- **Environment Mapping**
- **Shading**:
  - Simple shading: Lambert + Blinn-Phong
  - Advanced shading: Textured PBR
  - Both include normal mapping
- **Material Textures**:
  - `kd`, `ks`, and shininess for simple shading
  - Albedo, roughness, metallic, and ambient occlusion for PBR
- **Hierarchical Transformations**:
  - Animated robot arm with several components (animation is code-driven, not imported from external software)
- **Smooth Paths (basic Bézier curves)**

### 1.2. Extra Features (Optional)

- **Bézier Curves (extra part, moving at constant speed along a Bézier curve)**
- **Image-Based Lighting**
- **Particle Effects (explosions, magic spells, fire)**

## 2. References

- [LearnOpenGL - Cubemaps](https://learnopengl.com/Advanced-OpenGL/Cubemaps)
- [LearnOpenGL - Diffuse irradiance](https://learnopengl.com/PBR/IBL/Diffuse-irradiance)
- [LearnOpenGL - Normal Mapping](https://learnopengl.com/Advanced-Lighting/Normal-Mapping)
- [LearnOpenGL - Particles](https://learnopengl.com/In-Practice/2D-Game/Particles)
- [Medium - Bézier Curve](https://omaraflak.medium.com/b%C3%A9zier-curve-bfffdadea212)
