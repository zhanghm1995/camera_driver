cmake_minimum_required(VERSION 2.8.3)
project(marko_camera)

#cjs180314增加对于Marko相机源文件的索引
set(SRC_LIST ${SRC_LIST} src/Source/ApiController.cpp)
set(SRC_LIST ${SRC_LIST} src/Source/FrameObserver.cpp)
set(SRC_LIST ${SRC_LIST} src/Source/Markocamera.cpp)

#cjs180314增加对于相机的包含文件的引用
set(INC_LIST ${INC_LIST} src/Source)
set(INC_LIST ${INC_LIST} /home/zhanghm/Softwares/Vimba_2_1/)
set(INC_LIST ${INC_LIST} /home/zhanghm/Softwares/Vimba_2_1/VimbaCPP/Examples)
set(INC_LIST ${INC_LIST} /home/zhanghm/Softwares/Vimba_2_1/VimbaImageTransform/Include)
set(INC_LIST ${INC_LIST} /home/zhanghm/Softwares/Vimba_2_1/VimbaCPP/Include)
set(INC_LIST ${INC_LIST} /usr/local/include)

#cjs180314增加对于Marko相机的库文件的应用
set(LIB_LIST ${LIB_LIST} /usr/local/lib/libVimbaCPP.so)
set(LIB_LIST ${LIB_LIST} /usr/local/lib/libVimbaC.so)
set(LIB_LIST ${LIB_LIST} /usr/local/lib/libVimbaImageTransform.so)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES my_image_transport
#  CATKIN_DEPENDS cv_bridge image_transport
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
# include
  ${catkin_INCLUDE_DIRS} ${INC_LIST}
)


find_package(OpenCV)
include_directories(include ${OpenCV_INCLUDE_DIRS})
#build my_publisher and my_subscriber
add_executable(my_publisher src/my_publisher.cpp ${SRC_LIST})
target_link_libraries(my_publisher ${catkin_LIBRARIES} ${LIB_LIST} ${OpenCV_LIBS})

add_executable(my_subscriber src/my_subscriber.cpp )
target_link_libraries(my_subscriber ${catkin_LIBRARIES} ${OpenCV_LIBS} ${LIB_LIST})

##############################
###### install packages #########
##############################

install(TARGETS my_subscriber my_publisher
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(
  TARGETS my_subscriber my_publisher
  EXPORT my_image_transportExport
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)
