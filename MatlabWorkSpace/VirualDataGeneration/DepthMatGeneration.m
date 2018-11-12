% Create depth_mat
depth_mat = ones(CAM_HEIGHT, CAM_WIDTH) * 3000;
eular_angle = start_eular_angle ...
  + (i / total_frame_num - 1) * (end_eular_angle - start_eular_angle);
trans_vec = start_trans_vec ...
  + (i / total_frame_num - 1) * (end_trans_vec - start_trans_vec);
rad_para = deg2rad(eular_angle);
rot_mat = eul2rotm(rad_para);
bunny2cam_mat = [rot_mat, trans_vec; [0, 0, 0, 1]];
% Project to cam_image
poly_size = size(poly_set, 1);
for p_idx = 1:poly_size
  %     break;
  poly_idx = poly_set(p_idx, :) + 1;
  depth_mat = TransferPoly2DepthMat(poly_idx, depth_mat, cam_mat, bunny2cam_mat, point_cloud);
  % Show part
%   if mod(p_idx, 100) == 0
%     fprintf('%d/%d finished.\n', p_idx, poly_size);
%     imshow(depth_mat, [1000, 3000]);
%   end
end
depth_mat(depth_mat == 3000) = -1;