vecf3 teapot_centers[6320];
    vecf3 teapot_normals[6320];
    vecf3 teapot_normals_lines[6320*2];
    int centroid_count = 0;
    for (size_t i = 0; i < teapot_obj.i_count; i+=3) {

        vecf3 triangle[3];

        unsigned int t_one_index = teapot_obj.indices[i] * 3; // uncoding
        unsigned int t_two_index = teapot_obj.indices[i+1] * 3;
        unsigned int t_three_index = teapot_obj.indices[i+2] * 3;

        triangle[0] = (vecf3){teapot_obj.vertices[t_one_index],
                              teapot_obj.vertices[t_one_index+1],
                              teapot_obj.vertices[t_one_index+2]};
        triangle[1] = (vecf3){teapot_obj.vertices[t_two_index],
                              teapot_obj.vertices[t_two_index+1],
                              teapot_obj.vertices[t_two_index+2]};
        triangle[2] = (vecf3){teapot_obj.vertices[t_three_index],
                              teapot_obj.vertices[t_three_index+1],
                              teapot_obj.vertices[t_three_index+2]};

        //centroids[centroid_count] = lamath_triangle_centroid(triangle);
        vecf3 center = lamath_triangle_centroid(triangle);
        vecf3 normal = lamath_calc_triangle_normal(triangle); 
    
        // PIECE OF HIGH TEC CODE.
        // float dot = center.x*normal.x + center.y*normal.y + center.z*normal.z;
        // if (dot < 0.0f) {
        //     normal.x = -normal.x;
        //     normal.y = -normal.y;
        //     normal.z = -normal.z;
        // }

        teapot_centers[centroid_count].x = center.x;
        teapot_centers[centroid_count].y = center.y;
        teapot_centers[centroid_count].z = center.z;

        teapot_normals[centroid_count+1].x = normal.x;
        teapot_normals[centroid_count+1].y = normal.y;
        teapot_normals[centroid_count+1].z = normal.z;

        centroid_count++;
    } 

    float tn_scale = 0.1f;
    for (int i = 0; i < 6320; i++) {
        // MERGE TWO BUFFER TOGGERTHER KINDA CODE...
        // Start point of line
        teapot_normals_lines[i*2] = teapot_centers[i];
        // End point of line (center + scaled normal)
        teapot_normals_lines[i*2+1].x = teapot_centers[i].x + (teapot_normals[i].x * tn_scale);
        teapot_normals_lines[i*2+1].y = teapot_centers[i].y + (teapot_normals[i].y * tn_scale);
        teapot_normals_lines[i*2+1].z = teapot_centers[i].z + (teapot_normals[i].z * tn_scale);
    }
