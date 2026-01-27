float triangle_verts[] = { 
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

float quad_verts[] = {
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // P1
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // P2
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // P3
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // P4
};

unsigned int quad_indices[] = {
    0, 1, 2,
    2, 3, 0,
};

//float quad_verts[] = { // USE GL_TRIANGLE_STRIP
//   -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,  // bottom-left
//    0.5f, -0.5f, 0.0f,    1.0f, 0.0f,  // bottom-right
//   -0.5f,  0.5f, 0.0f,    0.0f, 1.0f,  // top-left
//    0.5f,  0.5f, 0.0f,    1.0f, 1.0f   // top-right
//};

float anchor_vertices[] = {
    // xquad
    0.0f, 0.0f, 0.0f,
    10.0f, 0.0f, 0.0f,
    10.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // yquad
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 10.0f, 0.0f,
    0.0f, 10.0f, 0.0f,
    // zquad
    0.0f, 1.0f, 10.0f,
    0.0f, 0.0f, 10.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    // xquad2
    0.0f, 0.5f, -0.5f,
    0.0f, 0.5f, 0.5f,
    10.0f, 0.5f, 0.5f,
    10.0f, 0.5f, -0.5f,
};

unsigned int anchor_indices[] = {
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4,
    8, 9, 10,
    10, 11, 8,
    12, 13, 14,
    14, 15, 12,
};
