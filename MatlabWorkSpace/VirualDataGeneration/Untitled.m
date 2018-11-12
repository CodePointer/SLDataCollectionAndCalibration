depth = 0.7;
h = 512;
w = 640;
tmp_vec = [(w - 1 - cam_mat(1, 3)) / cam_mat(1, 1) * depth; ...
        (h - 1 - cam_mat(2, 3)) / cam_mat(2, 2) * depth; ...
        depth;
        1];
x_pro = (pro_matrix(1, :) * tmp_vec) / (pro_matrix(3, :) * tmp_vec);
disp(x_pro);