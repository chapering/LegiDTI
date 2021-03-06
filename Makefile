# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/chap/DTI-SVL/hcai/LegiDTI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/chap/DTI-SVL/hcai/LegiDTI

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/chap/DTI-SVL/hcai/LegiDTI/CMakeFiles /home/chap/DTI-SVL/hcai/LegiDTI/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/chap/DTI-SVL/hcai/LegiDTI/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named LegiDTI

# Build rule for target.
LegiDTI: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 LegiDTI
.PHONY : LegiDTI

# fast build rule for target.
LegiDTI/fast:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/build
.PHONY : LegiDTI/fast

GLoader.o: GLoader.cpp.o
.PHONY : GLoader.o

# target to build an object file
GLoader.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/GLoader.cpp.o
.PHONY : GLoader.cpp.o

GLoader.i: GLoader.cpp.i
.PHONY : GLoader.i

# target to preprocess a source file
GLoader.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/GLoader.cpp.i
.PHONY : GLoader.cpp.i

GLoader.s: GLoader.cpp.s
.PHONY : GLoader.s

# target to generate assembly for a file
GLoader.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/GLoader.cpp.s
.PHONY : GLoader.cpp.s

LegiDTI.o: LegiDTI.cpp.o
.PHONY : LegiDTI.o

# target to build an object file
LegiDTI.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/LegiDTI.cpp.o
.PHONY : LegiDTI.cpp.o

LegiDTI.i: LegiDTI.cpp.i
.PHONY : LegiDTI.i

# target to preprocess a source file
LegiDTI.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/LegiDTI.cpp.i
.PHONY : LegiDTI.cpp.i

LegiDTI.s: LegiDTI.cpp.s
.PHONY : LegiDTI.s

# target to generate assembly for a file
LegiDTI.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/LegiDTI.cpp.s
.PHONY : LegiDTI.cpp.s

imgVolRender.o: imgVolRender.cpp.o
.PHONY : imgVolRender.o

# target to build an object file
imgVolRender.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/imgVolRender.cpp.o
.PHONY : imgVolRender.cpp.o

imgVolRender.i: imgVolRender.cpp.i
.PHONY : imgVolRender.i

# target to preprocess a source file
imgVolRender.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/imgVolRender.cpp.i
.PHONY : imgVolRender.cpp.i

imgVolRender.s: imgVolRender.cpp.s
.PHONY : imgVolRender.s

# target to generate assembly for a file
imgVolRender.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/imgVolRender.cpp.s
.PHONY : imgVolRender.cpp.s

moc_imgVolRender.o: moc_imgVolRender.cxx.o
.PHONY : moc_imgVolRender.o

# target to build an object file
moc_imgVolRender.cxx.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_imgVolRender.cxx.o
.PHONY : moc_imgVolRender.cxx.o

moc_imgVolRender.i: moc_imgVolRender.cxx.i
.PHONY : moc_imgVolRender.i

# target to preprocess a source file
moc_imgVolRender.cxx.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_imgVolRender.cxx.i
.PHONY : moc_imgVolRender.cxx.i

moc_imgVolRender.s: moc_imgVolRender.cxx.s
.PHONY : moc_imgVolRender.s

# target to generate assembly for a file
moc_imgVolRender.cxx.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_imgVolRender.cxx.s
.PHONY : moc_imgVolRender.cxx.s

moc_tfWidget.o: moc_tfWidget.cxx.o
.PHONY : moc_tfWidget.o

# target to build an object file
moc_tfWidget.cxx.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_tfWidget.cxx.o
.PHONY : moc_tfWidget.cxx.o

moc_tfWidget.i: moc_tfWidget.cxx.i
.PHONY : moc_tfWidget.i

# target to preprocess a source file
moc_tfWidget.cxx.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_tfWidget.cxx.i
.PHONY : moc_tfWidget.cxx.i

moc_tfWidget.s: moc_tfWidget.cxx.s
.PHONY : moc_tfWidget.s

# target to generate assembly for a file
moc_tfWidget.cxx.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_tfWidget.cxx.s
.PHONY : moc_tfWidget.cxx.s

moc_vmRenderWindow.o: moc_vmRenderWindow.cxx.o
.PHONY : moc_vmRenderWindow.o

# target to build an object file
moc_vmRenderWindow.cxx.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_vmRenderWindow.cxx.o
.PHONY : moc_vmRenderWindow.cxx.o

moc_vmRenderWindow.i: moc_vmRenderWindow.cxx.i
.PHONY : moc_vmRenderWindow.i

# target to preprocess a source file
moc_vmRenderWindow.cxx.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_vmRenderWindow.cxx.i
.PHONY : moc_vmRenderWindow.cxx.i

moc_vmRenderWindow.s: moc_vmRenderWindow.cxx.s
.PHONY : moc_vmRenderWindow.s

# target to generate assembly for a file
moc_vmRenderWindow.cxx.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/moc_vmRenderWindow.cxx.s
.PHONY : moc_vmRenderWindow.cxx.s

tfWidget.o: tfWidget.cpp.o
.PHONY : tfWidget.o

# target to build an object file
tfWidget.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/tfWidget.cpp.o
.PHONY : tfWidget.cpp.o

tfWidget.i: tfWidget.cpp.i
.PHONY : tfWidget.i

# target to preprocess a source file
tfWidget.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/tfWidget.cpp.i
.PHONY : tfWidget.cpp.i

tfWidget.s: tfWidget.cpp.s
.PHONY : tfWidget.s

# target to generate assembly for a file
tfWidget.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/tfWidget.cpp.s
.PHONY : tfWidget.cpp.s

tgdataReader.o: tgdataReader.cpp.o
.PHONY : tgdataReader.o

# target to build an object file
tgdataReader.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/tgdataReader.cpp.o
.PHONY : tgdataReader.cpp.o

tgdataReader.i: tgdataReader.cpp.i
.PHONY : tgdataReader.i

# target to preprocess a source file
tgdataReader.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/tgdataReader.cpp.i
.PHONY : tgdataReader.cpp.i

tgdataReader.s: tgdataReader.cpp.s
.PHONY : tgdataReader.s

# target to generate assembly for a file
tgdataReader.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/tgdataReader.cpp.s
.PHONY : tgdataReader.cpp.s

vmRenderWindow.o: vmRenderWindow.cpp.o
.PHONY : vmRenderWindow.o

# target to build an object file
vmRenderWindow.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vmRenderWindow.cpp.o
.PHONY : vmRenderWindow.cpp.o

vmRenderWindow.i: vmRenderWindow.cpp.i
.PHONY : vmRenderWindow.i

# target to preprocess a source file
vmRenderWindow.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vmRenderWindow.cpp.i
.PHONY : vmRenderWindow.cpp.i

vmRenderWindow.s: vmRenderWindow.cpp.s
.PHONY : vmRenderWindow.s

# target to generate assembly for a file
vmRenderWindow.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vmRenderWindow.cpp.s
.PHONY : vmRenderWindow.cpp.s

vtkDepthSortPoints.o: vtkDepthSortPoints.cpp.o
.PHONY : vtkDepthSortPoints.o

# target to build an object file
vtkDepthSortPoints.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkDepthSortPoints.cpp.o
.PHONY : vtkDepthSortPoints.cpp.o

vtkDepthSortPoints.i: vtkDepthSortPoints.cpp.i
.PHONY : vtkDepthSortPoints.i

# target to preprocess a source file
vtkDepthSortPoints.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkDepthSortPoints.cpp.i
.PHONY : vtkDepthSortPoints.cpp.i

vtkDepthSortPoints.s: vtkDepthSortPoints.cpp.s
.PHONY : vtkDepthSortPoints.s

# target to generate assembly for a file
vtkDepthSortPoints.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkDepthSortPoints.cpp.s
.PHONY : vtkDepthSortPoints.cpp.s

vtkLegiInteractorStyle.o: vtkLegiInteractorStyle.cpp.o
.PHONY : vtkLegiInteractorStyle.o

# target to build an object file
vtkLegiInteractorStyle.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkLegiInteractorStyle.cpp.o
.PHONY : vtkLegiInteractorStyle.cpp.o

vtkLegiInteractorStyle.i: vtkLegiInteractorStyle.cpp.i
.PHONY : vtkLegiInteractorStyle.i

# target to preprocess a source file
vtkLegiInteractorStyle.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkLegiInteractorStyle.cpp.i
.PHONY : vtkLegiInteractorStyle.cpp.i

vtkLegiInteractorStyle.s: vtkLegiInteractorStyle.cpp.s
.PHONY : vtkLegiInteractorStyle.s

# target to generate assembly for a file
vtkLegiInteractorStyle.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkLegiInteractorStyle.cpp.s
.PHONY : vtkLegiInteractorStyle.cpp.s

vtkTubeFilterEx.o: vtkTubeFilterEx.cpp.o
.PHONY : vtkTubeFilterEx.o

# target to build an object file
vtkTubeFilterEx.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkTubeFilterEx.cpp.o
.PHONY : vtkTubeFilterEx.cpp.o

vtkTubeFilterEx.i: vtkTubeFilterEx.cpp.i
.PHONY : vtkTubeFilterEx.i

# target to preprocess a source file
vtkTubeFilterEx.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkTubeFilterEx.cpp.i
.PHONY : vtkTubeFilterEx.cpp.i

vtkTubeFilterEx.s: vtkTubeFilterEx.cpp.s
.PHONY : vtkTubeFilterEx.s

# target to generate assembly for a file
vtkTubeFilterEx.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkTubeFilterEx.cpp.s
.PHONY : vtkTubeFilterEx.cpp.s

vtkTubeHaloMapper.o: vtkTubeHaloMapper.cpp.o
.PHONY : vtkTubeHaloMapper.o

# target to build an object file
vtkTubeHaloMapper.cpp.o:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkTubeHaloMapper.cpp.o
.PHONY : vtkTubeHaloMapper.cpp.o

vtkTubeHaloMapper.i: vtkTubeHaloMapper.cpp.i
.PHONY : vtkTubeHaloMapper.i

# target to preprocess a source file
vtkTubeHaloMapper.cpp.i:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkTubeHaloMapper.cpp.i
.PHONY : vtkTubeHaloMapper.cpp.i

vtkTubeHaloMapper.s: vtkTubeHaloMapper.cpp.s
.PHONY : vtkTubeHaloMapper.s

# target to generate assembly for a file
vtkTubeHaloMapper.cpp.s:
	$(MAKE) -f CMakeFiles/LegiDTI.dir/build.make CMakeFiles/LegiDTI.dir/vtkTubeHaloMapper.cpp.s
.PHONY : vtkTubeHaloMapper.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... LegiDTI"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... GLoader.o"
	@echo "... GLoader.i"
	@echo "... GLoader.s"
	@echo "... LegiDTI.o"
	@echo "... LegiDTI.i"
	@echo "... LegiDTI.s"
	@echo "... imgVolRender.o"
	@echo "... imgVolRender.i"
	@echo "... imgVolRender.s"
	@echo "... moc_imgVolRender.o"
	@echo "... moc_imgVolRender.i"
	@echo "... moc_imgVolRender.s"
	@echo "... moc_tfWidget.o"
	@echo "... moc_tfWidget.i"
	@echo "... moc_tfWidget.s"
	@echo "... moc_vmRenderWindow.o"
	@echo "... moc_vmRenderWindow.i"
	@echo "... moc_vmRenderWindow.s"
	@echo "... tfWidget.o"
	@echo "... tfWidget.i"
	@echo "... tfWidget.s"
	@echo "... tgdataReader.o"
	@echo "... tgdataReader.i"
	@echo "... tgdataReader.s"
	@echo "... vmRenderWindow.o"
	@echo "... vmRenderWindow.i"
	@echo "... vmRenderWindow.s"
	@echo "... vtkDepthSortPoints.o"
	@echo "... vtkDepthSortPoints.i"
	@echo "... vtkDepthSortPoints.s"
	@echo "... vtkLegiInteractorStyle.o"
	@echo "... vtkLegiInteractorStyle.i"
	@echo "... vtkLegiInteractorStyle.s"
	@echo "... vtkTubeFilterEx.o"
	@echo "... vtkTubeFilterEx.i"
	@echo "... vtkTubeFilterEx.s"
	@echo "... vtkTubeHaloMapper.o"
	@echo "... vtkTubeHaloMapper.i"
	@echo "... vtkTubeHaloMapper.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

