developer notice:

add Boost home path and WTL include path to:
Visual Studio Options > Projects and Solutions > VC++ Directories > Include files;

build boost libraries with msvc toolset (see boost documentation);
add <boost home>\stage\lib to:
Visual Studio Options > Projects and Solutions > VC++ Directories > Library files;

voila.