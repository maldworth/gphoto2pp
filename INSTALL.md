Installation
============
Don't be overwhelmed with all the installation steps listed below here. Most distros can install both **libgphoto** and **cmake** through their package manger, and then all you need to do is follow the instructions in the [gphoto2pp section](#installing-gphoto2pp).

Table Of Contents
-----------------
* [User Prerequisites](#user-prerequisites)
* [Dev/Test/Contributor Prerequisites](#devtestcontributor-prerequisites)
* [Libgphoto2 (Simple)](#installing-libgphoto2-simple)
* [Libgphoto2 (Advanced)](#installing-libgphoto2-advanced)
* [CMake](#installing-cmake)
* [Dev/Test/Contributor Only](#installing-devtestcontributor-only)
* [gphoto2pp](#installing-gphoto2pp)

User Prerequisites
------------------
* A C++11 compiler (I've only tested g++, but others should work)
* libgphoto2 (2.4.14 or greater)
* cmake (2.8.3 or greater)

Dev/Test/Contributor Prerequisites
----------------------------------
Everything mentioned in 'User Prerequisites' plus:
* cxxtest 
* '(optional)' doxygen and graphviz
Once complete the installation, please also view the [Dev-Test document](DEV-TEST.md) for more information.

Installing libgphoto2 (Simple)
------------------------------
``apt-get install libgphoto2-2-dev`` or ``brew install libgphoto2`` or yum, or whatever package management system your OS uses

Installing libgphoto2 (Advanced)
--------------------------------
You only need to do this if you explicitly want the latest and greatest libgphoto2 to work with. But most distros have 2.4.14 or greater in their package management system, in which case you should probably use the simple way.
* download libgphoto from http://www.sourceforge.net/projects/gphoto/files
* If you are using this method I'm going to assume you are a more proficient and can follow the INSTALL instructions
* If you need some references, check out: http://github.com/gonzalo/gphoto2-updater http://www.ohmypi.com/2013/06/07/compiling-gphoto2-dependencies-how-to/ or http://www.yannock.be/computer/compiling-gphoto2-on-the-raspberry-pi (note: some of these guide download and compile/install the libusbx-1.X... beforehand, which I didn't have to do on my distro, I just did **apt-get install libusb-1.0.0-dev)

Installing CMake
----------------
Again, most distros have this, so install it through your package manager 'brew, apt-get, yum, etc...'

Installing Dev/Test/Contributor Only
------------------------------------
Install ``cxxtest`` using package manager.
Also, if you'd like to generate the documentation, install ``doxygen`` and ``graphviz`` the same way. You can make the documentation by typing ``make doc``.

Installing gphoto2pp
--------------------
We are almost there.
* Download gphoto2pp from here (of course), and extract it to any directory of your choosing.
* Open a terminal and go into the ./gphoto2pp/build folder. Then run the command ``cmake ../``. CMake will scan and make sure gphoto2 is linked in the system before continuing, and then it will create all the 'Makefile's which will be used to compile and install gphoto2pp
* Pay attention to the output, if there's an error saying 'Could NOT find CxxTest...' that's okay if you are just a User because you will not be running any unit tests
* Now enter the command ``make``, and it will build the library and all examples.
* Lastly, you most likely want to install, as this will copy the library and headers to your system lib paths. Run the command ``make install`` and let it do it's magic. (will probably require sudo)
* You might have to use the command ``ldconfig`` to update your PATH with the new library (this is the command my OS, it might vary)
* Optional, if you want to uninstall at any time, you can run ``make uninstall`` from this same directory. You will need to have the 'install_manifest.txt' preserved so the uninstall know's what files to uninstall. (again, will probably require sudo)
