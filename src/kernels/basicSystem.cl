
#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#pragma OPENCL EXTENSION cl_khr_gl_event : enable
#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

__kernel void computeStep(__global const float * restrict E_1, __global float * restrict E_2,
					__global const float * restrict R_1, __global float * restrict R_2,
					const float alpha1, const float alpha2, const float mu1, const float mu2,
					const float kk, const float epsilon, const float d,
					const unsigned int width, const unsigned int height,
					const float dh, const float dt)

{
    // get index of the work item
	unsigned int idx = get_global_id(0);
	unsigned int idy = get_global_id(1);
	unsigned long long int id = idy*width + idx;

	if(idx >= width || idy >= height || id >= width*height)
		return;

	float e = E_1[id];
	float r = R_1[id];
	
	float F = -kk*e*(e - alpha1)*(e - 1.0f) - e*r;
	float G = (epsilon + mu1*r/(e + mu2)) * (-r - kk*e*(e-alpha2-1.0f));
	float L =  d/(dh*dh) * (
			+ E_1[idx == 0 ? id : id-1]
			+ E_1[idx == width-1 ? id : id+1]
			+ E_1[idy == 0 ? id : id-width ]
			+ E_1[idy == height-1 ? id : id+width]
			- 4*e
			);

	E_2[id] = e + dt*(L+F);
	R_2[id] = r + dt*G;
}

