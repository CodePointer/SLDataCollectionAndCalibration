frm_idx = 1;

raw_img = imread(['dyna_mat', num2str(frm_idx), '.png']);

hsv_img = rgb2hsv(raw_img);

hsv_img(:, :, 2) = 1;
for h = 1:1024
  for w = 1:1280
    if hsv_img(h, w, 3) > 0.2
      hsv_img(h, w, 3) = 1.0;
    else
      hsv_img(h, w, 3) = 0.0;
    end
  end
end

imshow(hsv2rgb(hsv_img));

rgb_img = hsv2rgb(hsv_img);

thred = [0.6, 0.5, 0.05];

new_rgb_img = zeros(1024, 1280);
for h = 1:1024
  for w = 1:1280
    for c = 1:3
      if rgb_img(h, w, c) > thred(c)
        new_rgb_img(h, w, c) = 1.0;
      else
        new_rgb_img(h, w, c) = 0.0;
      end
    end
  end
end
imshow(new_rgb_img);