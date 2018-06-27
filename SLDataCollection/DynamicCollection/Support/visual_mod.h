#ifndef _VISUALMOD_H_
#define _VISUALMOD_H_

#include <string>
#include <opencv2/opencv.hpp>
#include "../static_para.h"

// visualization module. Create and Destroy Windows Antomatically.
class VisualModule {
private:
	std::string win_name_;		// name of created window

public:
	VisualModule(std::string win_name);
	~VisualModule();
	int Show(cv::Mat pic, int time, bool norm = false, double zoom = 1.0);
  //int CombineShow(Mat * pics, int num, int time, Mat mask, double zoom = 1.0);
};

#endif