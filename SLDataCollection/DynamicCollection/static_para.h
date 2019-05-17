#ifndef _STATICPARA_H_
#define _STATICPARA_H_

#include <string>
#include <opencv2/opencv.hpp>

// Resolutions of projector and camera
const int kCamDeviceNum = 1;
const int kProWidth = 1280;
const int kProHeight = 800;
const int kCamWidth = 1280;
const int kCamHeight = 1024;

// bias resolution of computer screen
const int kScreenBiasHeight = 0;
const int kScreenBiasWidth = 1920;

// GrayCode & PhaseShifting parameters
const int kVerGrayNum = 6;
const int kHorGrayNum = 5;
const int kPhaseNum = 4;

// Visualization parameters
const int kShowPictureTime = 500;
const bool kVisualFlagForDebug = false;
const int kPointPerPixelNum = 10;

// Chess board parameters
const int kChessFrameNumber = 15;
const int kChessWidth = 11;
const int kChessHeight = 8;
const float kChessSquareLen = 10; // mm
const int kStereoSize = kCamDeviceNum + kCamDeviceNum*(kCamDeviceNum - 1) / 2;
struct StereoCalibSet {
  cv::Mat R;
  cv::Mat T;
  cv::Mat E;
  cv::Mat F;
};

// data pathes
const std::string kProPatternPath = "Patterns/";
const std::string kProVerGrayName = "vGray";
const std::string kProVerGrayCodeName = "vGrayCode";
const std::string kProVerGrayCodeSuffix = ".txt";
const std::string kProVerPhaseName = "vPhase";
const std::string kProDynaName = "dynaMat";
const std::string kProWaitName = "wait";
const std::string kProFileSuffix = ".bmp";


// Used for mats storage
struct CamMatSet {
  cv::Mat * ver_gray;
  cv::Mat * hor_gray;
  cv::Mat * ver_phase;
  cv::Mat * hor_phase;
  cv::Mat * dyna;
  cv::Mat * x_pro;
  cv::Mat * y_pro;
  CamMatSet() {
    this->ver_gray = NULL;
    this->hor_gray = NULL;
    this->ver_phase = NULL;
    this->hor_phase = NULL;
    this->dyna = NULL;
    this->x_pro = NULL;
    this->y_pro = NULL;
  }
  ~CamMatSet() {
    if (this->ver_gray != NULL) {
      delete[] this->ver_gray;
      this->ver_gray = NULL;
    }
    if (this->hor_gray != NULL) {
      delete[] this->hor_gray;
      this->hor_gray = NULL;
    }
    if (this->ver_phase != NULL) {
      delete[] this->ver_phase;
      this->ver_phase = NULL;
    }
    if (this->hor_phase != NULL) {
      delete[] this->hor_phase;
      this->hor_phase = NULL;
    }
    if (this->dyna != NULL) {
      delete[] this->dyna;
      this->dyna = NULL;
    }
    if (this->x_pro != NULL) {
      delete[] this->x_pro;
      this->x_pro = NULL;
    }
    if (this->y_pro != NULL) {
      delete[] this->y_pro;
      this->y_pro = NULL;
    }
  }
};

#endif