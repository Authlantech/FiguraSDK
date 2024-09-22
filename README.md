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
* **test.exe** will be placed in `test/out` folder

### Link
When used in a project **FiguraSDK.lib** and **assimp-vc143-mt.lib** must be linked together. They are located at `lib` folder
``` cmake
target_link_libraries(YOUR_TARGET_NAME PUBLIC FiguraSDK.lib assimp-vc143-mt.lib)
```

# Licenses 
Licenses of used libs can be found at locations given below
* Assimp `include/assimp/LICENSE.txt`
* GLM `include/glm/copying.txt`
* stb_image.h `include/stb_image.h`
* glad  `include/KHR/khrplatform.h`     
