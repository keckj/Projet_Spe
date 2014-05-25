
__kernel void vectorAdd(__global const float *x, 
                        __global const float *y, 
                        __global float *z)
{
    // get index of the work item
    int index = get_global_id(0);

    // add the vector elements
    z[index] = x[index] + y[index];
}
