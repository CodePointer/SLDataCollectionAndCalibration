image_mat = zeros(1024, 1280, 3);
for h = 1:CAM_HEIGHT
  for w = 1:CAM_WIDTH
    if x_pro_mat(h, w) < 0
      continue;
    end
    left_idx = floor(x_pro_mat(h, w) / 8.0);
    right_idx = left_idx + 1;
    left_color = [0, 0, 0];
    if left_idx >= 1 && left_idx <= 128
      left_color = color_set(de_seq(left_idx), :);
    end
    right_color = [0, 0, 0];
    if right_idx >= 1 && right_idx <= 128
      right_color = color_set(de_seq(right_idx), :);
    end
    % -4:4, sigmoid
    dis_norm = (x_pro_mat(h, w) / 8.0 - floor(x_pro_mat(h, w) / 8.0));
    left_weight = 1 - sigmf(dis_norm, [8, 0.5]);
    right_weight = 1 - left_weight;
    pixel_color = left_weight * left_color + right_weight * right_color;
    image_mat(h, w, 1) = pixel_color(1);
    image_mat(h, w, 2) = pixel_color(2);
    image_mat(h, w, 3) = pixel_color(3);
  end
end