#include "data_collector.h"
int main()
{
	bool status = true;
	DataCollector dc;
	if (status)	{
		status = dc.Init(true);
	}
	if (status)	{
    //status = dc.CollectStatData();
		status = dc.CollectDynaData();
    //status = dc.CalibrateSystem();
	}
	if (status)	{
		status = dc.Close();
	}
	return 0;
}

//#include "calibrator.h"
//VisualModule cam_visual("CameraShow");
//VisualModule pro_visual("ProjectorShow");
//
//int main()
//{
//  bool status = true;
//  Calibrator calib;
//  if (status) {
//    status = calib.Init();
//  }
//  if (status) {
//    status = calib.Calibrate();
//  }
//  if (status) {
//    status = calib.Result();
//  }
//  return 0;
//}