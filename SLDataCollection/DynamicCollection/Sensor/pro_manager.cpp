#include "pro_manager.h"

// Constructor function
ProManager::ProManager() {
	this->pattern_height_ = kProHeight;
	this->pattern_width_ = kProWidth;
	this->bias_height_ = kScreenBiasHeight;
	this->bias_width_ = kScreenBiasWidth;
	this->win_name_ = "ProWindow";
}

// Destruction funtion
ProManager::~ProManager() {

}

// Intital projector. Create an opencv window at position.
bool ProManager::InitProjector() {
	// Create window and set position
	cv::namedWindow(this->win_name_, cv::WINDOW_NORMAL);
  cv::setWindowProperty(this->win_name_, cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
  cv::resizeWindow(this->win_name_, this->pattern_width_, this->pattern_height_);
  cv::moveWindow(this->win_name_, this->bias_width_, this->bias_height_);
	return true;
}

// Destory window.
bool ProManager::CloseProjector() {
  cv::destroyWindow(this->win_name_);
	return true;
}

// make projector to project <pic> for <time>ms.
bool ProManager::PresentPicture(cv::Mat pic, int time) {
  cv::imshow(this->win_name_, pic);
  cv::waitKey(time);
	return true;
}

bool ProManager::PresentPicture(uchar x, int time)
{
  cv::Mat pic;
	pic.create(this->pattern_height_, this->pattern_width_, CV_8UC1);
	pic.setTo(x);
  cv::imshow(this->win_name_, pic);
  cv::waitKey(time);
	return true;
}