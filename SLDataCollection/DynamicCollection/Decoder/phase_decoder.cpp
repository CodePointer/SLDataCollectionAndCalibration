#include "phase_decoder.h"

PhaseDecoder::PhaseDecoder() {
	image_num_ = 0;
	pix_period_ = 16;
	img_height_ = kCamHeight;
	img_width_ = kCamWidth;
	grey_mats_ = NULL;
	vis_mod_ = NULL;
}

PhaseDecoder::~PhaseDecoder() {
	DeleteSpace();
}

// Allocate space for mats
bool PhaseDecoder::AllocateSpace() {
	if (image_num_ <= 0)
		return false;
	grey_mats_ = new cv::Mat[image_num_];
	return true;
}

// Release space
bool PhaseDecoder::DeleteSpace() {
	if (grey_mats_ != NULL)	{
		delete[](grey_mats_);
		grey_mats_ = NULL;
	}
	if (vis_mod_ != NULL)	{
		delete(vis_mod_);
		vis_mod_ = NULL;
	}
	return true;
}

// Calculate result according to grey mats
bool PhaseDecoder::CountResult() {
	res_mat_.create(img_height_, img_width_, CV_64FC1);
	cv::Mat SinValue0;
	cv::Mat SinValue1;
	for (int h = 0; h < img_height_; h++)	{
		for (int w = 0; w < img_width_; w++) {
			// acquaire 4 grey value at this point:
      // greyValue0 = (sin(x) + 1) * 127;
      // greyValue1 = (sin(x + CV_PI / 2) + 1) * 127
      // greyValue2 = (sin(x + CV_PI) + 1) * 127
      // greyValue3 = (sin(x + 3 * CV_PI / 2) + 1) * 127
			float greyValue0 = grey_mats_[0].at<uchar>(h, w);
			float greyValue1 = grey_mats_[1].at<uchar>(h, w);
			float greyValue2 = grey_mats_[2].at<uchar>(h, w);
			float greyValue3 = grey_mats_[3].at<uchar>(h, w);
			float sinValue = (greyValue0 - greyValue2) / 2;
			float cosValue = (greyValue1 - greyValue3) / 2;
			float x = cv::fastAtan2(sinValue, cosValue);
			float pix = (x) / (360)*(float)(pix_period_);
			pix += 0.5;
			if (pix > pix_period_)
				pix -= pix_period_;
			res_mat_.at<double>(h, w) = (double)pix;
		}
	}
	return true;
}

// decode the result
bool PhaseDecoder::Decode() {
	if (!CountResult())	{
		ErrorHandling("PhaseDecoder.Decode()->CountResult fault");
		return false;
	}
	Visualize();
	return true;
}

// get result when decode finished.
cv::Mat PhaseDecoder::GetResult() {
	cv::Mat result;
	res_mat_.copyTo(result);
	return result;
}

// Input grey images
bool PhaseDecoder::SetMat(int num, cv::Mat pic) {
	if (grey_mats_ == NULL)	{
		ErrorHandling("PhaseDecoder.SetMat->grePicture Space is not allocated.");
		return false;
	}
	pic.copyTo(grey_mats_[num]);
	return true;
}

// Set num digit information
bool PhaseDecoder::SetNumDigit(int num_digit, int pixperiod) {
	if ((num_digit <= 0))
		return false;
	image_num_ = num_digit;
	pix_period_ = pixperiod;

	if ((grey_mats_ != NULL))
		DeleteSpace();
	AllocateSpace();
	return true;
}

// visualization middle result
bool PhaseDecoder::Visualize() {
	if (kVisualFlagForDebug) {
		if (vis_mod_ == NULL)
			vis_mod_ = new VisualModule("PhaseDecoder");
		for (int i = 0; i < image_num_; i++) {
			vis_mod_->Show(grey_mats_[i], 300);
		}
		vis_mod_->Show(res_mat_, 1000, true, 0.5);
	}
	return true;
}