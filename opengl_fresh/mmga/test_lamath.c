#include "lamath.h"
//#include "linmath.h"


int main(void)
{
    // matf4x4 m = matf4x4_I_give();
    // m.col1 = (vecf4){1.0f, 2.0f, -1.0f, 2.0f};
    // m.col2 = (vecf4){10.0f, 5.0f,  2.0f, 1.0f};
    // m.col3 = (vecf4){4.0f, 0.0f,  3.0f, -2.0f};
    // m.col4 = (vecf4){-6.0f, 3.0f, 5.0f, 3.0f};
    // matf4x4_print(&m);
    
    
// 1 v -3.000000 1.800000 0.000000
// 4 v -2.989450 1.666162 0.000000
// 8 v -2.981175 1.667844 0.081000
    
    vecf3 triangle[3];
    triangle[0].x = 2.0;
    triangle[0].y = 2.0; 
    triangle[0].z = 0.0;

    triangle[1].x = 8.0;
    triangle[1].y = 4.0; 
    triangle[1].z = 0.0;

    triangle[2].x = 4.0;
    triangle[2].y = 6.0; 
    triangle[2].z = 0.0;

    vecf3_print(lamath_triangle_centroid(triangle));

    return 0;
}
