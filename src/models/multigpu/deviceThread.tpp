
#include "headers.hpp"
#include "log.hpp"
#include "clUtils.hpp"
#include <thread>


using namespace log4cpp;

template <unsigned int nCommandQueues>
std::mutex DeviceThread<nCommandQueues>::_mutex;

template <unsigned int nCommandQueues>
std::condition_variable DeviceThread<nCommandQueues>::_cond;

template <unsigned int nCommandQueues>
bool DeviceThread<nCommandQueues>::_called;

template <unsigned int nCommandQueues>
DeviceThread<nCommandQueues>::DeviceThread(MultiGpu *simulation, 
		const cl::Platform &platform,
		const cl::Context &context,
		const cl::Program &program,
		const cl::Device &device,
		Fence *fence,
		std::map<QString, Argument> *args,
		float dh,
		bool display,
		bool writeOnDisk,
		const std::string &rep) :
	_simulation(simulation), 
	_platform(platform), _context(context),
	_program(program), _device(device),
	_fence(fence),
	_dh(dh),
	_display(display),
	_writeOnDisk(writeOnDisk)
{ 
	_epsilon = args->at("epsilon")();
	_dd = args->at("d")();
	_kk = args->at("k")();
	_mu1 = args->at("mu_1")();
	_mu2 = args->at("mu_2")();
	_alpha1 = args->at("alpha_1")();
	_alpha2 = args->at("alpha_2")();
	_dt = computeOptimalTimestep();

	log_console->infoStream() << "DH = " << _dh << " DT = " << _dt;
	log_console->infoStream() 
		<< "epsilon=" << _epsilon 
		<< "\td=" << _dd
		<< "\tk=" << _kk 
		<< "\tmu_1=" << _mu1
		<< "\tmu_2=" << _mu2
		<< "\talpha_1=" << _alpha1
		<< "\talpha_2=" << _alpha2;

	_kernel = cl::Kernel(_program, "computeStep");
}

template <unsigned int nCommandQueues>
DeviceThread<nCommandQueues>::~DeviceThread() {
}

template <unsigned int nCommandQueues>
void DeviceThread<nCommandQueues>::operator()() {

	cl_int err;
	for (int i = 0; i < nCommandQueues; i++) {
		_commandQueues[i] = cl::CommandQueue(_context, _device, CL_QUEUE_PROFILING_ENABLE, &err); 
		CHK_ERRORS(err);
	}

	(*_fence)();

	std::map<std::string, MultiBufferedSubDomain<float,1u>*> currentDomain; 

	while(_simulation->subDomainAvailable()) {
		bool success = _simulation->tryToTakeSubDomain(currentDomain);
		if(success)
			this->initSubDomain(currentDomain);
	}
	
	std::function<void(MultiGpu*)> resetSubDomains(&MultiGpu::resetSubDomains);
	callOnce<void,MultiGpu*>(resetSubDomains, _fence, _simulation);
	
	//Prepare
	bool lockDomains = false;

	//Get as much work as possible, automatic device compute load management
	while(!lockDomains) { 

		//try to get work
		bool success = _simulation->tryToTakeSubDomain(currentDomain);

		if(!success) {
			lockDomains = true;
		}
		else {
			//host & device data management
			std::map<std::string, std::vector<cl::Buffer>> varBuffers;
			std::map<std::string, std::vector<cl::Buffer>> internalEdgeBuffers;
			std::map<std::string, std::vector<cl::Buffer>> externalEdgeBuffers;
			std::map<std::string, float* const**> internalEdgeHostData;
			std::map<std::string, float***> externalEdgeHostData;

			//get subdomain properties
			unsigned int borderSize = currentDomain.begin()->second->borderSize();
			unsigned int idx = currentDomain.begin()->second->idx();
			unsigned int idy = currentDomain.begin()->second->idy();
			unsigned int idz = currentDomain.begin()->second->idz();
			unsigned int nSplitsX = currentDomain.begin()->second->nSubDomainX();
			unsigned int nSplitsY = currentDomain.begin()->second->nSubDomainY();
			unsigned int nSplitsZ = currentDomain.begin()->second->nSubDomainZ();
			unsigned int subDomainWidth = currentDomain.begin()->second->subDomainWidth();
			unsigned int subDomainHeight = currentDomain.begin()->second->subDomainHeight();
			unsigned int subDomainLength = currentDomain.begin()->second->subDomainLength();
			unsigned long bytes = currentDomain.begin()->second->bytes();
			unsigned long edgeBytesX = currentDomain.begin()->second->edgeBytesX();
			unsigned long edgeBytesY = currentDomain.begin()->second->edgeBytesY();
			unsigned long edgeBytesZ = currentDomain.begin()->second->edgeBytesZ();

			unsigned long edgeBytes[3] = {edgeBytesX,edgeBytesY,edgeBytesZ};

			//create buffers on current device, get data pointers on host
			for(auto pairs : currentDomain) {

				//var subdomain (double buffered)
				std::vector<cl::Buffer> varBuffer;
				varBuffer.push_back(cl::Buffer(_context, CL_MEM_READ_WRITE, bytes, &err)); CHK_ERRORS(err);
				varBuffer.push_back(cl::Buffer(_context, CL_MEM_READ_WRITE, bytes, &err)); CHK_ERRORS(err);
				varBuffers.emplace(pairs.first, varBuffer);

				//subdomain edge host data
				//data already allocated in class <Domain>, when needed, some pointers are NULL

				//	 _v__v_		THOSE BORDERS ARE NOT NEEDED
				//	>|__|__|<	-> internal or external
				//	>|__|__|<	-> host pointers are 0 (NULL)
				//	>|__|__|<	-> device buffers are 0 sized
				//     ^ ^

				internalEdgeHostData.emplace(pairs.first, pairs.second->internalEdges());
				externalEdgeHostData.emplace(pairs.first, pairs.second->externalEdges());

				//subdomain edge device data
				//dont allocate buffer if not needed (whole domain edge internal and external buffers)
				std::vector<cl::Buffer> internalEdgeBuffer;
				for (int i = 0; i < 6; i++) {
					float* const* edgeData = internalEdgeHostData[pairs.first][i];
					internalEdgeBuffer.push_back(cl::Buffer(_context, CL_MEM_WRITE_ONLY, edgeData == 0 ? 0 : edgeBytes[i/2], &err)); 
					CHK_ERRORS(err);
				}
				internalEdgeBuffers.emplace(pairs.first, internalEdgeBuffer);

				std::vector<cl::Buffer> externalEdgeBuffer;
				for (int i = 0; i < 6; i++) {
					float* const* edgeData = externalEdgeHostData[pairs.first][i];
					externalEdgeBuffer.push_back(cl::Buffer(_context, CL_MEM_READ_ONLY, edgeData == 0 ? 0 : edgeBytes[i/2], &err)); 
					CHK_ERRORS(err);
				}
				externalEdgeBuffers.emplace(pairs.first, externalEdgeBuffer);
			}

			//keep current domain informations
			_acquiredDomains.push_back(currentDomain); 
			_acquiredDomainsVarBuffers.push_back(varBuffers);
			_acquiredDomainsInternalEdgeBuffers.push_back(internalEdgeBuffers);
			_acquiredDomainsExternalEdgeBuffers.push_back(externalEdgeBuffers);
			_acquiredDomainsInternalEdgeHostData.push_back(internalEdgeHostData);
			_acquiredDomainsExternalEdgeHostData.push_back(externalEdgeHostData);

			//Compute first step for auto workload dispatch
			_acquiredDomainsIsInitialDataSent.push_back(false);
			computeSubDomainStep(_acquiredDomains.size()-1, 0u);
			
			//wait work end
			for (unsigned int j = 0; j < nCommandQueues; j++) {
				_commandQueues[j].finish();
			}
		}
	}
	
	//Print informations about devices
	if(_acquiredDomains.size() == 0) {
		log_console->infoStream() << "Device " << _device() << " is idle !";
	}
	else {
		log_console->infoStream() << "Device " << _device() << " acquired " << _acquiredDomains.size() << " domains !";
	}
	
	std::function<void(MultiGpu*)> initDone(&MultiGpu::initDone);
	callOnce<void,MultiGpu*>(initDone, _fence, _simulation);
	
	//Compute loop
	std::function<void(MultiGpu*)> waitCompute(&MultiGpu::waitCompute);
	std::function<void(MultiGpu*)> lockStep(&MultiGpu::lockStep);
	std::function<void(MultiGpu*)> stepDone(&MultiGpu::stepDone);
	std::function<void(MultiGpu*)> releaseStep(&MultiGpu::releaseStep);
	unsigned int i = 1;
	while(true) {
		callOnce<void,MultiGpu*>(waitCompute, _fence, _simulation);
		callOnce<void,MultiGpu*>(lockStep, _fence, _simulation);

		for (unsigned int j = 0; j < _acquiredDomains.size(); j++) {
			computeSubDomainStep(j, i);
		}
		for (unsigned int j = 0; j < nCommandQueues; j++) {
			_commandQueues[j].finish();
		}

		if(_display)
			callOnce<void,MultiGpu*>(stepDone, _fence, _simulation);
		
		callOnce<void,MultiGpu*>(releaseStep, _fence, _simulation);

		i++;
	}
}

template <unsigned int nCommandQueues>
void DeviceThread<nCommandQueues>::initSubDomain(std::map<std::string, MultiBufferedSubDomain<float, 1u>*> subDomain) {
	log_console->infoStream() << "Thread " << std::this_thread::get_id() 
		<< " took subdomain " << subDomain.begin()->second->id()
		<< " for initialization !";

	for(auto &func : subDomain) {
		func.second->initSubDomain(0);	
	}
}


template <unsigned int nCommandQueues>
void DeviceThread<nCommandQueues>::computeSubDomainStep(unsigned int domainId, unsigned int stepId) {
		
	//get subdomain allocated buffers and host data
	std::map<std::string, MultiBufferedSubDomain<float,1u>*> currentDomain = _acquiredDomains[domainId]; 
	std::map<std::string, std::vector<cl::Buffer>> varBuffers = _acquiredDomainsVarBuffers[domainId];
	std::map<std::string, std::vector<cl::Buffer>> internalEdgeBuffers = _acquiredDomainsInternalEdgeBuffers[domainId];
	std::map<std::string, std::vector<cl::Buffer>> externalEdgeBuffers = _acquiredDomainsExternalEdgeBuffers[domainId];
	std::map<std::string, float* const**> internalEdgeHostData = _acquiredDomainsInternalEdgeHostData[domainId];
	std::map<std::string, float***> externalEdgeHostData = _acquiredDomainsExternalEdgeHostData[domainId];

	//get subdomain properties
	unsigned int borderSize = currentDomain.begin()->second->borderSize();
	unsigned int idx = currentDomain.begin()->second->idx();
	unsigned int idy = currentDomain.begin()->second->idy();
	unsigned int idz = currentDomain.begin()->second->idz();
	unsigned int nSplitsX = currentDomain.begin()->second->nSubDomainX();
	unsigned int nSplitsY = currentDomain.begin()->second->nSubDomainY();
	unsigned int nSplitsZ = currentDomain.begin()->second->nSubDomainZ();
	unsigned int subDomainWidth = currentDomain.begin()->second->subDomainWidth();
	unsigned int subDomainHeight = currentDomain.begin()->second->subDomainHeight();
	unsigned int subDomainLength = currentDomain.begin()->second->subDomainLength();
	unsigned long bytes = currentDomain.begin()->second->bytes();
	unsigned long edgeBytesX = currentDomain.begin()->second->edgeBytesX();
	unsigned long edgeBytesY = currentDomain.begin()->second->edgeBytesY();
	unsigned long edgeBytesZ = currentDomain.begin()->second->edgeBytesZ();
	unsigned long edgeBytes[3] = {edgeBytesX, edgeBytesY, edgeBytesZ};

	cl::CommandQueue currentCommandQueue = _commandQueues[domainId%nCommandQueues];
			
	//Send initial func data
	if(!_acquiredDomainsIsInitialDataSent[domainId]) {
		
		for(auto pair : currentDomain) {
			CHK_ERROR_RET(currentCommandQueue.enqueueWriteBuffer(varBuffers[pair.first][0], CL_FALSE, 0, bytes, *(pair.second->data())));
		}
	}
	_acquiredDomainsIsInitialDataSent[domainId] = true;

	//send extra external borders
	for(auto pair : currentDomain) {
		for (int j = 0; j < 6; j++) {
			float* const* edgeData = externalEdgeHostData[pair.first][j];

			//whole domain borders are not needed (just internal subdomain borders)
			if(edgeData != 0) {
				CHK_ERROR_RET(currentCommandQueue.enqueueWriteBuffer(
							externalEdgeBuffers[pair.first][j], CL_FALSE, 0, edgeBytes[j/2], *edgeData)
						);
			}
		}
	}

	int arg = 0;
	//kernel
	cl::NDRange global((subDomainWidth+31)/32*32, (subDomainHeight+31)/32*32);
	cl::NDRange local(32,32);

	//E
	CHK_ERROR_RET(_kernel.setArg<cl::Memory>(arg++, varBuffers["e"][stepId%2]));
	CHK_ERROR_RET(_kernel.setArg<cl::Memory>(arg++, varBuffers["e"][(stepId+1)%2]));
	for (int j = 0; j < 6; j++) CHK_ERROR_RET(_kernel.setArg<cl::Memory>(arg++, externalEdgeBuffers["e"][j]));
	for (int j = 0; j < 6; j++) CHK_ERROR_RET(_kernel.setArg<cl::Memory>(arg++, internalEdgeBuffers["e"][j]));

	//R
	CHK_ERROR_RET(_kernel.setArg<cl::Memory>(arg++,varBuffers["r"][stepId%2]));
	CHK_ERROR_RET(_kernel.setArg<cl::Memory>(arg++, varBuffers["r"][(stepId+1)%2]));

	//subgrid data
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, borderSize));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, idx));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, idy));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, idz));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, nSplitsX));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, nSplitsY));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, nSplitsZ));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, subDomainWidth));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, subDomainHeight));
	CHK_ERROR_RET(_kernel.setArg<unsigned int>(arg++, subDomainLength));

	//model & simulation data
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _alpha1));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _alpha2));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _mu1));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _mu2));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _kk));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _epsilon));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _dd));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _dh));
	CHK_ERROR_RET(_kernel.setArg<float>(arg++, _dt));

	//compute kernel
	currentCommandQueue.enqueueNDRangeKernel(_kernel, cl::NullRange, global, local);

	//get data to display
	//currentCommandQueue.enqueueReadBuffer(varBuffers["e"][(stepId+1)%2], CL_FALSE, 0, bytes, *(currentDomain["e"]->data()));

	//get slices to generate texture
	if(_display) {
		unsigned int sliceIdx = _simulation->sliceIdX();
		unsigned int sliceIdy = _simulation->sliceIdY();
		unsigned int sliceIdz = _simulation->sliceIdZ();
		unsigned int offsetX = currentDomain.begin()->second->offsetX();
		unsigned int offsetY = currentDomain.begin()->second->offsetY();
		unsigned int offsetZ = currentDomain.begin()->second->offsetZ();
		unsigned int domainWidth = currentDomain.begin()->second->domainWidth();
		unsigned int domainHeight = currentDomain.begin()->second->domainHeight();
		unsigned int domainLength = currentDomain.begin()->second->domainLength();
		unsigned int subDomainBaseWidth = currentDomain.begin()->second->subDomainBaseWidth();
		unsigned int subDomainBaseHeight = currentDomain.begin()->second->subDomainBaseHeight();
		unsigned int subDomainBaseLength = currentDomain.begin()->second->subDomainBaseLength();


		if(sliceIdz >= offsetZ && sliceIdz < offsetZ+subDomainLength) {
			cl::size_t<3> bufferOrigin;
			cl::size_t<3> hostOrigin;
			cl::size_t<3> region;

			const size_t bufferPitch[2] = {subDomainWidth*sizeof(float),subDomainWidth*subDomainHeight*sizeof(float)};
			bufferOrigin.push_back(0u);
			bufferOrigin.push_back(0u);
			bufferOrigin.push_back(sliceIdz%subDomainBaseLength);

			const size_t hostPitch[2] = {domainWidth*sizeof(float), domainWidth*domainHeight*sizeof(float)};
			hostOrigin.push_back(offsetX*sizeof(float));
			hostOrigin.push_back(offsetY);
			hostOrigin.push_back(0);

			region.push_back(subDomainWidth*sizeof(float));
			region.push_back(subDomainHeight);
			region.push_back(1);
			
			for(auto pair : currentDomain) {
				CHK_ERROR_RET(
				currentCommandQueue.enqueueReadBufferRect( 
							varBuffers[pair.first][stepId%2], CL_FALSE, 
							bufferOrigin, hostOrigin, region,
							bufferPitch[0], bufferPitch[1],
							hostPitch[0], hostPitch[1],
							(void*)_simulation->sliceZ()[pair.first])
				);
				//DEBUG
				//for (unsigned int j = 0; j < subDomainHeight; j++) {
				//for (unsigned int i = 0; i < subDomainWidth; i++) {
				//_simulation->sliceZ()[pair.first][j*subDomainWidth+i] = (j*subDomainWidth+i)/(float)(subDomainWidth*subDomainHeight);
				//}
				//}
			}

		}
	
		//actualize internal host borders
		for(auto pair : currentDomain) {
			for (int j = 0; j < 6; j++) {
				float* const* edgeData = internalEdgeHostData[pair.first][j];

				if(*edgeData) {
					CHK_ERROR_RET(currentCommandQueue.enqueueReadBuffer(
								internalEdgeBuffers[pair.first][j], CL_FALSE, 0, 
								edgeBytes[j/2], *edgeData));
				}
			}
		}
			
		//DEBUG
		//log_console->infoStream()
							   //<< "Subdomain " << currentDomain.begin()->second->id()
							   //<< "\n sliceIds " << toStringVec3<unsigned int>(sliceIdx,sliceIdy,sliceIdz)
							   //<< "\n blockIds " << toStringVec3<unsigned int>(idx,idy,idz)
							   //<< "\n domainSize " << toStringDimension<unsigned int>(domainWidth, domainHeight, domainLength)
							   //<< "\n subDomainSize " << toStringDimension<unsigned int>(subDomainWidth, subDomainHeight, subDomainLength)
							   //<< "\n baseSubSomainSize " << toStringDimension<unsigned int>(subDomainBaseWidth, subDomainBaseHeight, subDomainBaseLength)
							   //<< "\n BYTES " << bytes
							   //<< "\n id*base " << toStringVec3<unsigned int>(idx*subDomainBaseWidth, idy*subDomainBaseHeight, idz*subDomainBaseLength)
							   //<< "\n offsets " << toStringVec3<unsigned int>(offsetX, offsetY, offsetZ)
							   //<< "\n buffer origin " << toStringVec3<::size_t>(bufferOrigin[0],bufferOrigin[1],bufferOrigin[2])
							   //<< "\n buffer pitch " << toStringVec3<unsigned int>(bufferPitch[0],bufferPitch[1],0)
							   //<< "\n host origin " << toStringVec3<size_t>(hostOrigin[0],hostOrigin[1],hostOrigin[2])
							   //<< "\n host pitch " << toStringVec3<unsigned int>(hostPitch[0],hostPitch[1],0)
							   //<< "\n region " << toStringVec3<unsigned int>(region[0],region[1],region[2])
							   //<< "\n";
  
			
		
		//TODO
		//if(sliceIdx >= offsetX && sliceIdx < offsetX+subDomainWidth) {
			//bufferOrigin.clear(); hostOrigin.clear(); region.clear();

			//bufferOrigin.push_back(sliceIdx%subDomainBaseWidth);
			//bufferOrigin.push_back(0u);
			//bufferOrigin.push_back(0u);

			//hostOrigin.push_back(idy*subDomainBaseHeight);
			//hostOrigin.push_back(idz*subDomainBaseLength);
			//hostOrigin.push_back(0);

			//region.push_back(1);
			//region.push_back(subDomainHeight);
			//region.push_back(subDomainLength);

			//const size_t hostPitch[2] = {domainHeight, 0};
			
			//currentCommandQueue.enqueueReadBufferRect(
					//varBuffers["e"][(stepId+1)%2], CL_FALSE, 
					//bufferOrigin, hostOrigin, region,
					//bufferPitch[0], bufferPitch[1],
					//hostPitch[0], hostPitch[1],
					//sliceX);
		//}
		//if(sliceIdy >= offsetY && sliceIdy < offsetY+subDomainHeight) {
			//bufferOrigin.clear(); hostOrigin.clear(); region.clear();

			//bufferOrigin.push_back(0u);
			//bufferOrigin.push_back(sliceIdy%subDomainBaseHeight);
			//bufferOrigin.push_back(0u);

			//hostOrigin.push_back(idx*subDomainBaseWidth);
			//hostOrigin.push_back(idz*subDomainBaseLength);
			//hostOrigin.push_back(0);

			//region.push_back(subDomainWidth);
			//region.push_back(1);
			//region.push_back(subDomainLength);

			//const size_t hostPitch[2] = {domainWidth, 0};
			
			//currentCommandQueue.enqueueReadBufferRect(
					//varBuffers["e"][(stepId+1)%2], CL_FALSE, 
					//bufferOrigin, hostOrigin, region,
					//bufferPitch[0], bufferPitch[1],
					//hostPitch[0], hostPitch[1],
					//sliceY);
		//}
	}

}

template <unsigned int nCommandQueues>
float DeviceThread<nCommandQueues>::computeOptimalTimestep() {
	float lhs = SQUARE(_dh) / 
		(4*_dd + _kk*SQUARE(_dh)/4 *(SQUARE(_alpha1+1) + 1));

	float rhs = 1 /
		(_epsilon + _mu1/_mu2 * SQUARE(_dh)/4 * SQUARE(_alpha1 + 1));

	return 0.95*std::min(lhs, rhs);
}
