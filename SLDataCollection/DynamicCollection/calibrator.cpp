#include "calibrator.h"

//CVisualization myDebug("Debug");

// Set NULL to pointers
Calibrator::Calibrator() {
  sensor_ = NULL;
  cam_mats_ = NULL;
  cam_points_ = NULL;
  tmp_cam_points_ = NULL;
  cam_matrix_ = NULL;
  cam_distor_ = NULL;
  stereo_set_ = NULL;
  cam_visual_ = nullptr;
  pro_visual_ = nullptr;
}

// Release
Calibrator::~Calibrator() {
  ReleaseSpace();
}

// Release the space allocated by pointers
bool Calibrator::ReleaseSpace() {
  if (sensor_ != NULL) {
    delete(sensor_);
    sensor_ = NULL;
  }
  if (cam_mats_ != NULL) {
    delete[](cam_mats_);
    cam_mats_ = NULL;
  }
  if (cam_points_ != NULL) {
    delete[](cam_points_);
    cam_points_ = NULL;
  }
  if (tmp_cam_points_ != NULL) {
    delete[](tmp_cam_points_);
    tmp_cam_points_ = NULL;
  }
  if (cam_matrix_ != NULL) {
    delete[](cam_matrix_);
    cam_matrix_ = NULL;
  }
  if (cam_distor_ != NULL) {
    delete[](cam_distor_);
    cam_distor_ = NULL;
  }
  if (stereo_set_ != NULL) {
    delete[](stereo_set_);
    stereo_set_ = NULL;
  }
  if (cam_visual_ != NULL) {
    delete(cam_visual_);
    cam_visual_ = NULL;
  }
  if (pro_visual_ != NULL) {
    delete(pro_visual_);
    pro_visual_ = NULL;
  }
  return true;
}

// Initialize
bool Calibrator::Init() {
  ReleaseSpace();
  // Sensor
  sensor_ = new SensorManager;
  sensor_->InitSensor();
  pattern_path_ = "Patterns/";
  cam_mats_ = new CamMatSet[kCamDeviceNum];
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    cam_mats_[cam_idx].ver_gray = new cv::Mat[kVerGrayNum * 2];
    cam_mats_[cam_idx].hor_gray = new cv::Mat[kHorGrayNum * 2];
    cam_mats_[cam_idx].ver_phase = new cv::Mat[kPhaseNum];
    cam_mats_[cam_idx].hor_phase = new cv::Mat[kPhaseNum];
    cam_mats_[cam_idx].x_pro = new cv::Mat[kChessFrameNumber];
    cam_mats_[cam_idx].y_pro = new cv::Mat[kChessFrameNumber];
  }
  // Chess points reco
  cam_points_ = new vector<vector<cv::Point2f>>[kCamDeviceNum];
  tmp_cam_points_ = new vector<cv::Point2f>[kCamDeviceNum];
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    vector<vector<cv::Point2f>>().swap(cam_points_[cam_idx]);
  }
  vector<vector<cv::Point2f>>().swap(pro_points_);
  vector<vector<cv::Point3f>>().swap(obj_points_);
  // Result
  cam_matrix_ = new cv::Mat[kCamDeviceNum];
  cam_distor_ = new cv::Mat[kCamDeviceNum];
  stereo_set_ = new StereoCalibSet[kStereoSize];
  cam_visual_ = new VisualModule("CameraShow");
  pro_visual_ = new VisualModule("ProjectorShow");
  return true;
}

// main function for calibration
bool Calibrator::Calibrate() {
  bool status = true;
  cv::Mat * cam_tmp;
  cam_tmp = new cv::Mat[kCamDeviceNum];
  cv::Mat cam_mask(kCamHeight, kCamWidth, CV_8UC1);
  cam_mask.setTo(0);
  // Chess Reco
  for (int frm_idx = 0; frm_idx < kChessFrameNumber; frm_idx++) {
    // set projected pattern as empty
    sensor_->LoadPatterns(1, pattern_path_, "wait", ".bmp");
    sensor_->SetProPicture(0);
    printf("Ready for collection. Press 'y' to continue\n");
    while (true) {
      for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
        cam_tmp[cam_idx] = sensor_->GetCamPicture(cam_idx);
      }
      int key;
      key = cam_visual_->Show(cam_tmp[0], 100, false, 0.5);
      //key = cam_visual_->CombineShow(cam_tmp, 2, 100, cam_mask, 0.5);
      if (key == 'y') {
        break;
      }
    }
    sensor_->UnloadPatterns();
    sensor_->LoadPatterns(1, pattern_path_, "empty", ".png");
    sensor_->SetProPicture(0);
    sensor_->UnloadPatterns();

    // Fill ObjPoint£»
    status = RecoChessPointObj(frm_idx);
    printf("For %dth picture: ObjPoint finished.\n", frm_idx + 1);

    // Fill CamPoint£»
    status = RecoChessPointCam(frm_idx);
    printf("For %dth picture: CamPoint finished.\n", frm_idx + 1);

    // Fill ProPoint£»
    status = RecoChessPointPro(frm_idx);
    // Make sure the recognition result is correct
    // save the corner information and correspondence information
    if (status) {
      printf("For %dth picture: ProPoint finished.\n", frm_idx + 1);
      PushChessPoint(frm_idx);
    } else {
      printf("Invalid pro_point. Discard.\n");
      frm_idx = frm_idx - 1;
    }
  }

  // Calibration
  printf("Begin Calibrating.\n");
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    calibrateCamera(obj_points_,
      cam_points_[cam_idx],
      cv::Size(kCamWidth, kCamHeight),
      cam_matrix_[cam_idx],
      cam_distor_[cam_idx],
      cv::noArray(),
      cv::noArray(),
      cv::CALIB_FIX_K3,
      cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-16));
    printf("Finish cam[%d].\n", cam_idx);
  }
  calibrateCamera(obj_points_,
    pro_points_,
    cv::Size(kProWidth, kProHeight),
    pro_matrix_,
    pro_distor_,
    cv::noArray(),
    cv::noArray(),
    cv::CALIB_FIX_K3,
    cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 1e-16));
  printf("Finish pro.\n");
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    stereoCalibrate(obj_points_,
      cam_points_[cam_idx],
      pro_points_,
      cam_matrix_[cam_idx],
      cam_distor_[cam_idx],
      pro_matrix_,
      pro_distor_,
      cv::Size(kCamWidth, kCamHeight),
      stereo_set_[cam_idx].R,
      stereo_set_[cam_idx].T,
      stereo_set_[cam_idx].E,
      stereo_set_[cam_idx].F,
      256,
      cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 1e-16));
    printf("Finish cam[%d] & pro.\n", cam_idx);
  }
  int ste_idx = kCamDeviceNum;
  for (int c1 = 0; c1 < kCamDeviceNum; c1++) {
    for (int c2 = 1; c2 < kCamDeviceNum; c2++) {
      if (c1 != c2) {
        stereoCalibrate(obj_points_,
          cam_points_[c1],
          cam_points_[c2],
          cam_matrix_[c1],
          cam_distor_[c1],
          cam_matrix_[c2],
          cam_distor_[c2],
          cv::Size(kCamWidth, kCamHeight),
          stereo_set_[ste_idx].R,
          stereo_set_[ste_idx].T,
          stereo_set_[ste_idx].E,
          stereo_set_[ste_idx].F,
          256,
          cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 1e-16));
        printf("Finish cam[%d] & cam[%d].\n", c1, c2);
        ste_idx++;
      }
    }
  }
  delete[]cam_tmp;
  return true;
}

// Output and save
bool Calibrator::Result() {
  // Output individuals
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    stringstream ss;
    ss << cam_idx;
    string idx2str;
    ss >> idx2str;
    FileStorage fs_cam("cam" + idx2str + ".xml", FileStorage::WRITE);
    fs_cam << "cam_matrix" << cam_matrix_[cam_idx];
    fs_cam << "cam_distor" << cam_distor_[cam_idx];
    fs_cam.release();
    fstream file_cam;
    file_cam.open("cam" + idx2str + ".txt", ios::out);
    file_cam << "cam_matrix" << endl;
    file_cam << cam_matrix_[cam_idx] << endl;
    file_cam << "cam_distor" << endl;
    file_cam << cam_distor_[cam_idx] << endl;
    file_cam.close();
  }
  FileStorage fs_pro("pro.xml", FileStorage::WRITE);
  fs_pro << "pro_matrix" << pro_matrix_;
  fs_pro << "pro_distor" << pro_distor_;
  fs_pro.release();
  fstream file_pro;
  file_pro.open("pro.txt", ios::out);
  file_pro << "pro_matrix" << endl;
  file_pro << pro_matrix_ << endl;
  file_pro << "pro_distor" << endl;
  file_pro << pro_distor_ << endl;
  file_pro.close();
  // Stereo parameters
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    stringstream ss;
    ss << cam_idx;
    string idx2str;
    ss >> idx2str;
    FileStorage fs_cam_pro("cam" + idx2str + "_pro.xml", FileStorage::WRITE);
    fs_cam_pro << "rot" << stereo_set_[cam_idx].R;
    fs_cam_pro << "trans" << stereo_set_[cam_idx].T;
    fs_cam_pro.release();
    fstream file_cam_pro;
    file_cam_pro.open("cam" + idx2str + "_pro.txt", ios::out);
    file_cam_pro << "rot" << endl;
    file_cam_pro << stereo_set_[cam_idx].R << endl;
    file_cam_pro << "trans" << endl;
    file_cam_pro << stereo_set_[cam_idx].T << endl;
    file_cam_pro.close();
  }
  int ste_idx = kCamDeviceNum;
  for (int c_1 = 0; c_1 < kCamDeviceNum; c_1++) {
    stringstream ss_1;
    ss_1 << c_1;
    string idx2str_1;
    ss_1 >> idx2str_1;
    for (int c_2 = 1; c_2 < kCamDeviceNum; c_2++) {
      if (c_1 == c_2) {
        continue;
      }
      stringstream ss_2;
      ss_2 << c_2;
      string idx2str_2;
      ss_2 >> idx2str_2;
      FileStorage fs_cam_cam("cam" + idx2str_1 + "_cam" + idx2str_2 + ".xml",
                             FileStorage::WRITE);
      fs_cam_cam << "rot" << stereo_set_[ste_idx].R;
      fs_cam_cam << "trans" << stereo_set_[ste_idx].T;
      fs_cam_cam.release();
      fstream file_cam_cam;
      file_cam_cam.open("cam" + idx2str_1 + "_cam" + idx2str_2 + ".txt", 
                        ios::out);
      file_cam_cam << "rot" << endl;
      file_cam_cam << stereo_set_[ste_idx].R << endl;
      file_cam_cam << "trans" << endl;
      file_cam_cam << stereo_set_[ste_idx].T << endl;
      file_cam_cam.close();
      ste_idx++;
    }
  }
  printf("Calibration Finished.\n");
  return true;
}

// Fill the obj_points
bool Calibrator::RecoChessPointObj(int frameIdx) {
  vector<cv::Point3f>().swap(tmp_obj_points_);
  for (int x = 0; x < kChessWidth; x++) {
    for (int y = 0; y < kChessHeight; y++) {
      tmp_obj_points_.push_back(cv::Point3f(x, y, 0));
    }
  }
  return true;
}

// Reco cam_points and fill in
bool Calibrator::RecoChessPointCam(int frameIdx) {
  bool status = true;
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    vector<cv::Point2f>().swap(tmp_cam_points_[cam_idx]);
  }
  cv::Mat * cam_tmp;
  cam_tmp = new cv::Mat[kCamDeviceNum];
  cv::Mat cam_mask(kCamHeight, kCamWidth, CV_8UC1);
  cam_mask.setTo(0);
  // Set picture
  sensor_->LoadPatterns(1, pattern_path_, "empty", ".png");
  sensor_->SetProPicture(0);
  sensor_->UnloadPatterns();
  while (true) {
    bool reco_flag = true;
    for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
      cam_tmp[cam_idx] = sensor_->GetCamPicture(cam_idx);
    }
    //cam_visual_->CombineShow(cam_tmp, kCamDeviceNum, 100, cam_mask, 0.5);
    cam_visual_->Show(cam_tmp[0], 100, false, 0.5);
    for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
      int max_attempt_times = 1;
      int k = 0;
      int found = 0;
      while (k++ < max_attempt_times) {
        found = findChessboardCorners(
            cam_tmp[cam_idx], cv::Size(kChessHeight, kChessWidth),
            tmp_cam_points_[cam_idx], 
            cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
        /*drawChessboardCorners(
            cam_tmp[cam_idx], Size(kChessHeight, kChessWidth), 
            tmp_cam_points_[cam_idx], found);*/
        pro_visual_->Show(cam_tmp[cam_idx], 100, false, 0.5);
        if (found) {
          printf("Success for cam[%d].\n", cam_idx);
          break;
        } else {
          printf("Failed for cam[%d](%d/%d).\n", cam_idx, k, max_attempt_times);
          reco_flag = false;
        }
      }
      if (!reco_flag) {
        break;
      } else {
        cornerSubPix(
            cam_tmp[cam_idx], tmp_cam_points_[cam_idx], cv::Size(5,5), Size(-1,-1),
            cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1));
        cv::Mat draw_mat;
        cam_tmp[cam_idx].copyTo(draw_mat);
        drawChessboardCorners(
            draw_mat, Size(kChessHeight, kChessWidth),
            tmp_cam_points_[cam_idx], found);
        pro_visual_->Show(draw_mat, 100, false, 0.5);
      }
    }
    if (reco_flag) {
      //cam_visual_->CombineShow(cam_tmp, 2, 100, cam_mask, 0.5);
      cam_visual_->Show(cam_tmp[0], 100, false, 0.5);
      break;
    }
  }
  delete[]cam_tmp;
  return status;
}

// Reco pro_points according to cam[0]
bool Calibrator::RecoChessPointPro(int frameIdx) {
  bool status = true;
  vector<cv::Point2f>().swap(tmp_pro_points_);
  cv::Mat ver_gray_mat, hor_gray_mat, ver_phase_mat, hor_phase_mat;
  cv::Mat ver_pro_mat, hor_pro_mat;
  int kMultiCollectNum = 5;
  cv::Mat tmp_mul_collect;
  cv::Mat tmp_mat;
  cv::Mat temp_total_mat;
  temp_total_mat.create(kCamHeight, kCamWidth, CV_64FC1);
  int ver_period, hor_period;
  int ver_gray_num, hor_gray_num;
  int ver_gray_period, hor_gray_period;

  // Collect
  sensor_->LoadPatterns(kVerGrayNum * 2, pattern_path_, "vGray", ".bmp");
  for (int i = 0; i < kVerGrayNum * 2; i++) {
    sensor_->SetProPicture(i);
    temp_total_mat.setTo(0);
    for (int k = 0; k < kMultiCollectNum; k++) {
      tmp_mul_collect = sensor_->GetCamPicture(0);
      tmp_mul_collect.convertTo(tmp_mat, CV_64FC1);
      temp_total_mat += tmp_mat / kMultiCollectNum;
    }
    temp_total_mat.convertTo(cam_mats_[0].ver_gray[i], CV_8UC1);
  }
  sensor_->UnloadPatterns();
  sensor_->LoadPatterns(kHorGrayNum * 2, pattern_path_, "hGray", ".bmp");
  for (int i = 0; i < kHorGrayNum * 2; i++)	{
    sensor_->SetProPicture(i);
    temp_total_mat.setTo(0);
    for (int k = 0; k < kMultiCollectNum; k++) {
      tmp_mul_collect = sensor_->GetCamPicture(0);
      tmp_mul_collect.convertTo(tmp_mat, CV_64FC1);
      temp_total_mat += tmp_mat / kMultiCollectNum;
    }
    temp_total_mat.convertTo(cam_mats_[0].hor_gray[i], CV_8UC1);
  }
  sensor_->UnloadPatterns();
  sensor_->LoadPatterns(kPhaseNum, pattern_path_, "vPhase", ".bmp");
  for (int i = 0; i < kPhaseNum; i++) {
    sensor_->SetProPicture(i);
    temp_total_mat.setTo(0);
    for (int k = 0; k < kMultiCollectNum; k++) {
      tmp_mul_collect = sensor_->GetCamPicture(0);
      tmp_mul_collect.convertTo(tmp_mat, CV_64FC1);
      temp_total_mat += tmp_mat / kMultiCollectNum;
    }
    temp_total_mat.convertTo(cam_mats_[0].ver_phase[i], CV_8UC1);
  }
  sensor_->UnloadPatterns();
  sensor_->LoadPatterns(kPhaseNum, pattern_path_, "hPhase", ".bmp");
  for (int i = 0; i < kPhaseNum; i++) {
    sensor_->SetProPicture(i);
    temp_total_mat.setTo(0);
    for (int k = 0; k < kMultiCollectNum; k++) {
      tmp_mul_collect = sensor_->GetCamPicture(0);
      tmp_mul_collect.convertTo(tmp_mat, CV_64FC1);
      temp_total_mat += tmp_mat / kMultiCollectNum;
    }
    temp_total_mat.convertTo(cam_mats_[0].hor_phase[i], CV_8UC1);
  }
  sensor_->UnloadPatterns();

  // Decode
  GrayDecoder vgray_decoder;
  vgray_decoder.SetNumDigit(kVerGrayNum, true);
  vgray_decoder.SetCodeFileName(pattern_path_, "vGrayCode.txt");
  for (int i = 0; i < kVerGrayNum * 2; i++) {
    vgray_decoder.SetMat(i, cam_mats_[0].ver_gray[i]);
  }
  vgray_decoder.Decode();
  ver_gray_mat = vgray_decoder.GetResult();
  GrayDecoder hgray_decoder;
  hgray_decoder.SetNumDigit(kHorGrayNum, false);
  hgray_decoder.SetCodeFileName(pattern_path_, "hGrayCode.txt");
  for (int i = 0; i < kHorGrayNum * 2; i++) {
    hgray_decoder.SetMat(i, cam_mats_[0].hor_gray[i]);
  }
  hgray_decoder.Decode();
  hor_gray_mat = hgray_decoder.GetResult();
  PhaseDecoder vphase_decoder;
  ver_period = kProWidth / (1 << kVerGrayNum - 1);
  vphase_decoder.SetNumDigit(kPhaseNum, ver_period);
  for (int i = 0; i < kPhaseNum; i++) {
    vphase_decoder.SetMat(i, cam_mats_[0].ver_phase[i]);
  }
  vphase_decoder.Decode();
  ver_phase_mat = vphase_decoder.GetResult();
  PhaseDecoder hphase_decoder;
  hor_period = kProHeight / (1 << kHorGrayNum - 1);
  hphase_decoder.SetNumDigit(kPhaseNum, hor_period);
  for (int i = 0; i < kPhaseNum; i++) {
    hphase_decoder.SetMat(i, cam_mats_[0].hor_phase[i]);
  }
  hphase_decoder.Decode();
  hor_phase_mat = hphase_decoder.GetResult();

  // Combine
  //pro_visual.Show(ver_gray_mat, 0, true, 0.5);
  //pro_visual.Show(hor_gray_mat, 0, true, 0.5);
  //pro_visual.Show(ver_phase_mat, 0, true, 0.5);
  //pro_visual.Show(hor_phase_mat, 0, true, 0.5);
  ver_gray_num = 1 << kVerGrayNum;
  ver_gray_period = kProWidth / ver_gray_num;
  for (int h = 0; h < kCamHeight; h++) {
    for (int w = 0; w < kCamWidth; w++) {
      double gray_val = ver_gray_mat.at<double>(h, w);
      double phase_val = ver_phase_mat.at<double>(h, w);
      if (gray_val < 0) {
        ver_phase_mat.at<double>(h, w) = 0;
        continue;
      }
      if ((int)(gray_val / ver_gray_period) % 2 == 0) {
        if (phase_val >(double)ver_period * 0.75) {
          ver_phase_mat.at<double>(h, w) = phase_val - ver_period;
        }
      }
      else {
        if (phase_val < (double)ver_period * 0.25) {
          ver_phase_mat.at<double>(h, w) = phase_val + ver_period;
        }
        ver_phase_mat.at<double>(h, w) =
          ver_phase_mat.at<double>(h, w) - 0.5 * ver_period;
      }
    }
  }
  cam_mats_[0].x_pro[0] = ver_gray_mat + ver_phase_mat;
  hor_gray_num = 1 << kHorGrayNum;
  hor_gray_period = kProHeight / hor_gray_num;
  //my_debug_->Show(tmp_gray_mat, 0, true, 0.5);
  //my_debug_->Show(tmp_phase_mat, 0, true, 0.5);
  for (int h = 0; h < kCamHeight; h++) {
    for (int w = 0; w < kCamWidth; w++) {
      double gray_val = hor_gray_mat.at<double>(h, w);
      double phase_val = hor_phase_mat.at<double>(h, w);
      if (gray_val < 0) {
        hor_phase_mat.at<double>(h, w) = 0;
        continue;
      }
      if ((int)(gray_val / hor_gray_period) % 2 == 0) {
        if (phase_val >(double)hor_period * 0.75) {
          hor_phase_mat.at<double>(h, w) = phase_val - hor_period;
        }
      }
      else {
        if (phase_val < (double)hor_period * 0.25) {
          hor_phase_mat.at<double>(h, w) = phase_val + hor_period;
        }
        hor_phase_mat.at<double>(h, w) =
          hor_phase_mat.at<double>(h, w) - 0.5 * hor_period;
      }
    }
  }
  cam_mats_[0].y_pro[0] = hor_gray_mat + hor_phase_mat;
  for (int h = 1; h < kCamHeight - 1; h++) {
    for (int w = 1; w < kCamWidth - 1; w++) {
      double x_pro_val = cam_mats_[0].x_pro[0].at<double>(h, w);
      if (x_pro_val < 0) {
        double x_up_val = cam_mats_[0].x_pro[0].at<double>(h - 1, w);
        double x_dn_val = cam_mats_[0].x_pro[0].at<double>(h + 1, w);
        if ((x_up_val > 0) && (x_dn_val > 0)) {
          cam_mats_[0].x_pro[0].at<double>(h, w) = (x_up_val + x_dn_val) / 2;
        }
      }
      double y_pro_val = cam_mats_[0].y_pro[0].at<double>(h, w);
      if (y_pro_val < 0) {
        double y_lf_val = cam_mats_[0].y_pro[0].at<double>(h, w - 1);
        double y_rt_val = cam_mats_[0].y_pro[0].at<double>(h, w + 1);
        if ((y_lf_val > 0) && (y_rt_val > 0)) {
          cam_mats_[0].y_pro[0].at<double>(h, w) = (y_lf_val + y_rt_val) / 2;
        }
      }
    }
  }
  //pro_visual.Show(cam_mats_[0].x_pro[0], 0, true, 0.5);
  //pro_visual.Show(cam_mats_[0].y_pro[0], 0, true, 0.5);

  // projector coord
  vector<Point2f>::iterator i;
  for (i = tmp_cam_points_[0].begin(); i != tmp_cam_points_[0].end(); ++i) {
    Point2f cam = *i;
    Point2f pro;
    int X = cam.x;
    int Y = cam.y;
    pro.x = cam_mats_[0].x_pro[0].at<double>(Y, X);
    if ((pro.x < 0) && (X > 0) && (X < kCamWidth - 1)) {
      float lf_val = cam_mats_[0].x_pro[0].at<double>(Y, X - 1);
      float rt_val = cam_mats_[0].x_pro[0].at<double>(Y, X + 1);
      if ((lf_val > 0) && (rt_val > 0)) {
        pro.x = (lf_val + rt_val) / 2;
      }
    }
    pro.y = cam_mats_[0].y_pro[0].at<double>(Y, X);
    if ((pro.y < 0) && (Y > 0) && (Y < kCamHeight - 1)) {
      float up_val = cam_mats_[0].y_pro[0].at<double>(Y - 1, X);
      float dn_val = cam_mats_[0].y_pro[0].at<double>(Y + 1, X);
      if ((up_val > 0) && (dn_val > 0)) {
        pro.y = (up_val + dn_val) / 2;
      }
    }
    tmp_pro_points_.push_back(pro);
  }
  // Draw it
  Mat draw_mat;
  draw_mat.create(800, 1280, CV_8UC1);
  draw_mat.setTo(0);
  drawChessboardCorners(
      draw_mat, Size(kChessHeight, kChessWidth), tmp_pro_points_, true);
  int key = pro_visual_->Show(draw_mat, 0, false, 0.5);
  if (key == 'y') {
    status = true;
  } else {
    status = false;
  }

  return status;
}

// Save chess points
bool Calibrator::PushChessPoint(int frameIdx) {
  bool status = true;
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    cam_points_[cam_idx].push_back(tmp_cam_points_[cam_idx]);
  }
  obj_points_.push_back(tmp_obj_points_);
  pro_points_.push_back(tmp_pro_points_);
  return status;
}