# FiguraSDK 
A library aims to simplify the process of creating 3D graphics

## Build
Building is pretty straightforward. You can simply build with cmake.
``` 
cd FiguraSDK 
cmake -B out -DCMAKE_BUILD_TYPE=Release
cmake --build out --config Release
```
## Usage
FiguraSDK requires `opengl32.lib` and `assimp-vc143-mt.lib` to function.  
``` cmake
link_libraries(     
	"opengl32.lib"
	"FiguraSDK.lib"
	"assimp-vc143-mt.lib"
)
```
### Example Code 
An example program can be found in `examples` folder.
## Licenses
Licenses of used libs can be found at locations given below
* Assimp `include/assimp/LICENSE.txt`
* GLM `include/glm/copying.txt`
* stb_image.h `include/stb_image.h`
* glad  `include/KHR/khrplatform.h`    
* GLFW `examples/deps/GLFW/LICENSE.md`
