function [] = WriteDepthMat2File( file_path, image_mat )
  file = fopen(file_path, 'w+');
  for h = 1:size(image_mat, 1)
    for w = 1:size(image_mat, 2)
      if image_mat(h, w) < 0
        fprintf(file, '%d ', image_mat(h, w));
      else
        fprintf(file, '%.2f ', image_mat(h, w));
      end
    end
    fprintf(file, '\n');
  end
  fclose(file);
end

