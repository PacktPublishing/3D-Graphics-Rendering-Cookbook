# 3D Graphics Rendering Cookbook
A comprehensive guide to exploring rendering algorithms in modern OpenGL and Vulkan.

by Sergey Kosarevsky & Viktor Latypov

Published by Packt

https://www.amazon.com/Graphics-Rendering-Cookbook-comprehensive-algorithms/dp/1838986197

![image](https://user-images.githubusercontent.com/2510143/124914444-05144f80-dff9-11eb-8753-62667b6543ca.png)

===========================

![image](https://user-images.githubusercontent.com/2510143/120815793-d538de80-c558-11eb-9a0b-149b1212a442.png)

===========================

Downloading assets and dependencies:
------------------------------------

Run the `bootstrap.py` script from the root folder of this repository. Make sure you have ~15Gb of free space before you download the assets and build the source code.

Windows build instructions:
---------------------------

```
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
start RenderingCookbook.sln
```

Linux build instructions:
-------------------------

```
mkdir build
cd build
cmake ..
cd ..
```

Running the demos:
------------------

**Run compiled demos from the root folder of this repository**

Demo applications from this book require `.obj`/`.gltf2` assets to be converted to our run-time loadable formats.
Make sure you run `Chapter5/MeshConvert` before running anything else from chapters 5-10,
and `Chapter7/SceneConverter` before running demos from chapters 7-10.

Chapter1
--------

* 1_Bootstrap
* 2_CMake

Chapter2
--------

* 01_GLFW
* 02_GLM
* 03_STB
* 04_ImGui
* 05_EasyProfiler
* 06_Optick
* 07_Assimp
* 08_ETC2Comp
* 09_Taskflow
* 10_MeshOptimizer

Chapter3
--------

* GL01_APIWrapping
* GL02_VtxPulling
* GL03_CubeMap
* VK01_GLSLang
* VK02_DemoApp

Chapter4
--------

* GL01_Camera
* GL02_FPS
* VK01_DemoApp

Chapter5
--------

* GL01_Grid
* GL02_Tessellation
* GL03_MeshRenderer
* MeshConvert
* VK01_MultiMeshDraw

Chapter6
--------

* GL01_PBR
* Util01_FilterEnvmap
* VK01_BRDF_LUT
* VK02_DescriptorIndexing
* VK03_ComputeTexture
* VK04_ComputeMesh
* VK05_PBR

Chapter7
--------

* GL01_LargeScene
* SceneConverter
* VK01_SceneGraph
* VK02_LargeScene

Chapter8
--------

* GL01_ShadowMapping
* GL02_SSAO
* GL03_HDR
* GL04_HDR_Adaptation
* VK01_ShadowMapping
* VK02_SSAO
* VK03_HDR

Chapter9
--------

* VK01_Physics

Chapter10
---------

* GL01_CullingCPU
* GL02_CullingGPU
* GL03_OITransparency
* GL04_LazyLoading
* GL05_Final
* VK01_AtomicsTest
* VK02_Final
