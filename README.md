# FiguraSDK

*A cross-platform C++ SDK for simplified 3D rendering with OpenGL.*

## Features

- **Easy Model Loading** - Load OBJ models with textures using Assimp
- **Shader Management** - Load GLSL shaders from files
- **Camera System** - Perspective and orthographic cameras with built-in input controls
- **Async Loading** - Non-blocking model loading for responsive applications
- **Render Queue** - Flexible rendering pipeline with per-object shader support

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
    shader->LoadFromFile("shaders/model.vert", "shaders/model.frag");
    engine.ConfigureDefaultShader(shader);

    // Setup camera
    auto camera = std::make_shared<fgr::Camera>();
    camera->configure_perspective(60.f, 800.f / 600.f, 0.1f, 100.f);
    camera->set_position(glm::vec3(0.f, 2.f, 10.f));
    engine.ConfigureCamera(camera);

    // Load model
    auto model = engine.LoadModel("assets/model.obj");

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

## Camera Controls

| Key | Action |
|-----|--------|
| `W` | Move forward |
| `S` | Move backward |
| `A` | Strafe left |
| `D` | Strafe right |
| `Space` | Move up |
| `Left Ctrl` | Move down |
| `Right Mouse + Move` | Look around |

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
- `ConfigureDefaultShader(shader)` - Set the default shader for rendering
- `ConfigureCamera(camera)` - Set the active camera
- `GetCameraMovement()` - Process keyboard/mouse input for camera
- `LoadModel(file)` - Load a model synchronously
- `LoadModelAsync(file)` - Load a model asynchronously
- `AppendRenderQueue(item)` - Add item to render queue
- `Render()` - Render all queued items
- `UpdateWindow()` - Swap buffers and poll events

### Shader
- `LoadFromFile(vertex, fragment, geometry)` - Load shaders from GLSL files

### Camera
- `configure_perspective(fov, aspect, near, far)` - Set perspective projection
- `configure_ortho(left, right, bottom, top, near, far)` - Set orthographic projection
- `set_position(pos)` - Set camera position
- `face(target)` - Point camera at target

### Model
- `set_position(pos)` - Set model position
- `rotate(axis, angle)` - Rotate model around axis

## Licenses

See **Additional Licenses** folder for third-party library licenses.
