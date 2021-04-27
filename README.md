# cse125-sp21-group3

In order to run the application, download the repo, unzip it, and then run the CSE125Group3.exe file from the Release folder. Use WASD to move around.

If you are using Visual Studio, here are the instructions on how to compile and build the code.
http://ivl.calit2.net/wiki/index.php/BasecodeCSE167F19

In order to load in models, add the include folder located at "cse125-sp21-group3\assimp-5.0.1\include" and the include folder located at "cse125-sp21-group3\assimp-5.0.1\build\include" to the additional include directories section of the C/C++ project properties. Also add these two lib folder directories located at "cse125-sp21-group3\assimp-5.0.1\build\code\Release" and "cse125-sp21-group3\assimp-5.0.1\build\Release" to the additional library directories section of the Linker project properties. Then, add the "assimp-vc142-mt.lib", "IrrXML.lib", and the "zlib.lib" files to the additional dependencies section under Input of the Linker project properties. If you encounter errors when running the program such as "assimp-vc142-mtd.dll" not found, then copy these files to your System32 directory which should be located at C:\Windows\.
