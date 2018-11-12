function [ depth_mat ] = TransferPoly2DepthMat( poly_idx, depth_mat, ...
  cam_mat, bunny2cam_mat, point_set )
% Project bunny poly to camera space
  [CAM_HEIGHT, CAM_WIDTH] = size(depth_mat);
  
  % Calculate vertex 2D position
  vertex_3d_bunny = [[point_set(poly_idx(1), :)', ...
    point_set(poly_idx(2), :)', point_set(poly_idx(3), :)']; ones(1, 3)];
  vertex_3d_camera = bunny2cam_mat * vertex_3d_bunny;
  vertex_2d_camera = [cam_mat, zeros(3, 1)] * vertex_3d_camera;
  vertex_coord = zeros(2, 3);
  vertex_coord(1, :) = vertex_2d_camera(1, :) ./ vertex_2d_camera(3, :);
  vertex_coord(2, :) = vertex_2d_camera(2, :) ./ vertex_2d_camera(3, :);
  
  % Check every point in the coord range
  x_start = floor(min(vertex_coord(1, :)));
  x_end = ceil(max(vertex_coord(1, :)));
  y_start = floor(min(vertex_coord(2, :)));
  y_end = ceil(max(vertex_coord(2, :)));
  for x = x_start:x_end
    for y = y_start:y_end
      if x >= 0 && x < CAM_WIDTH && y >= 0 && y < CAM_HEIGHT
        % Calculate u, v
        b = vertex_coord(:, 2) - vertex_coord(:, 1);
        c = vertex_coord(:, 3) - vertex_coord(:, 1);
        p = [x; y] - vertex_coord(:, 1);
        tmp = (c'*c)*(b'*b) - (c'*b)*(b'*c);
        u = ((b'*b)*(p'*c) - (c'*b)*(p'*b)) / tmp;
        v = ((c'*c)*(p'*b) - (c'*b)*(p'*c)) / tmp;
        if u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1
          w = [1 - (u + v), u, v]';
          depth_val = vertex_3d_camera(3, :) * w;
          if depth_val < depth_mat(y + 1, x + 1)
            depth_mat(y + 1, x + 1) = depth_val;
          end
        end
      end
    end
  end
  
end

