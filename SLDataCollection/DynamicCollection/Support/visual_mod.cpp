#include "visual_mod.h"

VisualModule::VisualModule(std::string winName) {
	this->win_name_ = winName;
	cv::namedWindow(this->win_name_);
}

VisualModule::~VisualModule() {
	cv::destroyWindow(this->win_name_);
}

int VisualModule::Show(cv::Mat pic, int time, bool norm, double zoom) {
  cv::Mat show_mat;
  cv::Size show_size = cv::Size(pic.size().width*zoom, pic.size().height*zoom);
  cv::resize(pic, show_mat, show_size, 0.0, 0.0, cv::INTER_NEAREST);
	
	// if need to norm
	if (norm)	{
    // Set range
		int range = 0;		
		if (show_mat.depth() == CV_8U) {
			range = 0xff;
		}	else if (show_mat.depth() == CV_16U) {
			range = 0xffff;
		}	else if (show_mat.depth() == CV_64F) {
			range = 0xffff;
      cv::Mat tmp;
			tmp.create(show_mat.size(), CV_16UC1);
			for (int h = 0; h < show_mat.size().height; h++) {
				for (int w = 0; w < show_mat.size().width; w++)	{
					double value;
					value = show_mat.at<double>(h, w);
					if (value < 0)
						value = 0;
					tmp.at<ushort>(h, w) = (ushort)value;
				}
			}
			tmp.copyTo(show_mat);
		}
		// find min, max of initial mat
		int min, max;
		min = range;
		max = 0;
		for (int i = 0; i < show_mat.size().height; i++) {
			for (int j = 0; j < show_mat.size().width; j++)	{
				int value = 0;
				if (show_mat.depth() == CV_8U) {
					value = show_mat.at<uchar>(i, j);
				}	else if (show_mat.depth() == CV_16U) {
					value = show_mat.at<ushort>(i, j);
				}
				if (value < min)
					min = value;
				if (value > max)
					max = value;
			}
		}

		// initialization
    show_mat = (show_mat - min) / (max - min) * range;
	}

  cv::imshow(this->win_name_, show_mat);
	return cv::waitKey(time);
}

//int VisualModule::CombineShow(cv::Mat * pics, int num, int time, cv::Mat mask, double zoom) {
//  cv::Mat combine_mat, show_mat;
//  //Size combine_size = Size(pic.size().width * num, pic.size().height);
//  cv::Size show_size = cv::Size(pics[0].size().width*zoom*num, pics[0].size().height*zoom);
//  // Combine part: hconcat(B,C,A);
//  cv::Mat mat_B;
//  pics[0].copyTo(mat_B);
//  if (mat_B.size() == mask.size()) {
//    for (int h = 0; h < kCamHeight; h++) {
//      for (int w = 0; w < kCamWidth; w++) {
//        int pix_num = mask.at<uchar>(h, w);
//        if (pix_num >= kPointPerPixelNum) {
//          mat_B.at<uchar>(h, w) = 255;
//        }
//      }
//    }
//  }
//  cv::Mat mat_C;
//  for (int i = 1; i < num; i++) {
//	  mat_C = pics[i];
//	  hconcat(mat_B, mat_C, combine_mat);
//	  mat_B = combine_mat;
//  }
//  resize(combine_mat, show_mat, show_size, 0.0, 0.0, INTER_NEAREST);
//  imshow(this->win_name_, show_mat);
//  return waitKey(time);
//}