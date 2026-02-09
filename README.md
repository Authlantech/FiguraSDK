# FiguraSDK

*A cross-platform C++ SDK for simplified 3D rendering with OpenGL.*

## Features

- **Easy Model Loading** - Load OBJ models with textures using Assimp
- **Shader Management** - Load GLSL shaders from files
- **Camera System** - Perspective and orthographic cameras with built-in input controls
- **Async Loading** - Non-blocking model loading for responsive applications
- **Render Queue** - Flexible rendering pipeline with per-object shader support

## Planned Features

- [ ] **Post-Processing Support** - Add support for post-processing effects (bloom, HDR, etc.)
- [ ] **PBR (Physically Based Rendering)** - Implement PBR material system for realistic lighting
- [x] **Transparent Texture Rendering** - Support for rendering transparent textures with proper blending
- [ ] **Shadow Mapping** - Add dynamic shadow rendering capabilities
- [ ] **Skybox Support** - Add cubemap-based skybox rendering for environment backgrounds

## Example Program

<img width="364" height="585" alt="Ekran görüntüsü 2025-10-14 111114" src="https://github.com/user-attachments/assets/73e80a91-58c9-49b5-be33-81adeeb1e968" />
<img width="412" height="517" alt="Ekran görüntüsü 2025-10-14 110501" src="https://github.com/user-attachments/assets/6ea8b479-76ae-438e-acbe-dbc998c5dc60" /> 

<sub> *Model created by [Meshy AI](https://www.meshy.ai)* </sub> 

## Quick Start

```cpp
#include <Figura/GraphicsEngine.h>

int main() {
    // Create window
    fgr::WindowProperties props;
    props.width = 800;
    props.height = 600;
    props.tittle = "FiguraSDK Example";
    props.frames_per_second = 60;

    fgr::GraphicsEngine engine(props);

    // Load shader
    auto shader = std::make_shared<fgr::Shader>();
    shader->Load("shaders/default/default.vert", "shaders/default/default.frag");
    engine.ConfigureDefaultShader(shader);

    // Setup camera
    fgr::PerspectiveAttribs attribs = { 60.f, 800.f / 600.f, 0.1f, 100.f };
    auto camera = std::make_shared<fgr::PerspectiveCamera>(attribs);
    camera->set_position(glm::vec3(0.f, 2.f, 10.f));
    engine.ConfigureCamera(camera);

    // Load model
    auto model = std::make_shared<fgr::Model>();
    model->Load("assets/model.obj");

    // Render loop
    while (engine.IsWindowOpen()) {
        engine.GetCameraMovement();

        fgr::RenderItem item;
        item.model = model;
        engine.AppendRenderQueue(item);

        engine.Render();
        engine.UpdateWindow();
    }

    return 0;
}
```


## Build

Build with CMake:
```shell
cd FiguraSDK 
cmake -B out -DCMAKE_BUILD_TYPE=Release
cmake --build out --config Release
```

## Linking

**FiguraSDK** <ins>must</ins> be linked with the following libraries:

```cmake
link_libraries(     
    "opengl32.lib"
    "FiguraSDK.lib"
    "assimp-vc143-mt.lib"
)
```

> [!NOTE]
> Precompiled libraries for **Linux** are not included. You must download or compile them yourself for Linux builds.

## Project Structure

```
FiguraSDK/
├── include/Figura/    # Header files
├── source/            # Implementation files
├── examples/          # Example applications
│   └── shaders/       # GLSL shader files
├── lib/               # Library build output
└── Additional Licenses/
```

## API Reference

### GraphicsEngine

The main engine class that manages the rendering pipeline, window, and resources.

#### Constructor
- `GraphicsEngine(WindowProperties properties)` - Initialize the graphics engine with window properties

#### Configuration Methods
- `void ConfigureDefaultShader(std::shared_ptr<Shader> shader)` - Set the default shader used for rendering when no custom shader is specified
- `void ConfigureCamera(std::shared_ptr<Camera> camera)` - Set the active camera for the scene

#### Input Handling
- `void GetCameraMovement()` - Process keyboard and mouse input for camera movement (WASD, Space, Ctrl, Right Mouse)

#### Rendering
- `void AppendRenderQueue(RenderItem item)` - Add a render item to the queue for the current frame
- `void ClearRenderQueue()` - Clear all items from the render queue
- `void Render()` - Render all queued items to the screen

#### Window Management
- `bool IsWindowOpen()` - Check if the window is still open
- `void UpdateWindow(glm::vec4 surface_color = glm::vec4(0.f, 0.f, 0.f, 1.f))` - Swap buffers and poll events (call once per frame)
  - `surface_color` - Optional RGBA clear color for the background (default: black)

---

### Camera (Abstract Base)

Abstract base class for cameras. Use `PerspectiveCamera` or `OrthoCamera` to create instances.

#### Transform Methods
- `void set_position(glm::vec3 pos)` - Set camera position in world space
- `void face(glm::vec3 target)` - Point the camera at a target position

#### Query Methods
- `glm::vec3 get_position()` - Get current camera position
- `glm::vec3 get_oreintation()` - Get camera orientation vector

---

### PerspectiveCamera

Perspective projection camera. Inherits from `Camera`.

#### Constructor
- `PerspectiveCamera(PerspectiveAttribs attribs)` - Create with perspective projection settings

```cpp
struct PerspectiveAttribs {
    float fov;     // Field of view in degrees
    float aspect;  // Aspect ratio (width/height)
    float near;    // Near clipping plane
    float far;     // Far clipping plane
};
```

---

### OrthoCamera

Orthographic projection camera. Inherits from `Camera`.

#### Constructor
- `OrthoCamera(OrthographicAttribs attribs)` - Create with orthographic projection settings

```cpp
struct OrthographicAttribs {
    float left, right;   // Horizontal bounds
    float bottom, top;   // Vertical bounds
    float near, far;     // Near and far clipping planes
};
```

---

### Model

Represents a 3D model composed of one or more meshes.

#### Loading Methods
- `void Load(std::string file)` - Load a 3D model synchronously from file (blocks until complete)
- `void LoadAsync(std::string file)` - Load a 3D model asynchronously (non-blocking, loads in background)
- `static model_data LoadModelData(std::string file)` - Load model data without creating GPU resources (for custom loading)

#### Transform Methods
- `void set_position(glm::vec3 position)` - Set model position in world space
- `void scale(float v)` - Scale the model uniformly by factor `v`
- `void rotate(glm::vec3 v, float angle)` - Rotate model around axis `v` by `angle` radians

#### Query Methods
- `glm::vec3 get_position()` - Get current model position

---

### Model2D

A specialized model class for rendering 2D textured planes (sprites). Inherits from `Model`.

#### Constructor
- `Model2D(const char* texture_path)` - Create a 2D plane with the specified texture
  - `texture_path` - Path to the texture image file

All transform methods from `Model` are available (`set_position`, `scale`, `rotate`, etc.).

---

### Shader

Manages GLSL shader programs.

#### Loading
- `void Load(const char* vertex_shader_file, const char* fragment_shader_file, const char* geometry_shader_file = nullptr)` - Load and compile shaders from files
  - `vertex_shader_file` - Path to vertex shader file
  - `fragment_shader_file` - Path to fragment shader file
  - `geometry_shader_file` - Optional path to geometry shader file

> [!NOTE]
> Shaders must follow the uniform and attribute naming conventions documented in the [Shader Requirements](#shader-requirements) section.

---

### Data Structures

#### WindowProperties
```cpp
struct WindowProperties {
    int width;                          // Window width in pixels
    int height;                         // Window height in pixels
    const char* tittle;                 // Window title
    int frames_per_second;              // Target FPS (default: 30)
    GLint OpenGLContextVersionMajor;    // OpenGL major version (default: 4)
    GLint OpenGLContextVersionMinor;    // OpenGL minor version (default: 6)
};
```

#### RenderItem
```cpp
struct RenderItem {
    std::shared_ptr<Shader> shader;     // Custom shader (nullptr uses default)
    std::shared_ptr<Model> model;       // Model to render
};
```

#### Vertex
```cpp
struct Vertex {
    float position[3];           // Vertex position (x, y, z)
    float color[4];              // Vertex color (r, g, b, a)
    float texture_coordinates[3]; // Texture coordinates (u, v, w)
    float normal[3];             // Normal vector (x, y, z)
    float tangent[3];            // Tangent vector (x, y, z)
    float bittangent[3];         // Bitangent vector (x, y, z)
};
```

### Shader Requirements

When creating custom shaders for FiguraSDK, your shaders must adhere to specific uniform names, texture bindings, and vertex attribute locations that the engine expects. This ensures proper communication between the engine and your shader programs.

#### Required Uniforms

| Uniform Name | Type | Description |
|--------------|------|-------------|
| `modelMatrix` | `mat4` | Model transformation matrix |
| `viewMatrix` | `mat4` | View (camera) matrix |
| `projectionMatrix` | `mat4` | Projection matrix |
| `normalMatrix` | `mat4` | Normal transformation matrix (for lighting calculations) |

#### Texture Bindings

| Binding | Uniform Name | Description |
|---------|--------------|-------------|
| `0` | `albedo_map` | Albedo/diffuse color texture |
| `1` | `normal_map` | Normal map for bump mapping |
| `2` | `metallic_map` | Metallic texture (PBR) |
| `2` | `roughness_map` | Roughness texture (PBR) |
| `2` | `ao_map` | Ambient occlusion map |

> [!NOTE]
> Multiple textures can share the same binding point if they are not used simultaneously in the shader.

#### Vertex Attributes

| Location | Attribute Name | Type | Description |
|----------|----------------|------|-------------|
| `0` | `local_space_ver_pos` | `vec3` | Vertex position in local space |
| `2` | `ver_texture_coords` | `vec2` | Texture coordinates |
| `3` | `local_space_ver_normal` | `vec3` | Vertex normal in local space |
| `4` | `local_space_ver_tangent` | `vec3` | Vertex tangent in local space |
| `5` | `local_space_ver_bittangent` | `vec3` | Vertex bitangent in local space |

#### Default Vertex Shader

See [default.vert](shaders/default/default.vert) for the complete default vertex shader:

```glsl
#version 460 core

layout (location = 0) in vec3 local_space_ver_pos;
layout (location = 2) in vec2 ver_texture_coords;
layout (location = 3) in vec3 local_space_ver_normal;
layout (location = 4) in vec3 local_space_ver_tangent;
layout (location = 5) in vec3 local_space_ver_bittangent;

out VERTEX_DATA
{
	vec3 world_space_frag_pos;
	vec3 world_space_ver_normal;
	vec3 world_space_ver_tangent;
	vec3 world_space_ver_bittangent;
	vec2 ver_texture_coordinates;
} ver_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(local_space_ver_pos,1.0);
	ver_out.ver_texture_coordinates = ver_texture_coords;
	ver_out.world_space_ver_normal = mat3(normalMatrix) * local_space_ver_normal;
	ver_out.world_space_ver_tangent = mat3(normalMatrix) * local_space_ver_tangent;
	ver_out.world_space_ver_bittangent = mat3(normalMatrix) * local_space_ver_bittangent;
	ver_out.world_space_frag_pos = vec3(modelMatrix * vec4(local_space_ver_pos,1.0));
}
```

#### Default Fragment Shader

See [default.frag](shaders/default/default.frag) for the complete default fragment shader:

```glsl
#version 460 core

layout (binding = 0) uniform sampler2D albedo_map;
layout (binding = 1) uniform sampler2D normal_map;
layout (binding = 2) uniform sampler2D metallic_map; 
layout (binding = 2) uniform sampler2D roughness_map; 
layout (binding = 2) uniform sampler2D ao_map; 

in VERTEX_DATA
{
	vec3 world_space_frag_pos;
	vec3 world_space_ver_normal;
	vec3 world_space_ver_tangent;
	vec3 world_space_ver_bittangent;
	vec2 ver_texture_coordinates;
} frag_in;

out vec4 frag_color;

void main() {
	frag_color = vec4(texture(albedo_map, frag_in.ver_texture_coordinates).rgb, 1.0f);
}
```

## Licenses

See **Additional Licenses** folder for third-party library licenses.