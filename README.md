# FiguraSDK 
FiguraSDK is a library that aims to simplify the process of creating OpenGL applications.
## Build
Building is pretty straightforward. You can simply build with cmake.
``` 
cd FiguraSDK 
cmake -B out -DCMAKE_BUILD_TYPE=Release
cmake --build out --config Release
```
## Usage
FiguraSDK depends on several libraries to function. These libraries must be linked with FiguraSDK.lib.
``` cmake
link_libraries(     
	"opengl32.lib"
	"FiguraSDK.lib"
	"assimp-vc143-mt.lib"
	"glfw3.lib"
)
```
### Example Code 
An example program can be found in `examples` folder.

<img width="797" height="824" alt="Ekran görüntüsü 2025-08-12 002956" src="https://github.com/user-attachments/assets/70139f86-bf64-4818-a687-a4c400d576f2" />

## Licenses
License files of used libraries can be found in `LICENSES` folder.
