distPi
===================================================

Proof-of-concept distributed microservice architecture. Utilizing Redis for data-distribution, and Protobuf for modeling data-flow.  This stack was chosen for easy remote-monitoring, and portability between Windows and Linux.  Tested in Windows 10, and the Raspberry Pi OS.

Requirements for Windows 10
----------------------------
  * Visual Studio 2019 (with .NET 4.7 and MSVC v142)
	
Requirements for Linux
------------------------------
  * autoconf
  * automake
  * libtool
  * make
  * g++
  * unzip
  
Which, on Raspberry Pi OS, can be installed with:
	sudo apt-get install autoconf automake libtool curl make g++ unzip
