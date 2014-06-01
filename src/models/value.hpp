
#ifndef VALUE_H
#define VALUE_H

enum ValType {
	BOOL=0,
	INT,
	UNSIGNED_INT,
	LONG,
	UNSIGNED_LONG,
	FLOAT,
	DOUBLE,
	NO_TYPE
};

union ValData {
	bool b;
	int i;
	unsigned int ui;
	long l;
	unsigned long ul;
	float f;
	double d;
};

class Value {

	public:

		Value() {_type = ValType::NO_TYPE;}
		Value(const Value &val_) {_type = val_.type(); _val= val_();}

		Value(bool val_) {_type = ValType::BOOL, _val.b = val_;}
		Value(int val_) {_type = ValType::INT, _val.i = val_;}
		Value(unsigned int val_) {_type = ValType::UNSIGNED_INT, _val.ui = val_;}
		Value(long val_) {_type = ValType::LONG, _val.l = val_;}
		Value(unsigned long val_) {_type = ValType::UNSIGNED_LONG, _val.ul = val_;}
		Value(float val_) {_type = ValType::FLOAT, _val.f = val_;}
		Value(double val_) {_type = ValType::DOUBLE, _val.d = val_;}

		~Value() {};

		const ValData operator()() const {return _val;}
		ValData &operator()() {return _val;}

		ValType type() const {return _type;}

		operator bool() {return _val.b;}
		operator int() {return _val.i;}
		operator unsigned int() {return _val.ui;}
		operator long() {return _val.l;}
		operator unsigned long() {return _val.ul;}
		operator float() {return _val.f;}
		operator double() {return _val.d;}

	private:
		ValData _val;
		ValType _type;
};




#endif /* end of include guard: VALUE_H */
