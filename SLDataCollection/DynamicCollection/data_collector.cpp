#include "data_collector.h"

DataCollector::DataCollector() {
	this->sensor_manager_ = NULL;
  this->cam_mats_ = NULL;
	this->cam_view_ = NULL;
  this->res_view_ = NULL;
  this->visualize_flag_ = false;
  this->storage_flag_ = false;
  this->max_frame_num_ = 0;
	return;
}

DataCollector::~DataCollector() {
	if (this->sensor_manager_ != NULL) {
		delete this->sensor_manager_;
		this->sensor_manager_ = NULL;
	}
  if (this->cam_mats_ != NULL) {
    delete[] this->cam_mats_;
    this->cam_mats_ = NULL;
  }
	if (this->cam_view_ != NULL) {
		delete this->cam_view_;
		this->cam_view_ = NULL;
	}
  if (this->res_view_ != NULL) {
    delete this->res_view_;
    this->res_view_ = NULL;
  }
	return;
}

// For initialization; set parameters and create new object
bool DataCollector::Init(bool dynamic_flag) {
	bool status = true;

	// flags
	max_frame_num_ = 600;
	visualize_flag_ = true;
	storage_flag_ = true;
  dynamic_flag_ = dynamic_flag;

	// pattern paths
	pattern_path_ = "Patterns/";
	vgray_name_ = "vGray";
	hgray_name_ = "hGray";
	gray_suffix_ = ".png";
	vgray_code_name_ = "vGrayCode";
	hgray_code_name_ = "hGrayCode";
	gray_code_suffix_ = ".txt";
	vphase_name_ = "vPhase";
	hphase_name_ = "hPhase";
	phase_suffix_ = ".png";
	dyna_name_ = "pattern_T36SD";
	dyna_suffix_ = ".png";
  empty_name_ = "empty";
  empty_suffix_ = ".png";
	wait_name_ = "pattern_T36SD";
	wait_suffix_ = ".png";

	// storage paths
	save_data_path_ = "E:/SLDataSet/20180626/";
	dyna_frame_path_ = "dyna/";
	dyna_frame_name_ = "dyna_mat";
	dyna_frame_suffix_ = ".png";
	pro_frame_path_ = "pro/";
	xpro_frame_name_ = "xpro_mat";
	ypro_frame_name_ = "ypro_mat";
	pro_frame_suffix_ = ".png";

	// Create folder for storage
	StorageModule storage;
	storage.CreateFolder(save_data_path_);
	//storage.CreateFolder(this->save_data_path_ + "1/" + this->dyna_frame_path_);
	//storage.CreateFolder(this->save_data_path_ + "1/" + this->pro_frame_path_);

	// Initialize sensor manager
	if (status)	{
		this->sensor_manager_ = new SensorManager;
		status = this->sensor_manager_->InitSensor();
		if (!status) {
			ErrorHandling("DataCollector::Init(), InitSensor failed.");
		}
	}

	// Allocate space
  this->cam_mats_ = new CamMatSet[kCamDeviceNum];
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    this->cam_mats_[cam_idx].ver_gray = new Mat[kVerGrayNum * 2];
    this->cam_mats_[cam_idx].hor_gray = new Mat[kHorGrayNum * 2];
    this->cam_mats_[cam_idx].ver_phase = new Mat[kPhaseNum];
    this->cam_mats_[cam_idx].hor_phase = new Mat[kPhaseNum];
    if (dynamic_flag) {
      this->cam_mats_[cam_idx].dyna = NULL;
      this->cam_mats_[cam_idx].x_pro = new cv::Mat[1];
      this->cam_mats_[cam_idx].y_pro = new cv::Mat[1];
    } else {
      this->cam_mats_[cam_idx].dyna = new Mat[this->max_frame_num_];
      this->cam_mats_[cam_idx].x_pro = new Mat[this->max_frame_num_];
      this->cam_mats_[cam_idx].y_pro = new Mat[this->max_frame_num_];
    }
  }
	this->cam_view_ = new VisualModule("Camera");
  this->res_view_ = new VisualModule("Result");

	return status;
}

bool DataCollector::CollectDynaData() {
	bool status = true;
  printf("Begin collection.\n");

  // 0. Wait for signal.
  // Adjust object position.
  if (status) {
    printf("Wait for signal...");
    status = GetInputSignal();
    Sleep(3000);
    printf("captured.\n");
  }
  
  // 1. collect frame_0 image
  // frame_0 should have the depth value	
  // Use gray_code and phase shifting
  if (status) {
    printf("Collecting [%d] static images...", 0);
    status = CollectStaticFrame(0);
    printf("finished.\n");
  }
  if (status) {
    printf("Decoding [%d] static images...", 0);
    status = DecodeSingleFrame(0);
    printf("finished.\n");
  }

  // 2. collect dynamic frame data.
  if (status) {
    printf("Begin dynamic frame collection...");
    status = CollectDynamicFrame();
    printf("finished.\n");
  }

  // 3. Show result to see data value
  if (status) {
    status = VisualizationForStaticScene(0);
    status = VisualizationForDynamicScene(max_frame_num_);
  }

  // 4. Storage result to files.
  if (status) {
    status = StorageDataByFrame(0);
    status = StorageDataByGroup();
  }

	return status;
}

bool DataCollector::CollectStatData() {
  bool status = true;

  printf("Begin collection.\n");
  StorageModule storage;

  // For each frames:
  for (int frm_idx = 0; frm_idx < max_frame_num_; frm_idx++) {
    // 0. Wait for signal.
    // Adjust object position.
    if (status) {
      printf("Wait for signal...");
      status = GetInputSignal();
      printf("captured.\n");
    }

    // 1. collect frame_0 image
    // frame_0 should have the depth value	
    // Use gray_code and phase shifting
    if (status) {
      printf("Collecting [%d] static images...", frm_idx);
      status = CollectStaticFrame(frm_idx);
      printf("finished.\n");
    }
    if (status) {
      printf("Decoding [%d] static images...", frm_idx);
      status = DecodeSingleFrame(frm_idx);
      printf("finished.\n");
    }

    // 3. Show result to see data value
    if (status) {
      status = VisualizationForStaticScene(frm_idx);
    }

    // 4. Storage result to files.
    if (status) {
      status = StorageDataByFrame(frm_idx);
    }
  }

  return status;
}

bool DataCollector::CalibrateSystem() {
  bool status = true;

  printf("Begin collection.\n");
  StorageModule storage;
  std::vector<std::vector<cv::Point2f>>().swap(cam_points_);
  std::vector<std::vector<cv::Point2f>>().swap(pro_points_);
  std::vector<std::vector<cv::Point3f>>().swap(obj_points_);
  std::vector<cv::Point3f>().swap(tmp_obj_points_);
  for (int x = 0; x < kChessWidth; x++) {
    for (int y = 0; y < kChessHeight; y++) {
      tmp_obj_points_.push_back(cv::Point3f(x, y, 0));
    }
  }

  // For each frames:
  int valid_num = 0;
  int total_frames = 10;

  GetInputSignal();

  for (int frm_idx = 0; frm_idx < max_frame_num_; frm_idx++) {
    // 0. Wait for signal.
    // Adjust object position.
    if (status) {
      printf("Detecting boards[valid=%d]...", valid_num);
      status = DetectChessBoard();
      printf("captured.\n");
    }

    // 1. collect frame_0 image
    // frame_0 should have the depth value	
    // Use gray_code and phase shifting
    if (status) {
      printf("Collecting [%d] static images...", frm_idx);
      status = CollectStaticFrame(frm_idx);
      printf("finished.\n");
    }
    if (status) {
      printf("Decoding [%d] static images...", frm_idx);
      status = DecodeSingleFrame(frm_idx);
      printf("finished.\n");
    }

    // 2. Fill ChessBoards from collected data
    if (status) {
      printf("Filling chess boards [%d]...", frm_idx);
      status = FillChessBoard(frm_idx);
      printf("finished.\n");
    }

    // 3. Show result to see data value
    //   Storage result to files.
    if (status) {
      status = VisualizationForCalibration(frm_idx);
      if (!status) {
        status = true;
      } else {
        printf("Success.\n");
        status = PushChessBoardInfo();
        valid_num++;
      }
    }
    
    if (valid_num >= total_frames) {
      break;
    }
  }

  // Calibrate result
  status = CalibrateFromChessBoard();

  return status;
}

bool DataCollector::GetInputSignal() {
  // Set projector pattern as wait
  bool status = true;
  if (status) {
    status = sensor_manager_->LoadPatterns(1, pattern_path_, wait_name_, wait_suffix_);
    //status = sensor_manager_->LoadPatterns(1, pattern_path_, vphase_name_, wait_suffix_);
  }
  if (status) {
    status = sensor_manager_->SetProPicture(0);
  }

  // Show camera image on visualization module
  if (status) {
    status = sensor_manager_->ShowCaptureResult(cam_view_, ' ');
  }
  if (status) {
    this->sensor_manager_->UnloadPatterns();
  }
  return status;
}

bool DataCollector::DetectChessBoard() {
  // Set projector pattern as wait
  bool status = true;
  int kMultiCapture = 1;
  std::vector<cv::Point2f>().swap(tmp_cam_points_);
  if (status) {
    status = sensor_manager_->LoadPatterns(1, pattern_path_, 
        empty_name_, empty_suffix_);
  }
  if (status) {
    status = sensor_manager_->SetProPicture(0);
  }

  // Capture image and check
  if (status) {
    while (true) {
      cv::Mat captured = GetRobustCameraFrame(kMultiCapture);
      cam_view_->Show(captured, 10, false, 0.5);
      int max_attempt_times = 2;
      int k = 0;
      while (k < max_attempt_times) {
        int found = cv::findChessboardCorners(captured,
          cv::Size(kChessHeight, kChessWidth),
          tmp_cam_points_,
          cv::CALIB_CB_ADAPTIVE_THRESH + cv::CALIB_CB_NORMALIZE_IMAGE);
        cv::drawChessboardCorners(captured, cv::Size(kChessHeight, kChessWidth),
          tmp_cam_points_, found);
        res_view_->Show(captured, 10, false, 0.5);
        if (found) {
          break;
        }
        k++;
      }
      if (k < max_attempt_times) {
        cv::cornerSubPix(captured, tmp_cam_points_, cv::Size(5, 5), cv::Size(-1, -1),
          cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 30, 0.1));
        break;
      }
    }
  }
  if (status) {
    this->sensor_manager_->UnloadPatterns();
  }
  return status;
}

// Collect first frame
bool DataCollector::CollectStaticFrame(int frameNum) {
	bool status = true;
	int kMultiCollectNum = 1;
  Mat ver_gray_mat, ver_phase_mat, hor_gray_mat, hor_phase_mat;

	// ver_gray:
	if (status) {
		status = sensor_manager_->LoadPatterns(kVerGrayNum * 2, pattern_path_, 
      vgray_name_, gray_suffix_);
	}
	if (status) {
		for (int i = 0; (i < kVerGrayNum * 2) && status; i++) {
			status = sensor_manager_->SetProPicture(i);
      cam_mats_[0].ver_gray[i] = GetRobustCameraFrame(kMultiCollectNum);
		}
	}
	if (status) {
		status = sensor_manager_->UnloadPatterns();
	}

	// ver_phase:
	if (status) {
		status = sensor_manager_->LoadPatterns(kPhaseNum, pattern_path_, 
        vphase_name_, phase_suffix_);
	}
	if (status) {
		for (int i = 0; (i < kPhaseNum) && status; i++) {
			status = sensor_manager_->SetProPicture(i);
      cam_mats_[0].ver_phase[i] = GetRobustCameraFrame(kMultiCollectNum);
		}
	}
	if (status) {
		status = sensor_manager_->UnloadPatterns();
	}

	// hor_gray:
	if (status) {
		status = sensor_manager_->LoadPatterns(kHorGrayNum * 2, pattern_path_, 
        hgray_name_, gray_suffix_);
	}
	if (status) {
		for (int i = 0; (i < kHorGrayNum * 2) && status; i++) {
			status = sensor_manager_->SetProPicture(i);
      cam_mats_[0].hor_gray[i] = GetRobustCameraFrame(kMultiCollectNum);
		}
	}
	if (status) {
		status = sensor_manager_->UnloadPatterns();
	}

	// hor_phase
	if (status) {
		status = sensor_manager_->LoadPatterns(kPhaseNum, pattern_path_,
			  hphase_name_, phase_suffix_);
	}
	if (status) {
		for (int i = 0; (i < kPhaseNum) && status; i++) {
			status = sensor_manager_->SetProPicture(i);
      cam_mats_[0].hor_phase[i] = GetRobustCameraFrame(kMultiCollectNum);
		}
	}
	if (status) {
		status = sensor_manager_->UnloadPatterns();
	}

	return status;
}

cv::Mat DataCollector::GetRobustCameraFrame(int multi_capture) {
  cv::Mat result, temp_mat;
  Mat temp_total_mat(kCamHeight, kCamWidth, CV_64FC1);
  temp_total_mat.setTo(0);
  cv::Mat * tmp_mul_collect = sensor_manager_->GetCamMonoFrames(multi_capture);
  for (int k = 0; k < multi_capture; k++) {
    tmp_mul_collect[k].convertTo(temp_mat, CV_64FC1);
    temp_total_mat += temp_mat / multi_capture;
  }
  temp_total_mat.convertTo(result, CV_8UC1);
  cam_view_->Show(result, 1, false, 1.0);
  return result;
}

// ---------------------------------
// Use for collect data continually
// Set projector pattern previously
// Then collect picture with camera
// ----------------------------------
bool DataCollector::CollectDynamicFrame() {
	bool status = true;
	// Set projector pattern
	if (status) {
		status = sensor_manager_->LoadPatterns(1, pattern_path_, 
        dyna_name_, dyna_suffix_);
	}
	if (status) {
		status = sensor_manager_->SetProPicture(0);
	}
	// collect camera image
  cam_mats_[0].dyna = sensor_manager_->GetCamColorFrames(max_frame_num_);
	// Unload projector pattern
	if (status) {
		this->sensor_manager_->UnloadPatterns();
	}
	return status;
}

bool DataCollector::DecodeSingleFrame(int frameNum) {
	bool status = true;
  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    Mat ver_gray_mat, hor_gray_mat, ver_phase_mat, hor_phase_mat;
    Mat ver_pro_mat, hor_pro_mat;

    // Decode
    GrayDecoder vgray_decoder;
    if (status) {
      status = vgray_decoder.SetNumDigit(kVerGrayNum, true);
    }
    if (status) {
      status = vgray_decoder.SetCodeFileName(pattern_path_, 
          vgray_code_name_ + gray_code_suffix_);
    }
    for (int i = 0; (i < kVerGrayNum * 2) && status; i++) {
      status = vgray_decoder.SetMat(i, cam_mats_[cam_idx].ver_gray[i]);
    }
    if (status) {
      status = vgray_decoder.Decode();
    }
    if (status) {
      ver_gray_mat = vgray_decoder.GetResult();
    }
    GrayDecoder hgray_decoder;
    if (status) {
      status = hgray_decoder.SetNumDigit(kHorGrayNum, false);
    }
    if (status) {
      status = hgray_decoder.SetCodeFileName(pattern_path_,
          hgray_code_name_ + gray_code_suffix_);
    }
    for (int i = 0; (i < kHorGrayNum * 2) && status; i++) {
      status = hgray_decoder.SetMat(i, cam_mats_[cam_idx].hor_gray[i]);
    }
    if (status) {
      status = hgray_decoder.Decode();
    }
    if (status) {
      hor_gray_mat = hgray_decoder.GetResult();
    }
    PhaseDecoder vphase_decoder;
    int ver_period = kProWidth / (1 << (kVerGrayNum - 1));
    if (status) {
      status = vphase_decoder.SetNumDigit(kPhaseNum, ver_period);
    }
    for (int i = 0; (i < kPhaseNum) && status; i++) {
      status = vphase_decoder.SetMat(i, cam_mats_[cam_idx].ver_phase[i]);
    }
    if (status) {
      status = vphase_decoder.Decode();
    }
    if (status) {
      ver_phase_mat = vphase_decoder.GetResult();
    }
    PhaseDecoder hphase_decoder;
    int hor_period = kProHeight / (1 << (kHorGrayNum - 1));
    if (status) {
      status = hphase_decoder.SetNumDigit(kPhaseNum, hor_period);
    }
    for (int i = 0; i < kPhaseNum; i++) {
      status = hphase_decoder.SetMat(i, cam_mats_[cam_idx].hor_phase[i]);
    }
    if (status) {
      status = hphase_decoder.Decode();
    }
    if (status) {
      hor_phase_mat = hphase_decoder.GetResult();
    }

    // Combine
    if (status) {
      int ver_gray_num = 1 << kVerGrayNum;
      int ver_period = kProWidth / (1 << (kVerGrayNum - 1));
      int ver_gray_period = kProWidth / ver_gray_num;
      //this->my_debug_->Show(ver_gray_mat, 0, true, 0.5);
      //this->my_debug_->Show(ver_phase_mat, 0, true, 0.5);
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
      /*this->res_view_->Show(ver_gray_mat, 0, true, 0.5);
      this->res_view_->Show(ver_phase_mat, 0, true, 0.5);*/
      cam_mats_[cam_idx].x_pro[frameNum] = ver_gray_mat + ver_phase_mat;
    }
    if (status) {
      int hor_gray_num = 1 << kHorGrayNum;
      int hor_period = kProHeight / (1 << (kHorGrayNum - 1));
      int hor_gray_period = kProHeight / hor_gray_num;
      //this->my_debug_->Show(tmp_gray_mat, 0, true, 0.5);
      //this->my_debug_->Show(tmp_phase_mat, 0, true, 0.5);
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
      /*this->my_debug_->Show(tmp_gray_mat, 0, true, 0.5);
      this->my_debug_->Show(tmp_phase_mat, 0, true, 0.5);*/
      cam_mats_[cam_idx].y_pro[frameNum] = hor_gray_mat + hor_phase_mat;
    }
  }
	return status;
}

bool DataCollector::FillChessBoard(int frameNum) {
  bool status = true;
  std::vector<cv::Point2f>().swap(tmp_pro_points_);

  // projector coord
  std::vector<cv::Point2f>::iterator i;
  for (i = tmp_cam_points_.begin(); i != tmp_cam_points_.end(); ++i) {
    cv::Point2f cam_coord = *i;
    cv::Point2f pro_coord;
    int X = cam_coord.x, Y = cam_coord.y;
    pro_coord.x = cam_mats_[0].x_pro[frameNum].at<double>(Y, X);
    if ((pro_coord.x < 0) && (X > 0) && (X < kCamWidth - 1)) {
      float lf_val = cam_mats_[0].x_pro[frameNum].at<double>(Y, X - 1);
      float rt_val = cam_mats_[0].x_pro[frameNum].at<double>(Y, X + 1);
      if ((lf_val > 0) && (rt_val > 0)) {
        pro_coord.x = (lf_val + rt_val) / 2;
      }
    }
    pro_coord.y = cam_mats_[0].y_pro[frameNum].at<double>(Y, X);
    if ((pro_coord.y < 0) && (Y > 0) && (Y < kCamHeight - 1)) {
      float up_val = cam_mats_[0].y_pro[frameNum].at<double>(Y - 1, X);
      float dn_val = cam_mats_[0].y_pro[frameNum].at<double>(Y + 1, X);
      if ((up_val > 0) && (dn_val > 0)) {
        pro_coord.y = (up_val + dn_val) / 2;
      }
    }
    tmp_pro_points_.push_back(pro_coord);
  }

  return status;
}

bool DataCollector::VisualizationForDynamicScene(int total_frame_num) {
	int key;
	bool status = true;
  bool continue_flag = true;
  while (continue_flag) {
    for (int frm_idx = 0; frm_idx < total_frame_num; frm_idx++) {
      key = cam_view_->Show(cam_mats_[0].dyna[frm_idx], 10, false, 1.0);
      if (key == 'y' || key == 'n') {
        continue_flag = false;
        break;
      }
    }
  }
  if (key == 'y') {
    status = true;
  } else if (key == 'n') {
    status = false;
  }
	return status;
}

bool DataCollector::VisualizationForStaticScene(int frame_idx) {
  int key1, key2;
  bool status = true;
  while (true) {
    key1 = cam_view_->Show(cam_mats_[0].x_pro[frame_idx], 500, true, 0.5);
    key2 = cam_view_->Show(cam_mats_[0].y_pro[frame_idx], 500, true, 0.5);
    if (key1 == 'y' || key1 == 'n' || key2 == 'y' || key2 == 'n') {
      break;
    }
  }
  if (key1 == 'y' || key2 == 'y') {
    status = true;
  }
  else {
    status = false;
  }
  return status;
}

bool DataCollector::VisualizationForCalibration(int frame_idx) {
  bool status;

  cv::Mat draw_mat(kProHeight, kProWidth, CV_8UC1);
  draw_mat.setTo(0);
  cv::drawChessboardCorners(draw_mat, cv::Size(kChessHeight, kChessWidth),
      tmp_pro_points_, true);
  int key = res_view_->Show(draw_mat, 0, false, 0.5);
  if (key == 'y') {
    status = true;
  } else {
    status = false;
  }

  return status;
}

bool DataCollector::Close() {
	bool status = true;
	if (status)	{
		status = this->sensor_manager_->CloseSensor();
	}
	return status;
}

bool DataCollector::StorageDataByGroup() {
	if (!this->storage_flag_)
		return true;
	// Set Folder
  StorageModule store;

  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    // CamFolderInfo
    stringstream ss_cam;
    ss_cam << "cam_" << cam_idx << "/";
    string cam_folder_path;
    ss_cam >> cam_folder_path;

    // Save dyna mats
    store.SetMatFileName(save_data_path_ + cam_folder_path + dyna_frame_path_,
        dyna_frame_name_, dyna_frame_suffix_);
    store.StoreAsImage(cam_mats_[cam_idx].dyna, max_frame_num_);
  }
	return true;
}

bool DataCollector::StorageDataByFrame(int frame_idx) {
  if (!this->storage_flag_)
    return true;
  // Set Folder
  StorageModule store;

  for (int cam_idx = 0; cam_idx < kCamDeviceNum; cam_idx++) {
    // CamFolderInfo
    stringstream ss_cam;
    ss_cam << "cam_" << cam_idx << "/";
    string cam_folder_path;
    ss_cam >> cam_folder_path;

    // Save x_pro
    store.SetMatFileName(save_data_path_ + cam_folder_path + pro_frame_path_,
      xpro_frame_name_, pro_frame_suffix_);
    store.StoreAsXml(&(cam_mats_[cam_idx].x_pro[frame_idx]), 1);
    store.StoreAsText(&(cam_mats_[cam_idx].x_pro[frame_idx]), 1);
    // Save y_pro
    store.SetMatFileName(save_data_path_ + cam_folder_path + pro_frame_path_,
      ypro_frame_name_, pro_frame_suffix_);
    store.StoreAsXml(&(cam_mats_[cam_idx].y_pro[frame_idx]), 1);
    store.StoreAsText(&(cam_mats_[cam_idx].y_pro[frame_idx]), 1);
  }
  return true;
}

bool DataCollector::PushChessBoardInfo() {
  bool status = true;
  cam_points_.push_back(tmp_cam_points_);
  obj_points_.push_back(tmp_obj_points_);
  pro_points_.push_back(tmp_pro_points_);
  return status;
}

bool DataCollector::CalibrateFromChessBoard() {
  printf("Begin Calibrating.\n");
  calibrateCamera(obj_points_,
    cam_points_,
    cv::Size(kCamWidth, kCamHeight),
    cam_matrix_,
    cam_distor_,
    cv::noArray(),
    cv::noArray(),
    cv::CALIB_FIX_K3 + cv::CALIB_FIX_PRINCIPAL_POINT,
    cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 30, 1e-16));
  printf("Finish cam.\n");
  calibrateCamera(obj_points_,
    pro_points_,
    cv::Size(kProWidth, kProHeight),
    pro_matrix_,
    pro_distor_,
    cv::noArray(),
    cv::noArray(),
    cv::CALIB_FIX_K3 + cv::CALIB_FIX_PRINCIPAL_POINT,
    cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 1e-16));
  printf("Finish pro.\n");
  stereoCalibrate(obj_points_,
    cam_points_,
    pro_points_,
    cam_matrix_,
    cam_distor_,
    pro_matrix_,
    pro_distor_,
    cv::Size(kCamWidth, kCamHeight),
    stereo_set_.R,
    stereo_set_.T,
    stereo_set_.E,
    stereo_set_.F,
    256,
    cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 300, 1e-16));
  printf("Finish cam & pro.\n");

  StorageModule store;
  store.SetMatFileName(save_data_path_, "cam_mat", ".txt");
  store.StoreAsText(&cam_matrix_, 1);
  store.StoreAsXml(&cam_matrix_, 1);
  std::cout << "cam_matrix: " << cam_matrix_ << std::endl;
  store.SetMatFileName(save_data_path_, "pro_mat", ".txt");
  store.StoreAsText(&pro_matrix_, 1);
  store.StoreAsXml(&pro_matrix_, 1);
  std::cout << "pro_matrix: " << pro_matrix_ << std::endl;
  store.SetMatFileName(save_data_path_, "R", ".txt");
  store.StoreAsText(&stereo_set_.R, 1);
  store.StoreAsXml(&stereo_set_.R, 1);
  std::cout << "R: " << stereo_set_.R << std::endl;
  store.SetMatFileName(save_data_path_, "T", ".txt");
  store.StoreAsText(&stereo_set_.T, 1);
  store.StoreAsXml(&stereo_set_.T, 1);
  std::cout << "T: " << stereo_set_.T << std::endl;

  return true;
}
