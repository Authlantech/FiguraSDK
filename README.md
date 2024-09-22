# Build and Link
### Build 
You can simply download and build with cmake
#### Example build
``` cmd
cd FiguraSDK
cmake -B "out"
cmake --build "out"
```
After build : 
* **FiguraSDK.lib** will be placed in `lib` folder
* **example.exe** will be placed in `examples/out` folder

### Link
While using in a project you should link Assimp and FiguraSDK together. 
``` cmake
target_link_libraries(YOUR_TARGET_NAME PUBLIC FiguraSDK.lib assimp-vc143-mt.lib)
```

# Licenses 
Licenses of used libs can be found at locations given below
* Assimp `include/assimp/LICENSE.txt`
* GLM `include/glm/copying.txt`
* stb_image.h `include/stb_image.h`
* glad  `include/KHR/khrplatform.h`    
* GLFW `examples/deps/GLFW/LICENSE.md`
