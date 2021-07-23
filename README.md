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

  ![image](https://user-images.githubusercontent.com/2510143/126837948-24986193-b1d0-4915-b4bb-ba00d8ac06ea.png)

* 02_GLM

  ![image](https://user-images.githubusercontent.com/2510143/126838583-afe09d28-3dba-4b76-970a-9af2bad7e087.png)

* 03_STB

  ![image](https://user-images.githubusercontent.com/2510143/126838102-3f387f38-95a1-400b-a706-2d3ccb6183fb.png)

* 04_ImGui

  ![image](https://user-images.githubusercontent.com/2510143/126838651-451aff2b-73b2-4594-8ced-bcef9cef21a7.png)

* 05_EasyProfiler
* 06_Optick

  ![image](https://user-images.githubusercontent.com/2510143/126838719-cf91836d-d90a-4aaf-923a-57b4980ceebe.png)

* 07_Assimp

  ![image](https://user-images.githubusercontent.com/2510143/126838754-9c498e1a-5e16-4b74-a63e-6d10efc3edab.png)

* 08_ETC2Comp
* 09_Taskflow
* 10_MeshOptimizer

  ![image](https://user-images.githubusercontent.com/2510143/126838516-9b06bfec-2308-4e66-be46-8957a78a5409.png)


Chapter3
--------

* GL01_APIWrapping
* GL02_VtxPulling

  ![image](https://user-images.githubusercontent.com/2510143/126838905-d1e6b5ec-3abd-4b1d-bfd3-9500361475dc.png)

* GL03_CubeMap

  ![image](https://user-images.githubusercontent.com/2510143/126838957-77af6d5c-9ce3-4096-9a83-c996dc2bdeaf.png)

* VK01_GLSLang
* VK02_DemoApp

  ![image](https://user-images.githubusercontent.com/2510143/126839010-7b6d2194-325b-4705-bbcd-07bc93b29a69.png)


Chapter4
--------

* GL01_Camera

  ![image](https://user-images.githubusercontent.com/2510143/126839117-17dacc6d-d565-45ce-a503-bf12df16fe4e.png)

* GL02_FPS
* VK01_DemoApp

  ![image](https://user-images.githubusercontent.com/2510143/126839269-790a59aa-a6d5-452d-b7f8-6d75493bf08c.png)


Chapter5
--------

* GL01_Grid

  ![image](https://user-images.githubusercontent.com/2510143/126839349-8a790f29-e302-4662-b9f6-e7f79cc5d715.png)

* GL02_Tessellation

  ![image](https://user-images.githubusercontent.com/2510143/126839463-4df5bf4e-3a4c-45e7-9fd8-f881a69e18d0.png)

* GL03_MeshRenderer

  ![image](https://user-images.githubusercontent.com/2510143/126839504-286ac105-00c2-42bf-a7f9-72c6c3a8ee7c.png)

* MeshConvert
* VK01_MultiMeshDraw

  ![image](https://user-images.githubusercontent.com/2510143/126839626-9f4970da-ab60-4ef5-b603-3d4ea5cfd106.png)

Chapter6
--------

* GL01_PBR

  ![image](https://user-images.githubusercontent.com/2510143/126839692-9b6ec131-34cb-47de-95be-97faaf66f43c.png)

* Util01_FilterEnvmap
* VK01_BRDF_LUT
* VK02_DescriptorIndexing

 ![image](https://user-images.githubusercontent.com/2510143/126839776-9642d163-0be5-47f1-90b3-65c5b6767200.png) 

* VK03_ComputeTexture

  ![image](https://user-images.githubusercontent.com/2510143/126839816-b1851753-fcb4-43a7-ad64-1b0cddb08af9.png)

* VK04_ComputeMesh

  ![image](https://user-images.githubusercontent.com/2510143/126839900-8e3309b1-71d5-41c1-8ae8-d7f18b6797ac.png)

* VK05_PBR

  ![image](https://user-images.githubusercontent.com/2510143/126839938-18a5947f-f072-497e-ab5e-74a90c292b77.png)


Chapter7
--------

* GL01_LargeScene

  ![image](https://user-images.githubusercontent.com/2510143/126840137-984c5e0c-109f-4502-ace5-aaf9de282f6a.png)

* SceneConverter
* VK01_SceneGraph

  ![image](https://user-images.githubusercontent.com/2510143/126840328-ed3e1126-65be-466b-bc83-7e263a632293.png)

* VK02_LargeScene

  ![image](https://user-images.githubusercontent.com/2510143/126840405-2feeed0b-0175-4ace-8aea-5ba7bb14b52d.png)

Chapter8
--------

* GL01_ShadowMapping

  ![image](https://user-images.githubusercontent.com/2510143/126840472-2df1141f-4129-4003-a822-ca612c98157f.png)

* GL02_SSAO

  ![image](https://user-images.githubusercontent.com/2510143/126840579-711d530d-5579-4b46-b006-8abd9a72860d.png)

* GL03_HDR

  ![image](https://user-images.githubusercontent.com/2510143/126840720-319808dc-d3ce-497b-9de7-fb6f7f7eac38.png)

* GL04_HDR_Adaptation

  ![image](https://user-images.githubusercontent.com/2510143/126840821-4f63b973-184d-488e-a3ba-f91f2deb4fc8.png)

* VK01_ShadowMapping

  ![image](https://user-images.githubusercontent.com/2510143/126840862-7315dee3-1eee-45a4-8af5-43ab299f97bc.png)

* VK02_SSAO

  ![image](https://user-images.githubusercontent.com/2510143/126840927-42d51fd1-8161-449b-8bf4-5270c9c21573.png)

* VK03_HDR

Chapter9
--------

* VK01_Physics

  ![image](https://user-images.githubusercontent.com/2510143/126841051-7fa15ac0-3849-4e02-ae92-28af07abcd56.png)


Chapter10
---------

* GL01_CullingCPU

  ![image](https://user-images.githubusercontent.com/2510143/126841134-61f7faca-c347-4022-ac06-7bad3a87e205.png)

* GL02_CullingGPU

  ![image](https://user-images.githubusercontent.com/2510143/126841210-f4a4a0b7-2ad7-4a65-8f80-bb3a833b8b33.png)

* GL03_OITransparency

  ![image](https://user-images.githubusercontent.com/2510143/126841294-2653bb2b-cff9-446b-b6b9-301b4043b481.png)

* GL04_LazyLoading

  ![image](https://user-images.githubusercontent.com/2510143/126841382-9ac9631c-6ead-452a-946d-19508f8659ca.png)

* GL05_Final

  ![image](https://user-images.githubusercontent.com/2510143/126841497-19d420eb-5458-493b-9cc0-f66822a17451.png)

* VK01_AtomicsTest

  ![image](https://user-images.githubusercontent.com/2510143/126841533-1e41ce6b-abd9-4e41-9127-5684a7bebae7.png)

* VK02_Final

  ![image](https://user-images.githubusercontent.com/2510143/126841602-e70b4e8d-1dd9-49df-ad9a-54ffa20b72f0.png)

