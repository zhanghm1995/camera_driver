//======================================================================
// Author   : Haiming Zhang
// Email    : zhanghm_1995@qq.com
// Version  :
// Copyright    :
// Descriptoin  : 相机图像获取抽象类，用于统一相机调用接口，具体相机操作实现应继承该类实现
//                输入：
//                输出：相机图像
// References   :
//======================================================================

#ifndef SRC_CAMERA_DRIVER_SRC_ABSTRACT_CAMERA_GRABBER_H_
#define SRC_CAMERA_DRIVER_SRC_ABSTRACT_CAMERA_GRABBER_H_

class AbstractCameraGrabber {
public:
  AbstractCameraGrabber();
  virtual ~AbstractCameraGrabber();
};

#endif /* SRC_CAMERA_DRIVER_SRC_ABSTRACT_CAMERA_GRABBER_H_ */
