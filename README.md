# cse125-sp21-group3

In order to run the application, download the repo, unzip it, and then run the CSE125Group3.exe file from the Release folder. Use WASD to move around.

Using vcpkg makes installing dependencies very easy. After creating a new solution in visual studio, close your visual studio.

Open a new terminal, and run

git clone https://github.com/Microsoft/vcpkg.git

cd vcpkg

Windows: bootstrap-vcpkg.bat
Unix: ./booststrap-vcpkg.sh

Install 32 bit versions of all the packages. To install 64 bit version append :x64-windows (or mac) to the end of the install command. But 32 bit is easier.

vcpkg install boost-date-time

vcpkg install boost

vcpkg install assimp

vcpkg install glm

vcpkg install opengl

vcpkg install glfw3

vcpkg install glew

vcpkg install zlib

Finish with
vcpkg integrate install

That last command will add these packages GLOBALLY to ALL YOUR VISUAL STUDIO PROJECTS! Without having to setup any include or linker settings!

Restart Visual Studio for changes to take effect. 



To run the projects, right click the solution and click "Properties"

Click "Multiple Startup Projects"
Make sure "Server" is above "Client"
Set "Server" to "Start"
Set "Client" to "Start without debugging"








KEYS:
W - Move Forward
A - Move Left
S - Move Backward
D - Move Right

Z - Move Up (only available when dead)
X - Move Down (only available when dead)

F - Pick Up Ability (Must be looking at ability chest)
E - Use Ability

Left Mouse Click - Shoot

Left Shift - Sprint (Cannot shoot while sprinting)
Control - Crouch

