de_bruijn_seq = load('./DeBruijn_3_6.txt');

pattern = zeros(768, 1024, 3);
color_set = [1, 0, 1;
  1, 1, 0;
  0, 0, 1; 
  0, 1, 1;
  1, 0, 0;
  0, 1, 0;];

% 216. 1024 for 213 color.
for i = 1:204
  c_idx = de_bruijn_seq(i + 3) + 1;
  pattern(:, i*5-1:i*5+1, 1) = color_set(c_idx, 1);
  pattern(:, i*5-1:i*5+1, 2) = color_set(c_idx, 2);
  pattern(:, i*5-1:i*5+1, 3) = color_set(c_idx, 3);
end
imshow(pattern)
imwrite(pattern, 'pattern_D_3pix.png');
seq = double(de_bruijn_seq(4:204+3));
file = fopen('DeBruijnSeq.txt', 'w+');
for i = 1:204
  fprintf(file, '%d ', de_bruijn_seq(i + 3));
end
fclose(file);
file = fopen('ColorSet.txt', 'w+');
for h = 1:6
  for w = 1:3
    fprintf(file, '%d ', color_set(h, w));
  end
  fprintf(file, '\n');
end