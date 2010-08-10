developer notice:

build boost libraries with msvc toolset (see boost documentation);

then

copy <boost home>\boost\* to $(SolutionDir)\include\boost;
copy <boost home>\stage\lib\* to $(SolutionDir)\lib;
copy X:\Python27\include\* to $(SolutionDir)\include\python;
copy X:\Python27\libs\* to $(SolutionDir)\lib\python;

or, alternatively

add boost home path and Python include path to:
Visual Studio Options > Projects and Solutions > VC++ Directories > Include files;
add <boost home>\stage\lib and X:\Python27\libs to:
Visual Studio Options > Projects and Solutions > VC++ Directories > Library files;

voila.