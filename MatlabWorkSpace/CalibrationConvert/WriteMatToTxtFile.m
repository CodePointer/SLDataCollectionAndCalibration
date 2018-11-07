function [] = WriteMatToTxtFile(file_name, out_mat)
  [H, W] = size(out_mat);
  file_id = fopen(file_name, 'w+');
  for h = 1:H
    for w = 1:W
      fprintf(file_id, '%4f\t', out_mat(h, w));
    end
    fprintf(file_id, '\n');
  end
  fclose(file_id);
end