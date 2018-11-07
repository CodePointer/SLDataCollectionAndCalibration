#include "cam_manager.h"

// Constructor function:
// Set initial parameter values
CamManager::CamManager() {
  device_num_ = 0;
  is_valid_ = false;
  ptr_system_ = NULL;
}

// Destructor function:
CamManager::~CamManager() {
  
}

// get total camera number
int CamManager::get_device_num() {
  return device_num_;
}

// if camera is valid
bool CamManager::IsValid() {
  return is_valid_;
}

// Initial devices.
// Open the camera and set some parameters.
bool CamManager::InitCamera() {
  // Get cameras
  ptr_system_ = Spinnaker::System::GetInstance();
  cam_list_ = ptr_system_->GetCameras();
  if (cam_list_.GetSize() != 1) {
    return false;
  }
  
  Spinnaker::CameraPtr ptr_cam = cam_list_.GetByIndex(0);
  ptr_cam->Init();
  

  return true;
}

cv::Mat* CamManager::CollectMultiColorFrame(int frame_num) {
  if (frame_num <= 0) {
    return NULL;
  }
  cv::Mat* multi_result = new cv::Mat[frame_num];
  Spinnaker::CameraPtr ptr_cam = cam_list_.GetByIndex(0);
  ptr_cam->PixelFormat.SetValue(Spinnaker::PixelFormat_RGB8);
  ptr_cam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_MultiFrame);
  ptr_cam->AcquisitionFrameCount.SetValue(frame_num);

  int k = 0;
  ptr_cam->BeginAcquisition();
  while (k < frame_num) {
    Spinnaker::ImagePtr raw_img = ptr_cam->GetNextImage();
    if (raw_img->IsIncomplete()) {
      std::cerr << "Incomplete image at " << k << std::endl;
      continue;
    }
    Spinnaker::ImagePtr converted_img = raw_img->Convert(Spinnaker::PixelFormat_BGR8);
    cv::Mat tmp_mat(kCamHeight, kCamWidth, CV_8UC3, converted_img->GetData());
    tmp_mat.copyTo(multi_result[k]);
    raw_img->Release();
    k++;
    //std::cout << k << std::endl;
  }
  ptr_cam->EndAcquisition();
  return multi_result;
}

cv::Mat* CamManager::CollectMultiMonoFrame(int frame_num) {
  if (frame_num <= 0) {
    return NULL;
  }

  // Set options: MultiFrame, Mono8
  cv::Mat* multi_result = new cv::Mat[frame_num];
  Spinnaker::CameraPtr ptr_cam = cam_list_.GetByIndex(0);
  ptr_cam->PixelFormat.SetValue(Spinnaker::PixelFormat_RGB8);
  ptr_cam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_MultiFrame);
  ptr_cam->AcquisitionFrameCount.SetValue(frame_num);

  int k = 0;
  ptr_cam->BeginAcquisition();
  while (k < frame_num) {
    Spinnaker::ImagePtr raw_img = ptr_cam->GetNextImage();
    if (raw_img->IsIncomplete()) {
      std::cerr << "Incomplete image at " << k << std::endl;
      continue;
    }
    Spinnaker::ImagePtr converted_img = raw_img->Convert(Spinnaker::PixelFormat_Mono8);
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
  ptr_cam->PixelFormat.SetValue(Spinnaker::PixelFormat_RGB8);
  ptr_cam->AcquisitionMode.SetValue(Spinnaker::AcquisitionMode_Continuous);
  
  ptr_cam->BeginAcquisition();
  while (true) {
    Spinnaker::ImagePtr raw_img = ptr_cam->GetNextImage();
    if (raw_img->IsIncomplete()) {
      std::cerr << "Incomplete image" << std::endl;
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