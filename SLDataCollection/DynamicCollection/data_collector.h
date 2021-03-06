// ================================================================= //
// @File: data_collector.h data_collector.cpp
// @Author: Qiao Rukun
// @Date: 2019.05.16
// @LastEditTime: 2019.05.16
// @LastEditors: Qiao Rukun
// ================================================================= //

// @brief the data_collector.h/cpp is a general class for data collection.
// The file acquires images from a sensor, processes the raw data to required format.
// In general, the class can finished static decoding(by graycode & phase shifting),
// collect dynamic scene frames, and finish calibration part.
// All need you need to do is creating a DataCollector and call functions.

#ifndef _DATACOLLECTOR_H_
#define _DATACOLLECTOR_H_

#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
#include <vector>
#include "static_para.h"
#include "global_fun.h"
#include "Sensor/sensor_manager.h"
#include "Decoder/gray_decoder.h"
#include "Decoder/phase_decoder.h"
#include "Support/visual_mod.h"
#include "Support/storage_mod.h"

// The data collector of program. Finished all related operation about data collection.
//
// In general, the data collector is used for 3 types of data collection: calibration,
// dynamic frames, static frames. The class can also decode the static frames and get the 
// correspondence information from graycode & phaseshifting process.
//
// Example:
//    DataCollector dc = DataCollector();
//    status = dc.Init(dynamic_flag=true);
//    ......
//    status = dc.CollectDynamicData();
//    or
//    status = dc.CollectStatData();
//    or
//    status = dc.CalibrateSystem();
//    ......
//    dc.Close();
// Tips:
//    None.
class DataCollector {
private:
  // Sensor part
	SensorManager * sensor_manager_;
  // pattern paths
  std::string pattern_path_;
  std::string vgray_name_;
  std::string hgray_name_;
  std::string gray_suffix_;
  std::string vgray_code_name_;
  std::string hgray_code_name_;
  std::string gray_code_suffix_;
  std::string vphase_name_;
  std::string hphase_name_;
  std::string phase_suffix_;
  std::string dyna_name_;
  std::string dyna_suffix_;
  std::string empty_name_;
  std::string empty_suffix_;
  std::string wait_name_;
  std::string wait_suffix_;
  // storage data
  CamMatSet * cam_mats_;
  cv::Mat cam_mask_;
	// storage paths
  std::string save_data_path_;
  std::string dyna_frame_path_;
  std::string dyna_frame_name_;
  std::string dyna_frame_suffix_;
  std::string pro_frame_path_;
  std::string xpro_frame_name_;
  std::string ypro_frame_name_;
  std::string pro_frame_suffix_;
	// visualization
	VisualModule * cam_view_;
  VisualModule * res_view_;
  // Calibration parts
  cv::Mat cam_image_;
  cv::Mat pro_image_;
  std::vector<std::vector<cv::Point2f>> cam_points_;
  std::vector<cv::Point2f> tmp_cam_points_;
  std::vector<std::vector<cv::Point2f>> pro_points_;
  std::vector<cv::Point2f> tmp_pro_points_;
  std::vector<std::vector<cv::Point3f>> obj_points_;
  std::vector<cv::Point3f> tmp_obj_points_;
  cv::Mat cam_matrix_;
  cv::Mat cam_distor_;
  cv::Mat pro_matrix_;
  cv::Mat pro_distor_;
  StereoCalibSet stereo_set_;

	// flag parameters
	bool visualize_flag_;
  bool dynamic_flag_;
	bool storage_flag_;
	int max_frame_num_;
	
	bool GetInputSignal();
  bool DetectChessBoard();
	bool CollectStaticFrame(int frameNum);
  cv::Mat GetRobustCameraFrame(int multi_capture);
	bool CollectDynamicFrame();
	bool DecodeSingleFrame(int frameNum);
  bool FillChessBoard(int frameNum);
	bool VisualizationForDynamicScene(int total_frame_num);
  bool VisualizationForStaticScene(int frame_idx);
  bool VisualizationForCalibration(int frame_idx);
  bool StorageDataByGroup();
  bool StorageDataByFrame(int frame_idx);
  bool PushChessBoardInfo();
  bool CalibrateFromChessBoard();

public:
	DataCollector();
	~DataCollector();
	bool Init(bool dynamic_flag);
	bool CollectDynaData();
  bool CollectStatData();
  bool CalibrateSystem();
	bool Close();
};

#endif