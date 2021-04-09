# cse125-sp21-group3

Getting Boost Libraries:

https://www.boost.org/doc/libs/1_69_0/more/getting_started/index.html

Download Boost libraries from this link.  Currently, we (Michael and Robert) have
the Unix variants, but we will be switching over to Windows in the future.

Save the library on your machine.  The folder will be named boost_1_75_0.


Compiling with Boost Libraries:

Include path is necessary for compiling with Boost.

c++ -I <path>/boost_1_75_0 echoServer.cpp -o echoServer
c++ -I <path>/boost_1_75_0 echoClient.cpp -o echoClient
