### 程序理解
- 最终生成可执行文件
```CMake
add_executable(GetImage GetImage.cc)
target_link_libraries(GetImage CameraFuncs ${LIB_LIST})
```
其中链接库CameraFuncs由**pro**文件夹以下几个文件生成
```CMake
aux_source_directory(. DIR_LIB_SRCS)
add_library (CameraFuncs ${DIR_LIB_SRCS})
```

### 问题？
- 相机是网口还是什么接口的？只用单个相机的话，只要连接好就行吗，程序中只要将**Camnum**设置为1就行吗？
- Camera.RectifyImage()对图像做了一个什么样的校正工作？消除畸变吗？