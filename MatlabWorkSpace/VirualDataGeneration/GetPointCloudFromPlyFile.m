function [ point_cloud, poly_set ] = GetPointCloudFromPlyFile( file_name )
  ply_file = fopen(file_name, 'r+');
  point_num = fscanf(ply_file, '%d %d', 2);
  point_cloud = zeros(point_num(1), 3);
  poly_set = uint32(zeros(point_num(2), 3));
  for i = 1:point_num(1)
    data_vec = fscanf(ply_file, '%f %f %f %f %f', 5);
    point_cloud(i, :) = data_vec(1:3);
  end
  for i = 1:point_num(2)
    data_vec = fscanf(ply_file, '%f %f %f %f', 4);
    poly_set(i, :) = uint32(data_vec(2:4));
  end
  fclose(ply_file);
end