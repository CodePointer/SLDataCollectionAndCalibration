#ifndef _CAMMANAGER_H_
#define _CAMMANAGER_H_

#include <opencv2/opencv.hpp>
#include "../static_para.h"
#include "../Support/visual_mod.h"
#include <Spinnaker.h>

// 摄像头控制类。获取一帧摄像头图片，并实现对摄像头的管理（打开、关闭等等）
// Usage: InitCamera，CollectMultiFrame/ShowCollectionResult，CloseCamera
class CamManager {
private:
	int device_num_;			              // camera numbers in total
	Spinnaker::SystemPtr ptr_system_;		      // for camera system
  Spinnaker::CameraList cam_list_;    // camera list (with only one)
  bool is_valid_;                     // System is valid now, ready for collection

public:
	CamManager();
	~CamManager();
	int get_device_num();		// get total camera number
  bool IsValid();         // System is valid now
	bool InitCamera();			// Initialization & SetValue
  cv::Mat* CollectMultiColorFrame(int frame_num);  // Collect multiple frames
  cv::Mat* CollectMultiMonoFrame(int frame_num);
  bool ShowCollectionResult(VisualModule * window, int stop_key);  // CollectImages & Show in window
	bool CloseCamera();			// Close camera and Stop Collection
};

#endif