// ================================================================= //
// @File: cam_manager.cpp
// @Author: Qiao Rukun
// @Date: 2019.05.15
// @LastEditTime: 2019.05.15
// @LastEditors: Qiao Rukun
// ================================================================= //

// @brief ref to cam_manager.h.

#include "cam_manager.h"

// Constructor function:
// Set initial parameter values
CamManager::CamManager() {
  device_num_ = 0;
  is_valid_ = false;
  ptr_system_ = NULL;
}

// Destructor function:
// Delete used pointers.
CamManager::~CamManager() {
  
}

// Get the total number of camera. Useful if you have multiple cameras.
int CamManager::device_num() {
  return device_num_;
}

bool CamManager::is_valid() {
  return is_valid_;
}

// Initial devices.
// Open the camera and set some parameters.
bool CamManager::InitCamera() {
  // Get cameras
  ptr_system_ = System::GetInstance();
  cam_list_ = ptr_system_->GetCameras();
  if (cam_list_.GetSize() != 1) {
    return false;
  }
  device_num_ = cam_list_.GetSize();
  
  CameraPtr ptr_cam = cam_list_.GetByIndex(0);
  ptr_cam->Init();
  
  // Set options: Mono8, Multi frame
  INodeMap & node_map = ptr_cam->GetNodeMap();
  // Pixel format: mono
  CEnumerationPtr ptr_pixel_format = node_map.GetNode("PixelFormat");
  CEnumEntryPtr ptr_pixel_mono8 = ptr_pixel_format->GetEntryByName("Mono8");
  ptr_pixel_format->SetIntValue(ptr_pixel_mono8->GetValue());
  // Acquisition mode: MultiFrame
  CEnumerationPtr ptr_acquisition_mode = node_map.GetNode("AcquisitionMode");
  CEnumEntryPtr ptr_mode_multi = ptr_acquisition_mode->GetEntryByName("MultiFrame");
  ptr_acquisition_mode->SetIntValue(ptr_mode_multi->GetValue());
  

  return true;
}

//cv::Mat* CamManager::CollectMultiColorFrame(int frame_num) {
//  if (frame_num <= 0) {
//    return NULL;
//  }
//  cv::Mat* multi_result = new cv::Mat[frame_num];
//  Spinnaker::CameraPtr ptr_cam = cam_list_.GetByIndex(0);
//  ptr_cam->PixelFormat.SetValue(Spinnaker::PixelFormat_RGB8);
//  ptr_cam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_MultiFrame);
//  ptr_cam->AcquisitionFrameCount.SetValue(frame_num);
//
//  int k = 0;
//  ptr_cam->BeginAcquisition();
//  while (k < frame_num) {
//    Spinnaker::ImagePtr raw_img = ptr_cam->GetNextImage();
//    if (raw_img->IsIncomplete()) {
//      std::cerr << "Incomplete image at " << k << std::endl;
//      continue;
//    }
//    Spinnaker::ImagePtr converted_img = raw_img->Convert(Spinnaker::PixelFormat_BGR8);
//    cv::Mat tmp_mat(kCamHeight, kCamWidth, CV_8UC3, converted_img->GetData());
//    tmp_mat.copyTo(multi_result[k]);
//    raw_img->Release();
//    k++;
//    //std::cout << k << std::endl;
//  }
//  ptr_cam->EndAcquisition();
//  return multi_result;
//}

cv::Mat* CamManager::CollectMultiFrame(int frame_num) {
  if (frame_num <= 0) {
    return NULL;
  }

  // Set options: MultiFrame, Mono8
  cv::Mat* multi_result = new cv::Mat[frame_num];
  CameraPtr ptr_cam = cam_list_.GetByIndex(0);
  /*INodeMap & node_map = ptr_cam->GetNodeMap();
  CEnumerationPtr ptr_frame_count = node_map.GetNode("AcquisitionFrameCount");
  ptr_frame_count->SetIntValue(frame_num);*/
  ptr_cam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_MultiFrame);
  ptr_cam->AcquisitionFrameCount.SetValue(frame_num);

  int k = 0;
  ptr_cam->BeginAcquisition();
  while (k < frame_num) {
    ImagePtr raw_img = ptr_cam->GetNextImage();
    if (raw_img->IsIncomplete()) {
      std::cerr << "Incomplete image at " << k << std::endl;
      continue;
    }
    ImagePtr converted_img = raw_img->Convert(PixelFormat_Mono8, HQ_LINEAR);
    cv::Mat tmp_mat(kCamHeight, kCamWidth, CV_8UC1, converted_img->GetData());
    tmp_mat.copyTo(multi_result[k]);
    raw_img->Release();
    k++;
  }
  ptr_cam->EndAcquisition();
  return multi_result;
}

bool CamManager::ShowCollectionResult(VisualModule * window, int stop_key) {
  if (window == NULL) {
    return false;
  }

  Spinnaker::CameraPtr ptr_cam = cam_list_.GetByIndex(0);
  ptr_cam->PixelFormat.SetValue(Spinnaker::PixelFormat_Mono8);
  ptr_cam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_Continuous);
  
  ptr_cam->BeginAcquisition();
  while (true) {
    //Sleep(300);
    Spinnaker::ImagePtr raw_img = ptr_cam->GetNextImage();
    if (raw_img->IsIncomplete()) {
      std::cerr << "Incomplete image on show" << std::endl;
      continue;
    }
    Spinnaker::ImagePtr converted_img = raw_img->Convert(Spinnaker::PixelFormat_BGR8);
    cv::Mat tmp_mat(kCamHeight, kCamWidth, CV_8UC3, converted_img->GetData());
    int key = window->Show(tmp_mat, 2, false, 1.0);
    raw_img->Release();
    if (key == stop_key) {
      break;
    }
  }
  ptr_cam->EndAcquisition();

  return true;
}

bool CamManager::CloseCamera() {
  cam_list_.Clear();
  ptr_system_->ReleaseInstance();
  return true;
}