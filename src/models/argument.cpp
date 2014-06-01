
#include "argument.hpp"
#include <cassert>

Argument::Argument(Value defaultVal_, Value minVal_, Value maxVal_, WidgetType preferredWidgetType_) :
	_val(defaultVal_), _defaultVal(defaultVal_), _minVal(minVal_), _maxVal(maxVal_),
	_preferredWidgetType(preferredWidgetType_)
{
	ValType	type_ = defaultVal_.type();
	assert(minVal_.type() == type_ && maxVal_.type() == type_);
}

Argument::~Argument() {
}

ValType Argument::type() const {
	return _val.type();
}
		
WidgetType Argument::preferredWidgetType() const {
	return _preferredWidgetType;
}

Value Argument::val() const {
	return _val;
}

Value Argument::minVal() const {
	return _minVal;
}

Value Argument::maxVal() const {
	return _maxVal;
}

Value Argument::defaultVal() const {
	return _defaultVal;
}

Value &Argument::operator ()() {
	return _val;
}

const Value Argument::operator ()() const {
	return _val;
}
