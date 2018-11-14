% Some parameters
% clear;
main_path = 'E:/SLDataSet/20181112/';
cam_mat = load([main_path, 'cam_mat0.txt']);
pro_mat = load([main_path, 'pro_mat0.txt']);
% r_mat = load([main_path, 'R0.txt']);
% t_vec = load([main_path, 'T0.txt']);
r_mat = eul2rotm(deg2rad([12, -12, 2]));
t_vec = [0.41, -0.18, 0.9];
color_set = load([main_path, 'ColorSet.txt']);
de_seq = load([main_path, 'rand_seq.txt']);
CAM_HEIGHT = 1024;
CAM_WIDTH = 1280;
pro_matrix = pro_mat * [r_mat, t_vec'];
warning('off');

% Set parameters
total_frame_num = 1000;
output_folder = 'DataSet3';
start_eular_angle = [180, 0, 45];
end_eular_angle = [180, 360.0, 0];
start_trans_vec = [-0.1; 0.15; 1.0];
end_trans_vec = [-0.1; 0.25; 1.0];

% Get point cloud
model_name = 'bun_zipper_res3.ply';
[point_cloud, poly_set] = GetPointCloudFromPlyFile([main_path, model_name]);
point_cloud = point_cloud * 2;

% Make
mkdir([main_path, output_folder]);
for i = 0:total_frame_num - 1
  % Depth mat
  DepthMatGeneration;
%   figure(1), imshow(depth_mat, [0, 2]);
  
  % Xpro mat
  XproMatGeneration;
%   figure(2), imshow(x_pro_mat, [0, 1024]);

  % image mat
  ImageMatGeneration;
  figure(3), imshow(image_mat), title(['Image: (', num2str(i), ')']);
  fprintf('Frame(%d) generation finished.\n', i);
  
%   WriteDepthMat2File([main_path, output_folder, '/', 'depth', num2str(i), '.txt'], depth_mat);
  WriteDepthMat2File([main_path, output_folder, '/', 'x_pro', num2str(i), '.txt'], x_pro_mat);
  imwrite(image_mat, [main_path, output_folder, '/', 'cam_img', num2str(i), '.png']);
  pause(0.1);
  fprintf('Frame(%d) writing finished.\n', i);
end