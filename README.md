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

## Licenses
### Used programs
License files of used libraries can be found in `LICENSES` folder.
### Used assets
* “Dirty stones pile” by matousekfoto, available at [Sketchfab](https://sketchfab.com/3d-models/dirty-stones-pile-893e64e62f414b439672a8770dd2869d), Licensed under [CC BY 4.0.](https://creativecommons.org/licenses/by/4.0/legalcode.en).
