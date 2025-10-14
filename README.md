## **About FiguraSDK**

*A cross-platform SDK made to simplify the process of rendering 3D objects.* 

**Example Program** 

<img width="364" height="585" alt="Ekran görüntüsü 2025-10-14 111114" src="https://github.com/user-attachments/assets/73e80a91-58c9-49b5-be33-81adeeb1e968" />
<img width="412" height="517" alt="Ekran görüntüsü 2025-10-14 110501" src="https://github.com/user-attachments/assets/6ea8b479-76ae-438e-acbe-dbc998c5dc60" /> 

<sub> *Model created by [Meshy AI](https://www.meshy.ai)* </sub> 

## Build

Simply build with cmake
``` shell
cd FiguraSDK 
cmake -B out -DCMAKE_BUILD_TYPE=Release
cmake --build out --config Release
```
## Link
**FiguraSDK** <ins>must</ins> be linked with the libraries below when used.
> [!NOTE]
> The precompiled libraries for **Linux** is not included in this repo. You have to download / compile them yourself if you intent on using ***FiguraSDK*** on **Linux**.

**Example link**
``` cmake
link_libraries(     
	"opengl32.lib"
	"FiguraSDK.lib"
	"assimp-vc143-mt.lib"
)
```
> [!WARNING]
> ***FiguraSDK*** does not provide any interface for creating window and **OpenGL** context. You must provide these requirements before using ***FiguraSDK***. An example program using **GLFW** can be found at **examples** folder
## Licenses
See ***Additional Licenses*** folder.
