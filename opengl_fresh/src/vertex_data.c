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
