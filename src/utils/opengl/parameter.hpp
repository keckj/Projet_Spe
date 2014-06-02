#ifndef PARAMETER_H
#define PARAMETER_H

#include "headers.hpp"

enum ParamType {F,I,IV,FV};

union ParamData {
	int i;
	float f;
	int *iv;
	float *fv;
};

class Parameter {
	public:
		explicit Parameter(GLenum paramName_, int param_);
		explicit Parameter(GLenum paramName_, float param_);

		explicit Parameter(GLenum paramName_, int *params_);
		explicit Parameter(GLenum paramName_, float *params_);

		ParamType type() const;
		GLenum paramName() const;
		ParamData params() const;

	private:
		ParamType _type;
		GLenum _paramName;
		ParamData _params;
};

#endif /* end of include guard: PARAMETER_H */
