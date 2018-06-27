#include "sensor_manager.h"


// Sensor module.
// Used for manage camera and projector.
// Project pattern and capture image.
SensorManager::SensorManager() {
  this->pattern_num_ = 0;
  this->now_num_ = 0;
  this->pattern_mats_ = NULL;
  this->cam_device_ = NULL;
  this->pro_device_ = NULL;
}

SensorManager::~SensorManager() {
	if (this->pattern_mats_ != NULL) {
		delete[]this->pattern_mats_;
		this->pattern_mats_ = NULL;
	}
	if (this->cam_device_ != NULL) {
		delete this->cam_device_;
		this->cam_device_ = NULL;
	}
	if (this->pro_device_ != NULL) {
		delete this->pro_device_;
		this->pro_device_ = NULL;
	}
}

// Initialize sensor.
// Create camera, projector and initialize them.
bool SensorManager::InitSensor() {
	bool status;

  this->cam_device_ = new CamManager();
  status = this->cam_device_->InitCamera();

	this->pro_device_ = new ProManager();
	status = this->pro_device_->InitProjector();

	return status;
}

// Close sensor.
// Release spaces.
bool SensorManager::CloseSensor() {
	bool status = true;

	if (this->cam_device_ != NULL) {
		this->cam_device_->CloseCamera();
		delete this->cam_device_;
		this->cam_device_ = NULL;
	}
	if (this->pro_device_ != NULL) {
		delete this->pro_device_;
		this->pro_device_ = NULL;
	}
	this->UnloadPatterns();

	return status;
}

// Read patterns
bool SensorManager::LoadPatterns(int pattern_num, std::string file_path,
    std::string file_name, std::string file_suffix) {
	// Check status
	if (pattern_mats_ != NULL) {
		UnloadPatterns();
	}
	// Set input parameters
	this->pattern_num_ = pattern_num;
	this->now_num_ = 0;
	this->pattern_mats_ = new cv::Mat[this->pattern_num_];
	// Load patterns
	for (int i = 0; i < this->pattern_num_; i++) {
    std::string idx2Str;
    std::strstream ss;
    ss << i;
    ss >> idx2Str;
    
    cv::Mat tmp_mat;
    std::string read_path = file_path + file_name + idx2Str + file_suffix;
    tmp_mat = cv::imread(read_path, CV_LOAD_IMAGE_COLOR);
    tmp_mat.copyTo(this->pattern_mats_[i]);

		if (tmp_mat.empty()) {
			ErrorHandling("SensorManager::LoadPatterns::<Read>, imread error, file_path:" + read_path);
		}
	}
	return true;
}

// Release patterns
bool SensorManager::UnloadPatterns()
{
	if (this->pattern_mats_ != NULL)
	{
		delete[]this->pattern_mats_;
		this->pattern_mats_ = NULL;
	}
	this->pattern_num_ = 0;
	this->now_num_ = 0;

	return true;
}

// Set project patterns, count from 0
bool SensorManager::SetProPicture(int pat_idx) {
	bool status = true;
	// Check parameters
	if (pat_idx >= pattern_num_ || pat_idx < 0) {
		status = false;
  }
  else {
    now_num_ = pat_idx;
    status = pro_device_->PresentPicture(pattern_mats_[now_num_], 150);
  }
	return status;
}

// 获取相机图像
cv::Mat* SensorManager::GetCamColorFrames(int frame_num) {
  return cam_device_->CollectMultiColorFrame(frame_num);
}

cv::Mat* SensorManager::GetCamMonoFrames(int frame_num) {
  return cam_device_->CollectMultiMonoFrame(frame_num);
}

bool SensorManager::ShowCaptureResult(VisualModule* window, int stop_key) {
  return cam_device_->ShowCollectionResult(window, stop_key);
}

// 获取投影仪投影的图像
cv::Mat SensorManager::GetProPicture() {
	return pattern_mats_[now_num_];
}