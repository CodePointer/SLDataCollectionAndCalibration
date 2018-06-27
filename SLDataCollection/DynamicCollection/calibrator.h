#ifndef _CALIBRATOR_H_
#define _CALIBRATOR_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <sstream>
#include "Sensor/sensor_manager.h"
#include "Decoder/gray_decoder.h"
#include "Decoder/phase_decoder.h"
#include "Support/visual_mod.h"
#include "global_fun.h"
#include "static_para.h"

// Used for calibration for system.
class Calibrator {
private:
  // Sensor
  SensorManager * sensor_;
  std::string pattern_path_;
  CamMatSet * cam_mats_;
  // Chess points reco
  std::vector<std::vector<cv::Point2f>> * cam_points_;
  std::vector<cv::Point2f> * tmp_cam_points_;
  std::vector<std::vector<cv::Point2f>> pro_points_;
  std::vector<cv::Point2f> tmp_pro_points_;
  std::vector<std::vector<cv::Point3f>> obj_points_;
  std::vector<cv::Point3f> tmp_obj_points_;
  // Result
  cv::Mat * cam_matrix_;
  cv::Mat * cam_distor_;
  cv::Mat pro_matrix_;
  cv::Mat pro_distor_;
  int stereo_size_;
  StereoCalibSet * stereo_set_;
  // Visual
  VisualModule * cam_visual_;
  VisualModule * pro_visual_;

  bool ReleaseSpace();			// �ͷſռ�
  bool RecoChessPointObj(int frameIdx);		// ���m_objPoint
  bool RecoChessPointCam(int frameIdx);		// ���m_camPoint
  bool RecoChessPointPro(int frameIdx);		// ���m_proPoint
  bool PushChessPoint(int frameIdx);			// �������̸񣬴洢������
public:
  Calibrator();
  ~Calibrator();
  bool Init();				// ��ʼ��
  bool Calibrate();			// �궨��������
  bool Result();
};

#endif