// ================================================================= //
// @File: cam_manager.cpp
// @Author: Qiao Rukun
// @Date: 2019.05.15
// @LastEditTime: 2019.05.15
// @LastEditors: Qiao Rukun
// ================================================================= //

// @brief the cam_manager.cpp is used for acquiring images from given camera. 
// For now, the camera we use is the PointGrey FL3-U3-13E4M-C camera, which is a grey camera
// with 1280x1024 resolution, global shutter. The FL3-U3-13E4C-C is also avaliable, with using 
// change init part of the class.

#ifndef _CAMMANAGER_H_
#define _CAMMANAGER_H_

#include <opencv2/opencv.hpp>
#include "../static_para.h"
#include "../Support/visual_mod.h"
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>

using namespace Spinnaker;
using namespace Spinnaker::GenApi;

// 摄像头控制类。获取一帧摄像头图片，并实现对摄像头的管理（打开、关闭等等）
// The controller of camera. Used for get one frame from camera. Manage all related operations with camera.
// Example:
//    my_camera = CamManager();
//    my_camera = InitCamera();
//      ......
//      cv::Mat * frames = my_camera.CollectMultiFrame(frame_num);
//      ......
//    my_camera = CloseCamera();
// Tips:
//    None.
class CamManager {
private:
	int device_num_;			              // camera numbers in total
	SystemPtr ptr_system_;		      // for camera system
  CameraList cam_list_;    // camera list (with only one)
  bool is_valid_;                     // System is valid now, ready for collection

public:
	CamManager();
	~CamManager();
	int device_num();		// get total camera number
  bool is_valid();         // System is valid now
	bool InitCamera();			// Initialization & SetValue
  // cv::Mat* CollectMultiColorFrame(int frame_num);  // Collect multiple frames
  cv::Mat* CollectMultiFrame(int frame_num);
  bool ShowCollectionResult(VisualModule * window, int stop_key);  // CollectImages & Show in window
	bool CloseCamera();			// Close camera and Stop Collection
};

#endif