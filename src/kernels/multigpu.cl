
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

	if(idx >= subGridWidth || idy >= subGridHeight)
		return;

	/*float e = E_1[id];*/
	/*float r = R_1[id];*/
	
	/*float Le;*/
	/*if(idx==0 || idx==subGridWidth-1 || idy==0 || idy==subGridHeight-1) {*/
		/*float left,right,top,down,front,back;*/
		/*left =  (idx == 0               ? (domainIdx == 0          ? e : E_externalEdgesLeft[idy])  : E_1[id-1]);	*/
		/*right = (idx == subGridWidth-1  ? (domainIdx == nSplitsX-1 ? e : E_externalEdgesRight[idy]) : E_1[id+1]);	*/
		/*top =   (idy == 0               ? (domainIdy == 0          ? e : E_externalEdgesTop[idx] )  : E_1[id-subGridWidth]);	*/
		/*down =  (idy == subGridHeight-1 ? (domainIdy == nSplitsY-1 ? e : E_externalEdgesDown[idx])  : E_1[id+subGridWidth]);	*/
		
		/*Le = (left+right+top+down-4*e)/(dh*dh);*/
	/*}*/
	/*else {*/
		/*Le =(+ E_1[id-1]*/
			 /*+ E_1[id+1]*/
			 /*+ E_1[id-subGridWidth]*/
			 /*+ E_1[id+subGridWidth]*/
			 /*- 4*e) / (dh*dh);*/
	/*}*/
	
	/*float F = -kk*e*(e - alpha1)*(e - 1.0f) - e*r;*/
	/*float G = (epsilon + mu1*r/(e + mu2)) * (-r - kk*e*(e-alpha2-1.0f));*/
	/*float L =  d*Le;*/

	/*float newE = e + dt*(L+F);*/
	/*E_2[id] = newE;*/
	/*R_2[id] = r + dt*G;*/

	/*if(idx==0			    && domainIdx>0)			 E_internalEdgesLeft[idy]  = newE;*/
	/*if(idx==subGridWidth-1  && domainIdx<nSplitsX-1) E_internalEdgesRight[idy] = newE;*/
	/*if(idy==0               && domainIdy>0)		   	 E_internalEdgesTop[idx]   = newE;*/
	/*if(idy==subGridHeight-1 && domainIdy<nSplitsY-1) E_internalEdgesDown[idx]  = newE;*/

	E_2[id] = 1;
	R_2[id] = 1;
	
	if(idx==0			    && domainIdx>0)			 E_internalEdgesLeft[idy]  = 0.5;
	if(idx==subGridWidth-1  && domainIdx<nSplitsX-1) E_internalEdgesRight[idy] = 0.5;
	if(idy==0               && domainIdy>0)		   	 E_internalEdgesTop[idx]   = 0.5;
	if(idy==subGridHeight-1 && domainIdy<nSplitsY-1) E_internalEdgesDown[idx]  = 0.5;
	
	if(idx==0			    && domainIdx>0)			 E_2[id] = E_externalEdgesLeft[idy];
	if(idx==subGridWidth-1  && domainIdx<nSplitsX-1) E_2[id] = E_externalEdgesRight[idy];
	if(idy==0               && domainIdy>0)		   	 E_2[id] = E_externalEdgesTop[idx];  
	if(idy==subGridHeight-1 && domainIdy<nSplitsY-1) E_2[id] = E_externalEdgesDown[idx]; 
}
	
	//DEBUG 1
	/*E_2[id] = 1;*/
	/*R_2[id] = 1;*/

	/*E_2[id] = (idy == 0               ? (domainIdy == 0          ? 0 : 0.5)  : E_2[id]);	*/
	/*E_2[id] = (idy == subGridHeight-1 ? (domainIdy == nSplitsY-1 ? 0 : 0.5)  : E_2[id]);	*/
	/*E_2[id] = (idx == 0               ? (domainIdx == 0          ? 0 : 0.5)  : E_2[id]);	*/
	/*E_2[id] = (idx == subGridWidth-1  ? (domainIdx == nSplitsX-1 ? 0 : 0.5)  : E_2[id]);	*/
	
	/*R_2[id] = (idx == 0               ? (domainIdx == 0          ? 0 : 0.5)  : R_2[id]);	*/
	/*R_2[id] = (idx == subGridWidth-1  ? (domainIdx == nSplitsX-1 ? 0 : 0.5)  : R_2[id]);	*/
	/*R_2[id] = (idy == 0               ? (domainIdy == 0          ? 0 : 0.5)  : R_2[id]);	*/
	/*R_2[id] = (idy == subGridHeight-1 ? (domainIdy == nSplitsY-1 ? 0 : 0.5)  : R_2[id]);	*/
	
	//DEBUG 2
	/*E_2[id] = 1;*/
	/*R_2[id] = 1;*/
	/*if(idx==0			    && domainIdx>0)			 E_2[id]=0;*/
	/*if(idx==subGridWidth-1  && domainIdx<nSplitsX-1) E_2[id]=0;*/
	/*if(idy==0               && domainIdy>0)		   	 E_2[id]=0;*/
	/*if(idy==subGridHeight-1 && domainIdy<nSplitsY-1) E_2[id]=0;*/

	
