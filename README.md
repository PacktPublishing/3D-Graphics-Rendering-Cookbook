


# 3D Graphics Rendering Cookbook

**A comprehensive guide to exploring rendering algorithms in modern OpenGL and Vulkan**

by <a href="https://github.com/corporateshark">Sergey Kosarevsky</a> & <a href="https://github.com/LLViktor">Viktor Latypov</a>

<a href="https://www.packtpub.com/game-development/3d-graphics-rendering-cookbook?utm_source=github&utm_medium=repository&utm_campaign=9781786461629"><img src="https://user-images.githubusercontent.com/2510143/124914444-05144f80-dff9-11eb-8753-62667b6543ca.png" alt="Book Name" height="256px" align="right"></a>

This is the code repository for [3D Graphics Rendering Cookbook](https://www.packtpub.com/game-development/3d-graphics-rendering-cookbook?utm_source=github&utm_medium=repository&utm_campaign=9781786461629), published by Packt.



## What is this book about?
OpenGL is a popular cross-language, cross-platform application programming interface (API) used for rendering 2D and 3D graphics, while Vulkan is a low-overhead, cross-platform 3D graphics API that targets high-performance applications. 3D Graphics Rendering Cookbook helps you learn about modern graphics rendering algorithms and techniques using C++ programming along with OpenGL and Vulkan APIs.

This book covers the following exciting features:

* Improve the performance of legacy OpenGL applications
* Manage a substantial amount of content in real-time 3D rendering engines
* Discover how to debug and profile graphics applications
* Understand how to use the Approaching Zero Driver Overhead (AZDO) philosophy in OpenGL
* Integrate various rendering techniques into a single application
* Find out how to develop Vulkan applications
* Implement a physically based rendering pipeline from scratch
* Integrate a physics library with your rendering engine

If you feel this book is for you, get your [copy](https://www.amazon.com/Graphics-Rendering-Cookbook-comprehensive-algorithms/dp/1838986197) today!


Software and Hardware List
--------------------------

| Chapter | Software required | OS required |
| -------- | ------------------------------------ | ----------------------------------- |
| 1-10 | Microsoft Visual Studio 2019 | Windows |
| 1-10 | GCC 8 | Linux |
| 1-10 | Python 3.x | Windows, Linux |
| 1-10 | Vulkan SDK | Windows, Linux |

The code runs on graphics cards as old as GeForce 740M. However, GeForce GTX 1060 or newer is strongly recommended.

## Instructions and Navigations
All of the code is organized into folders. For example, Chapter01.

The code will look like the following:
```
macro(SETUP_GROUPS src_files)
foreach(FILE ${src_files})
get_filename_component(PARENT_DIR "${FILE}" PATH)
```

**Following is what you need for this book:**
This book is for 3D graphics developers who are familiar with the mathematical fundamentals of 3D rendering and want to gain expertise in writing fast rendering engines with advanced techniques using C++ libraries and APIs. A solid understanding of C++ and basic linear algebra, as well as experience in creating custom 3D applications without using premade rendering engines is required.

With the following software and hardware list you can run all code files present in the book (Chapter 1-10).


Downloading assets and dependencies:
------------------------------------

Run the `bootstrap.py` script from the root folder of this repository. Make sure you have ~15Gb of free space before you download the assets and build the source code.

Models downloaded from Morgan McGuire's [Computer Graphics Archive](https://casual-effects.com/data)

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

  ![image](https://user-images.githubusercontent.com/2510143/126911851-d3f383bd-0a87-4dfb-a1c2-37c2d81d2f8e.png)

* VK01_BRDF_LUT

  ![image](https://user-images.githubusercontent.com/2510143/126911829-01cd9d5c-5994-43ea-beef-14aa22b97512.png)

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

  ![image](https://user-images.githubusercontent.com/2510143/127224028-df442bb5-8940-4411-9d99-7af7cce63ec7.png)

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

  ![image](https://user-images.githubusercontent.com/2510143/127025874-b13eba3b-7406-45a7-bdda-d3e3fbc1bb17.png)

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

We also provide a PDF file that has color images of the screenshots/diagrams used in this book. [Click here to download it](https://static.packt-cdn.com/downloads/9781838986193_ColorImages.pdf).

### Related products
* OpenGL 4 Shading Language Cookbook - Third Edition [[Packt]](https://www.packtpub.com/product/opengl-4-shading-language-cookbook-third-edition/9781789342253) [[Amazon]](https://www.amazon.com/OpenGL-Shading-Language-Cookbook-high-quality/dp/1789342252)

* Vulkan Cookbook [[Packt]](https://www.packtpub.com/product/vulkan-cookbook/9781786468154?utm_source=github&utm_medium=repository&utm_campaign=9781786468154) [[Amazon]](https://www.amazon.com/dp/1786468158)


## Get to Know the Authors
**Sergey Kosarevsky**
Sergey Kosarevsky is a rendering lead at Ubisoft RedLynx. He worked in the mobile industry at SPB Software, Yandex, Layar and Blippar, TWNKLS, and DAQRI, where he designed and implemented real-time rendering technology. He has more than 18 years of software development experience and more than 10 years of mobile and embedded 3D graphics experience. In his Ph.D. thesis, Sergey employed computer vision to solve mechanical engineering problems. He has co-authored several books on mobile software development in C++.

**Viktor Latypov**
Viktor Latypov is a software engineer specializing in embedded C/C++, 3D graphics, and computer vision. With more than 15 years of software development experience and a Ph.D. in Applied Mathematics, he has implemented a number of real-time renderers for medical and automotive applications over the last 10 years.
Together with Sergey, he has co-authored two books on mobile software development in C++



## Other books by the authors
[Android NDK Game Development Cookbook](https://www.packtpub.com/product/android-ndk-game-development-cookbook/9781782167785?utm_source=github&utm_medium=repository&utm_campaign=9781782167785)

[Mastering Android NDK](https://www.amazon.com/Mastering-Android-NDK-Sergey-Kosarevsky-ebook/dp/B0151N0GAO)
### Download a free PDF

 <i>If you have already purchased a print or Kindle version of this book, you can get a DRM-free PDF version at no cost.<br>Simply click on the link to claim your free PDF.</i>
<p align="center"> <a href="https://packt.link/free-ebook/9781838986193">https://packt.link/free-ebook/9781838986193 </a> </p>