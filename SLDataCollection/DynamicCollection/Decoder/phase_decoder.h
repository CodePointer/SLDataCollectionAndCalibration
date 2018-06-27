#ifndef _PHASEDECODER_H_
#define _PHASEDECODER_H_

#include <opencv2/opencv.hpp>
#include "../Support/visual_mod.h"
#include "../global_fun.h"
#include "../static_para.h"

// Decoder for phase shifting.
// Input several grey mats and numdigit
// Output a mats, with bias value on each pixel
// Usage: SetMat(),SetNumDigit(),Decode(),GetResult()
class PhaseDecoder {
private:
	int image_num_;		        // image number. default: 4
	int pix_period_;	        // pixel period of mat
	int img_height_;				  // Resolution, height
	int img_width_;			      // Resolution, width
	cv::Mat * grey_mats_;	        // The input grey mats
	cv::Mat res_mat_;		          // Output result
	VisualModule * vis_mod_;	// visualization module

	bool AllocateSpace();
	bool DeleteSpace();
	bool CountResult();
	bool Visualize();

public:
	PhaseDecoder();
	~PhaseDecoder();

	bool Decode();
	cv::Mat GetResult();
	bool SetMat(int num, cv::Mat pic);
	bool SetNumDigit(int numDigit, int pixperiod);
};

#endif