#ifndef _STATICPARA_H_
#define _STATICPARA_H_

#include <string>
#include <opencv2/opencv.hpp>

// resolution of projector
extern const int kProWidth;
extern const int kProHeight;

// resolution of camera
extern const int kCamWidth;
extern const int kCamHeight;

// bias resolution of computer screen
extern const int kScreenBiasHeight;
extern const int kScreenBiasWidth;

// 格雷码和PhaseShifting的数目
extern const int kVerGrayNum;
extern const int kHorGrayNum;
extern const int kPhaseNum;

// Visualization parameters
extern const int kShowPictureTime;
extern const bool kVisualFlagForDebug;
extern const int kPointPerPixelNum;

// Chess board parameters
extern const int kChessFrameNumber;
extern const int kChessWidth;
extern const int kChessHeight;
extern const int kStereoSize;
struct StereoCalibSet {
  cv::Mat R;
  cv::Mat T;
  cv::Mat E;
  cv::Mat F;
};

// data pathes
extern const std::string kProPatternPath;
extern const std::string kProVerGrayName;
extern const std::string kProVerGrayCodeName;
extern const std::string kProVerGrayCodeSuffix;
extern const std::string kProVerPhaseName;
extern const std::string kProDynaName;
extern const std::string kProWaitName;
extern const std::string kProFileSuffix;
extern const int kCamDeviceNum;

// Used for mats storage
struct CamMatSet {
  cv::Mat * ver_gray;
  cv::Mat * hor_gray;
  cv::Mat * ver_phase;
  cv::Mat * hor_phase;
  cv::Mat * dyna;
  cv::Mat * x_pro;
  cv::Mat * y_pro;
  CamMatSet();
  ~CamMatSet();
};

#endif