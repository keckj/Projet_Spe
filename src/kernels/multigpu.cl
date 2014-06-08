
//188 bytes args
__kernel void computeStep(
					__global const float * restrict E_1, 
					__global       float * restrict E_2,
					__global const float * restrict E_externalEdgesLeft, 
					__global const float * restrict E_externalEdgesRight, 
					__global const float * restrict E_externalEdgesTop, 
					__global const float * restrict E_externalEdgesDown, 
					__global const float * restrict E_externalEdgesFront, 
					__global const float * restrict E_externalEdgesBack, 
					__global       float * restrict E_internalEdgesLeft, 
					__global       float * restrict E_internalEdgesRight, 
					__global       float * restrict E_internalEdgesTop, 
					__global       float * restrict E_internalEdgesDown, 
					__global       float * restrict E_internalEdgesFront, 
					__global       float * restrict E_internalEdgesBack, 
					__global const float * restrict R_1, 
					__global       float * restrict R_2,
					const unsigned int borderSize,
					const unsigned int domainIdx, 
					const unsigned int domainIdy, 
					const unsigned int domainIdz,
					const unsigned int nSplitsX, 
					const unsigned int nSplitsY,
					const unsigned int nSplitsZ,
					const unsigned int subGridWidth, 
					const unsigned int subGridHeight,
					const unsigned int subGridLength,
					const float alpha1, 
					const float alpha2, 
					const float mu1, 
					const float mu2,
					const float kk, 
					const float epsilon, 
					const float d,
					const float dh, 
					const float dt)

{
    // get index of the work item
	unsigned int idx = get_global_id(0);
	unsigned int idy = get_global_id(1);
	unsigned long long int id = idy*subGridWidth + idx;

	if(idx >= subGridWidth || idy >= subGridHeight || id >= subGridWidth*subGridHeight)
		return;

	float e = E_1[id];
	float r = R_1[id];
	
	float F = -kk*e*(e - alpha1)*(e - 1.0f) - e*r;
	float G = (epsilon + mu1*r/(e + mu2)) * (-r - kk*e*(e-alpha2-1.0f));
	float L =  d/(dh*dh) * (
			+ E_1[idx == 0 ? id : id-1]
			+ E_1[idx == subGridWidth-1 ? id : id+1]
			+ E_1[idy == 0 ? id : id-subGridWidth ]
			+ E_1[idy == subGridHeight-1 ? id : id+subGridWidth]
			- 4*e
			);

	E_2[id] = e + dt*(L+F);
	R_2[id] = r + dt*G;
}

