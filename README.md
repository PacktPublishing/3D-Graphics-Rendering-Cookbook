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

Run the `bootstrap.py` script from the root folder of this repository.

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
