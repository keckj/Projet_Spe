

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include "value.hpp"

enum WidgetType {
	SLIDER=0,
	BOX,
	CHECKBOX
};

class Argument {
	
	public:
		Argument(Value defaultVal, Value minVal, Value maxVal, WidgetType type);
		~Argument();

		//get and set value
		const Value operator ()() const;
		Value &operator ()();

		Value val() const;
		Value defaultVal() const;
		Value minVal() const;
		Value maxVal() const;
		
		WidgetType preferredWidgetType() const;
		ValType type() const;
	
	private:
		Value _val, _defaultVal, _minVal, _maxVal;
		WidgetType _preferredWidgetType;
};


#endif /* end of include guard: ARGUMENTS_H */
