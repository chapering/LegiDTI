cmake_minimum_required(VERSION 2.8)
 
PROJECT(LegiDTI)
 
FIND_PACKAGE(VTK)
INCLUDE(${VTK_USE_FILE})
 
FIND_PACKAGE(Qt4 REQUIRED)
INCLUDE(${QT_USE_FILE})

set(OTHER_FLAGS "-DLINUX -Wall -O3")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${OTHER_FLAGS}")

INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}" "/usr/local/include/nifti")

# Set your files and resources here
SET(LegiDTISrcs 
	vtkLegiInteractorStyle.cpp
	vtkDepthSortPoints.cpp
	vtkTubeFilterEx.cpp vtkTubeHaloMapper.cpp
	GLoader.cpp tgdataReader.cpp 
	LegiDTI.cpp imgVolRender.cpp 
	tfWidget.cpp vmRenderWindow.cpp)
SET(LegiDTIUI testbed.ui)
SET(LegiDTIHeaders imgVolRender.h tfWidget.h vmRenderWindow.h)
 
QT4_WRAP_UI(UISrcs ${LegiDTIUI})
QT4_WRAP_CPP(MOCSrcs ${LegiDTIHeaders} )
 
SOURCE_GROUP("Resources" FILES
  ${LegiDTIUI}
)
 
SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
  ${RCS_SOURCES}
)
 
ADD_EXECUTABLE( LegiDTI ${LegiDTISrcs} ${UISrcs} ${MOCSrcs})
TARGET_LINK_LIBRARIES( LegiDTI QVTK niftiio znz z GL kaapi "kaapi++" )
