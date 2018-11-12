x_pro_mat = -ones(1024, 1280);
for h = 1:CAM_HEIGHT
  for w = 1:CAM_WIDTH
    if depth_mat(h, w) < 0
      continue;
    end
    tmp_vec = [(w - 1 - cam_mat(1, 3)) / cam_mat(1, 1) * depth_mat(h, w); ...
      (h - 1 - cam_mat(2, 3)) / cam_mat(2, 2) * depth_mat(h, w); ...
      depth_mat(h, w);
      1];
    x_pro = (pro_matrix(1, :) * tmp_vec) / (pro_matrix(3, :) * tmp_vec);
    x_pro_mat(h, w) = x_pro;
  end
end