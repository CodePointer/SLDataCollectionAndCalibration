hit_mat = zeros(6, 6, 6);
for i1 = 1:6
  for i2 = 1:6
    for i3 = 1:6
      if i1 == i2 || i2 == i3
        hit_mat(i1, i2, i3) = 1;
      end
    end
  end
end
outputs = -ones(120, 1);

outputs(1) = 1;
outputs(2) = 2;
i = 3;
while (1)
  if sum(hit_mat(outputs(i - 2), outputs(i - 1), :)) == 6
    % Check if still have ways out
    break;
  end
  outputs(i) = randi([1, 6]);
  outputs = uint8(outputs);
  if hit_mat(outputs(i - 2), outputs(i - 1), outputs(i)) == 0
    hit_mat(outputs(i - 2), outputs(i - 1), outputs(i)) = 1;
    i = i + 1;
  end
end

if i >= 128
  file = fopen('rand_seq.txt', 'w+');
  for x = 1:128
    fprintf(file, '%d\n', outputs(x));
  end
  fclose(file);
  fprintf('Success.\n');
end