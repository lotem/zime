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

voila! 

now, make a release build, and run $(SolutionDir)\release.bat.
the product will be place in $(SolutionDir)\output.

usage:

install.bat
(require super user previlege)

uninstall.bat
(require super user previlege)

stop_service.bat
you may use this to terminate ZIME core service process.

before you go...
issue the command populate-db.bat in $(SolutionDir)\data,
in case you never had a zime database at %userprofile%\.ibus\zime\zime.db.
otherwise, you will be noticed a message "NO SCHEMA" (in Chinese) on your first try.
