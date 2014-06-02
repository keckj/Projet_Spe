
#include "parameter.hpp"

Parameter::Parameter(GLenum paramName_, int param_) :
	_type(ParamType::I), _paramName(paramName_)
{
	_params.i = param_;
}

Parameter::Parameter(GLenum paramName_, float param_) :
	_type(ParamType::F), _paramName(paramName_)
{
	_params.f = param_;
}

Parameter::Parameter(GLenum paramName_, int *params_) :
	_type(ParamType::IV), _paramName(paramName_)
{
	_params.iv = params_;
}

Parameter::Parameter(GLenum paramName_, float *params_) :
	_type(ParamType::FV), _paramName(paramName_)
{
	_params.fv = params_;
}
		
ParamType Parameter::type() const {
	return _type;
}

GLenum Parameter::paramName() const {
	return _paramName;
}

ParamData Parameter::params() const {
	return _params;
}
