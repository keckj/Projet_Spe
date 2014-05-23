#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "CL/cl.h"
#include "utils.hpp"

#ifndef __DEBUG
#define __DEBUG 1 
#endif 

#ifndef __LOG_LEVEL
#define __LOG_LEVEL INFO
#endif

int main (void) {
		
	cl_uint nPlatforms=0;
	clGetPlatformIDs(0,NULL,&nPlatforms);
	printf("%i platorms found !\n\n",nPlatforms);

	cl_platform_id *platformIds = malloc(nPlatforms*sizeof(cl_platform_id));
	clGetPlatformIDs(nPlatforms, platformIds, NULL);

	cl_uint *nDevices = malloc(nPlatforms*sizeof(cl_uint));
	cl_device_id **deviceIds = malloc(nPlatforms*sizeof(cl_device_id *));

	unsigned char buffer[1000];
	for (cl_uint i = 0; i < nPlatforms; i++) {
		printf("== Platform %i ==\n", i);
		clGetPlatformInfo(platformIds[i], CL_PLATFORM_VENDOR, 1000, buffer, NULL);
		printf("%s", buffer);
		clGetPlatformInfo(platformIds[i], CL_PLATFORM_NAME, 1000, buffer, NULL);
		printf(", %s\n", buffer);
		
		clGetPlatformInfo(platformIds[i], CL_PLATFORM_VERSION, 1000, buffer, NULL);
		printf("Version : %s\n", buffer);
		clGetPlatformInfo(platformIds[i], CL_PLATFORM_PROFILE, 1000, buffer, NULL);
		printf("Profile : %s\n", buffer);
	
		printf("Devices :\n");
		clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_ALL, 0, NULL, nDevices + i);
		deviceIds[i] = malloc(nDevices[i]*sizeof(cl_device_id));
		clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_ALL, nDevices[i], deviceIds[i], NULL);
		cl_device_type device_type;
		for (cl_uint j = 0; j < nDevices[i]; j++) {
			clGetDeviceInfo(deviceIds[i][j], CL_DEVICE_NAME, 1000, buffer, NULL);
			printf("\t%s\n",buffer);
			clGetDeviceInfo(deviceIds[i][j], CL_DEVICE_TYPE, sizeof(cl_device_type), &device_type, NULL);
			printf("\tType : ");
			switch(device_type) {
				case CL_DEVICE_TYPE_CPU: printf("CPU"); break;
				case CL_DEVICE_TYPE_GPU: printf("GPU"); break;
				case CL_DEVICE_TYPE_ACCELERATOR: printf("ACCELERATOR"); break;
				default: printf("UNKNOWN"); 
			}
			printf("\n\n");
		}
	}

	return 0;
}




