seq_file = load('rand_seq.txt');
color_set = load('ColorSet.txt');

pattern = zeros(768, 1024, 3);
for idx = 1:127
  left = idx * 8 - 1;
  right = idx * 8 + 2;
  if idx == 128
    right = 1024;
  end
  pattern(:, left:right, 1) = color_set(seq_file(idx), 1);
  pattern(:, left:right, 2) = color_set(seq_file(idx), 2);
  pattern(:, left:right, 3) = color_set(seq_file(idx), 3);
end

imshow(pattern)
imwrite(pattern, 'pattern_rand_seq0.png');