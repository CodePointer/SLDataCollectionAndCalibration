% Important info:
% The coordiantes are inverse on X
% Which means the klab use the left-handed coordinates order.
% So just inverse the X coord in the processing.

clear;
main_path = 'E:/SLDataSet/20181105/calib1105/';
main_para_path = [main_path, ''];

cam_para_name = [main_para_path, '/cam/param.co.00.txt'];
pro_para_name = [main_para_path, '/pro/projparam.txt'];
epi_para_name = [main_para_path, '/pro/projtocampose.txt'];

cam_para_data = load(cam_para_name);
pro_para_data = load(pro_para_name);
epi_para_data = load(epi_para_name);

cam_mat = zeros(3, 3);
cam_mat(1, 1) = cam_para_data(1) / cam_para_data(3);
cam_mat(2, 2) = cam_para_data(1) / cam_para_data(4);
cam_mat(1, 3) = cam_para_data(7) - cam_para_data(5); % Inverse of X
cam_mat(2, 3) = cam_para_data(6);
cam_mat(3, 3) = 1.0;

pro_mat = zeros(3, 3);
pro_mat(1, 1) = pro_para_data(1) / pro_para_data(3);
pro_mat(2, 2) = pro_para_data(1) / pro_para_data(4);
pro_mat(1, 3) = pro_para_data(7) - pro_para_data(5); % Inverse of X
pro_mat(2, 3) = pro_para_data(6);
pro_mat(3, 3) = 1.0;

trans_vec = epi_para_data(1:3)';
trans_vec(1) = -trans_vec(1);% Inverse of X
% eular_angle = epi_para_data(4:6)';
% X = epi_para_data(4);
% Y = epi_para_data(5);
% Z = epi_para_data(6);
% rot_X = [1, 0, 0; 0, cos(X), -sin(X); 0, sin(X), cos(X)];
% rot_Y = [cos(Y), 0, sin(Y); 0, 1, 0; -sin(Y), 0, cos(Y)];
% rot_Z = [cos(Z), -sin(Z), 0; sin(Z), cos(Z), 0; 0, 0, 1];
rad_para = deg2rad(epi_para_data(4:6));
rad_para(1) = -rad_para(1);% Inverse of X
rot_mat = eul2rotm(rad_para);

% save([main_path, 'cam_mat0.txt'], 'cam_mat');
% save([main_path, 'pro_mat0.txt'], 'pro_mat');
% save([main_path, 'R0.txt'], 'rot_mat');
% save([main_path, 'T0.txt'], 'trans_vec');
WriteMatToTxtFile([main_path, 'cam_mat0.txt'], cam_mat);
WriteMatToTxtFile([main_path, 'pro_mat0.txt'], pro_mat);
WriteMatToTxtFile([main_path, 'R0.txt'], rot_mat);
WriteMatToTxtFile([main_path, 'T0.txt'], trans_vec');