# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/cmake-3.29.3-linux-x86_64/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake-3.29.3-linux-x86_64/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wyb/Websocket

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wyb/Websocket/build

# Include any dependencies generated for this target.
include CMakeFiles/TCPServer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/TCPServer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/TCPServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/TCPServer.dir/flags.make

CMakeFiles/TCPServer.dir/TCPServer.cpp.o: CMakeFiles/TCPServer.dir/flags.make
CMakeFiles/TCPServer.dir/TCPServer.cpp.o: /home/wyb/Websocket/TCPServer.cpp
CMakeFiles/TCPServer.dir/TCPServer.cpp.o: CMakeFiles/TCPServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wyb/Websocket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/TCPServer.dir/TCPServer.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TCPServer.dir/TCPServer.cpp.o -MF CMakeFiles/TCPServer.dir/TCPServer.cpp.o.d -o CMakeFiles/TCPServer.dir/TCPServer.cpp.o -c /home/wyb/Websocket/TCPServer.cpp

CMakeFiles/TCPServer.dir/TCPServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/TCPServer.dir/TCPServer.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyb/Websocket/TCPServer.cpp > CMakeFiles/TCPServer.dir/TCPServer.cpp.i

CMakeFiles/TCPServer.dir/TCPServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/TCPServer.dir/TCPServer.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyb/Websocket/TCPServer.cpp -o CMakeFiles/TCPServer.dir/TCPServer.cpp.s

CMakeFiles/TCPServer.dir/base64.cpp.o: CMakeFiles/TCPServer.dir/flags.make
CMakeFiles/TCPServer.dir/base64.cpp.o: /home/wyb/Websocket/base64.cpp
CMakeFiles/TCPServer.dir/base64.cpp.o: CMakeFiles/TCPServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wyb/Websocket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/TCPServer.dir/base64.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TCPServer.dir/base64.cpp.o -MF CMakeFiles/TCPServer.dir/base64.cpp.o.d -o CMakeFiles/TCPServer.dir/base64.cpp.o -c /home/wyb/Websocket/base64.cpp

CMakeFiles/TCPServer.dir/base64.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/TCPServer.dir/base64.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyb/Websocket/base64.cpp > CMakeFiles/TCPServer.dir/base64.cpp.i

CMakeFiles/TCPServer.dir/base64.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/TCPServer.dir/base64.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyb/Websocket/base64.cpp -o CMakeFiles/TCPServer.dir/base64.cpp.s

CMakeFiles/TCPServer.dir/sha1.cpp.o: CMakeFiles/TCPServer.dir/flags.make
CMakeFiles/TCPServer.dir/sha1.cpp.o: /home/wyb/Websocket/sha1.cpp
CMakeFiles/TCPServer.dir/sha1.cpp.o: CMakeFiles/TCPServer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wyb/Websocket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/TCPServer.dir/sha1.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/TCPServer.dir/sha1.cpp.o -MF CMakeFiles/TCPServer.dir/sha1.cpp.o.d -o CMakeFiles/TCPServer.dir/sha1.cpp.o -c /home/wyb/Websocket/sha1.cpp

CMakeFiles/TCPServer.dir/sha1.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/TCPServer.dir/sha1.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wyb/Websocket/sha1.cpp > CMakeFiles/TCPServer.dir/sha1.cpp.i

CMakeFiles/TCPServer.dir/sha1.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/TCPServer.dir/sha1.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wyb/Websocket/sha1.cpp -o CMakeFiles/TCPServer.dir/sha1.cpp.s

# Object files for target TCPServer
TCPServer_OBJECTS = \
"CMakeFiles/TCPServer.dir/TCPServer.cpp.o" \
"CMakeFiles/TCPServer.dir/base64.cpp.o" \
"CMakeFiles/TCPServer.dir/sha1.cpp.o"

# External object files for target TCPServer
TCPServer_EXTERNAL_OBJECTS =

TCPServer: CMakeFiles/TCPServer.dir/TCPServer.cpp.o
TCPServer: CMakeFiles/TCPServer.dir/base64.cpp.o
TCPServer: CMakeFiles/TCPServer.dir/sha1.cpp.o
TCPServer: CMakeFiles/TCPServer.dir/build.make
TCPServer: CMakeFiles/TCPServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/wyb/Websocket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable TCPServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/TCPServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/TCPServer.dir/build: TCPServer
.PHONY : CMakeFiles/TCPServer.dir/build

CMakeFiles/TCPServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/TCPServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/TCPServer.dir/clean

CMakeFiles/TCPServer.dir/depend:
	cd /home/wyb/Websocket/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wyb/Websocket /home/wyb/Websocket /home/wyb/Websocket/build /home/wyb/Websocket/build /home/wyb/Websocket/build/CMakeFiles/TCPServer.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/TCPServer.dir/depend

