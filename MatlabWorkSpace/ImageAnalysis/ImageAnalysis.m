warning('off');
main_file_path = 'E:/SLDataSet/20181024/HandWave/cam_0/';

for frm_idx = 30:300
  KMeansClassfication;
  fprintf('%d frame finished.\n', frm_idx);
end