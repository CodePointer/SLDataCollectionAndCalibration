main_path = 'E:/SLDataSet/20181105/set1/';

% Load pattern
pattern_seq = load('DeBruijnSeq.txt');
pattern_seq = [6, pattern_seq, 6];
color_set = [1 0 1 
  1 1 0 
  0 0 1 
  0 1 1 
  1 0 0 
  0 1 0
  0 0 0];

% Load x_pro
dyna_mat = double(imread([main_path, 'dyna/dyna_mat0.png'])) / 255.0;
x_pro_mat = load([main_path, 'pro/xpro_mat0.txt']);

show_image = zeros(1024, 1280, 3);
for h = 1:1024
  for w = 1:1280
    if x_pro_mat(h, w) > 0
      left_idx = floor(x_pro_mat(h, w) / 5);
      color_idx = pattern_seq(left_idx + 1);
      show_image(h, w, 1) = color_set(color_idx + 1, 1);
      show_image(h, w, 2) = color_set(color_idx + 1, 2);
      show_image(h, w, 3) = color_set(color_idx + 1, 3);
    end
  end
end

figure(1), imshow(show_image);
figure(2), imshow(dyna_mat);