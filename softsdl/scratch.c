//  // ROTATE CUBE.
    // float rot = PI;
    // for (size_t i = 0; i < ARRLEN(cube); i+=3) {
    //     float x = cube[i];
    //     float z = cube[i+2];
    //     cube[i]   = cos(rot) * x - sin(rot) * z;
    //     cube[i+2] = sin(rot) * x + cos(rot) * z;
    // }

    // TRANSLATE CUBE.
  //  for (size_t i = 0; i < ARRLEN(cube); i+=3) {
   //     cube[i+2] += -4.0f;
   // }
 //
    // int points[16] = {0};
    // int pt = 0;
    // for (size_t vertex = 0; vertex < ARRLEN(cube); vertex+=3) {
    //     if (cube[vertex+2] == 0.0f) continue; // clipping XD
    //     float x_proj = (cube[vertex] / -cube[vertex+2]) / aspect_ratio;
    //     float y_proj = cube[vertex+1] / -cube[vertex+2];
    //     float x_proj_remap = (x_proj + 1) / 2;
    //     float y_proj_remap = (y_proj + 1) / 2;
    //     int x_proj_pix = (int)(x_proj_remap * ec.width);
    //     int y_proj_pix = (int)(y_proj_remap * ec.height);
    //     points[pt] = x_proj_pix;
    //     points[pt+1] = y_proj_pix;
    //     printf("[x:%f, y:%f, z:%f]\n",cube[vertex], cube[vertex+1], cube[vertex+2]);
    //     pt+=2;
    // }
    //
    // for (size_t i = 0; i < ARRLEN(points);) {
    //     printf("[%d, %d]\n", points[i], points[i+1]);
    //     i+=2;
    // }
    // fflush(stdout);
    //
    // for (size_t i = 0; i < ARRLEN(cube_idx);) {
    //     int idx1 = cube_idx[i] * 2;
    //     int idx2 = cube_idx[i+1] * 2;
    //     int x1 = points[idx1];
    //     int y1 = points[idx1+1];
    //     int x2 = points[idx2];
    //     int y2 = points[idx2+1];
    //     eray_plot_line(ec, x1, y1, x2, y2, ERAY_COLOR_RED);
    //     i+=2;
    // }   
    //
