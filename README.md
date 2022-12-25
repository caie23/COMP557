# COMP557
Three assignments of COMP557 - Fundamentals of Computer Graphics.

0. Install required softwares according to Professor Paul Kry's getting started code template:
https://github.com/paulkry/Comp557-L00

Run with Xcode:
1. In the folder that contains CMakeLists.txt:
```
mkdir build
cd build
```
2. Then from the build folder:
```
cmake -G Xcode ..
```
3. Open L0x.xcodeproj with Xcode.
4. Change the target by clicking on “ALL_BUILD” and selecting L00.
5. Edit the scheme by going to “Product” -> “Scheme” -> “Edit Scheme” to add command-line arguments (``../../resources``).
(Except A4, command-line arguments example: ``../../resources/scenes/Plane.json ../../out/Plane.png``)
6. Press Command+R to run.
