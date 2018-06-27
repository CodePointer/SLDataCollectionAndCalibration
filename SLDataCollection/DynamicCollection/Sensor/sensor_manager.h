#ifndef _SENSORMANAGER_H_
#define _SENSORMANAGER_H_

#include <string>
#include <opencv2/opencv.hpp>
#include <strstream>
#include "cam_manager.h"
#include "pro_manager.h"
#include "../static_para.h"
#include "../global_fun.h"

// Sensor module
// usage: init, load/unload, close
class SensorManager {
private:
	// Pattern set£º
	int pattern_num_;
	int now_num_;
	cv::Mat * pattern_mats_;

	// devices:
	CamManager * cam_device_;
	ProManager * pro_device_;

public:
	SensorManager();
	~SensorManager();

	// Initialization
	bool InitSensor();

	// Close sensor.
	bool CloseSensor();

	// Read patterns
	bool LoadPatterns(int pattern_num, std::string file_path,
      std::string file_name, std::string file_suffix);

	// Release patterns
	bool UnloadPatterns();

	// Set project patterns, count from 0
	bool SetProPicture(int pat_idx = 0);
	
	// Get camera picture according to index
	cv::Mat* GetCamColorFrames(int frame_num);

  cv::Mat* GetCamMonoFrames(int frame_num);

  bool ShowCaptureResult(VisualModule* window, int stop_key);

	// Get now projected pattern
  cv::Mat GetProPicture();
	
};

#endif