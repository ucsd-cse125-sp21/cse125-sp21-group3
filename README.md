# cse125-sp21-group3

In order to run the application, download the repo, unzip it, and then run the CSE125Group3.exe file from the Release folder. Use WASD to move around.

If you are using Visual Studio, here are the instructions on how to compile and build the code. 
http://ivl.calit2.net/wiki/index.php/BasecodeCSE167F19

IMPORTANT: Due to the way Assimp was built, the code must be built using the "Release" and "x64" configurations in Visual Studio. 

In order to load in models, add the include folder located at "cse125-sp21-group3\assimp-5.0.1\include" and the include folder located at "cse125-sp21-group3\assimp-5.0.1\build\include" to the additional include directories section of the C/C++ project properties. Also add these two lib folder directories located at "cse125-sp21-group3\assimp-5.0.1\build\code\Release" and "cse125-sp21-group3\assimp-5.0.1\build\Release" to the additional library directories section of the Linker project properties. Then, add the "assimp-vc142-mt.lib", "IrrXML.lib", and the "zlib.lib" files to the additional dependencies section under Input of the Linker project properties. If you encounter errors when running the program such as "assimp-vc142-mtd.dll" not found, then copy these files to your System32 directory which should be located at C:\Windows\.

Getting Boost Libraries:

https://www.boost.org/doc/libs/1_69_0/more/getting_started/index.html

Download Boost libraries from this link.  Currently, we (Michael and Robert) have
the Unix variants, but we will be switching over to Windows in the future.

Save the library on your machine.  The folder will be named boost_1_75_0.


Compiling with Boost Libraries:

Include path is necessary for compiling with Boost.

c++ -I <path>/boost_1_75_0 echoServer.cpp -o echoServer

c++ -I <path>/boost_1_75_0 echoClient.cpp -o echoClient


To create the client and server, create a Visual Studio solution with two projects.

Add all .cpp/.h files except echoServer and serverParse to one project

Add only echoServer.cpp, serverParse.cpp, and serverParse.h to the other project




Right click the Server project, and click "Properties"

VC++ Directories: Library Directories
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0


C/C++: General: Additional Include Directories
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0\stage\lib
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0


Linker: General: Additional Library Directories
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0\stage\lib
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0

Linker: General: Link Library Dependencies
Yes

Linker: Input: Additional Dependencies
libboost_date_time-vc142-mt-x64-1_76.lib
libboost_system-vc142-mt-x64-1_76.lib






Right click the Client project, and click "Properties"

VC++ Directories: Library Directories
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0


C/C++: General: Additional Include Directories
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\assimp-5.0.1
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\assimp-5.0.1\build\include
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\assimp-5.0.1\include
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\irrKlang-64bit-1.6.0\include

Linker: General: Additional Library Directories
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\assimp-5.0.1
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\boost_1_76_0
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\assimp-5.0.1\build\Release
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\assimp-5.0.1\build\code\Release
C:\Programs\CSE 125\cse125-sp21-group3-Graphics\irrKlang-64bit-1.6.0\lib\Winx64-visualStudio

Linker: General: Link Library Dependencies
Yes

Linker: Input: Additional Dependencies
libboost_date_time-vc142-mt-x64-1_76.lib
libboost_system-vc142-mt-x64-1_76.lib
assimp-vc142-mt.lib
IrrXML.lib
zlib.lib
irrKlang.lib
opengl32.lib


To run the projects, right click the solution and click "Properties"

Click "Multiple Startup Projects"
Make sure "Server" is above "Client"
Set "Server" to "Start"
Set "Client" to "Start without debugging"

