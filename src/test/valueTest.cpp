
#include "headers.hpp"
#include "value.hpp"
#include "argument.hpp"
#include "log.hpp"

#include <cassert>
#include <vector>

#if _N_MAIN == 4

using namespace log4cpp;
using namespace utils;

int main(int argc, char** argv) {

	log4cpp::initLogs();

	std::cout <<   "=========================" << std::endl;
	std::cout << "\n== VALUE TEST PROGRAMM ==" << std::endl;
	std::cout << "\n=========================" << std::endl;
	{
		Value v;
		Value vb(true);
		Value vi(0);
		Value vui(1u);
		Value vl(2l);
		Value vul(3ul);
		Value vf(4.0f);
		Value vd(5.0);

		bool b;
		b = v;
		b = vb;
		assert(b == true);
		b = vi;
		b = vui;
		b = vl;
		b = vul;
		b = vf;
		b = vd;

		int i;
		i = v;
		i = vb;
		i = vi;
		assert(i == 0);
		i = vui;
		i = vl;
		i = vul;
		i = vf;
		i = vd;

		unsigned int ui;
		ui = v;
		ui = vb;
		ui = vi;
		ui = vui;
		assert(ui == 1u);
		ui = vl;
		ui = vul;
		ui = vf;
		ui = vd;

		long l;
		l = v;
		l = vb;
		l = vi;
		l = vui;
		l = vl;
		assert(l == 2l);
		l = vf;
		l = vd;

		unsigned long ul;
		ul = v;
		ul = vb;
		ul = vi;
		ul = vui;
		ul = vl;
		ul = vul;
		assert(ul == 3ul);
		ul = vf;
		ul = vd;

		float f;
		f = v;
		f = vb;
		f = vi;
		f = vui;
		f = vl;
		f = vul;
		f = vf;
		f = vd;

		double d;
		d = v;
		d = vb;
		d = vi;
		d = vui;
		d = vl;
		d = vul;
		d = vf;
		d = vd;

		if(b || i || ui || l || ul || f>0.0f || d>0.0f) {;}
		std::cout << "get value : ok" << std::endl;

		v = true;
		assert(v.type() == ValType::BOOL);
		assert(bool(v) == true);
		v = 1;
		assert(v.type() == ValType::INT);
		assert(int(v) == 1);
		v = 1u;
		assert(v.type() == ValType::UNSIGNED_INT);
		assert((unsigned int)(v) == 1u);
		v = 1l;
		assert(v.type() == ValType::LONG);
		assert(long(v) == 1u);
		v = 1ul;
		assert(v.type() == ValType::UNSIGNED_LONG);
		assert((unsigned long)(v) == 1u);
		v = 1.0f;
		assert(v.type() == ValType::FLOAT);
		v = 1.0;
		assert(v.type() == ValType::DOUBLE);

		std::cout << "set value : ok" << std::endl;
	}

	std::cout << "\n\n============================" << std::endl;
	std::cout <<   "\n== ARGUMENT TEST PROGRAMM ==" << std::endl;
	std::cout <<   "\n============================" << std::endl;
	
	{
		Argument ab(true, false, true, WidgetType::CHECKBOX);
		Argument ai(50, -100, 100, WidgetType::CHECKBOX);
		Argument aui(50u, 0u, 100u, WidgetType::CHECKBOX);
		Argument al(50l, -100l, 100l, WidgetType::CHECKBOX);
		Argument aul(50ul, -100ul, 100ul, WidgetType::CHECKBOX);
		Argument af(50.0f, -100.0f, 100.0f, WidgetType::CHECKBOX);
		Argument ad(50.0, -100.0, 100.0, WidgetType::CHECKBOX);
		
		std::cout << "constructors : ok" << std::endl;
		
		assert(ab.type() == ValType::BOOL);
		assert(bool(ab()) == true);
		ab() = false;
		assert(bool(ab()) == false);
		
		assert(ai.type() == ValType::INT);
		assert(int(ai()) == 50);
		ai() = 51;
		assert(int(ai()) == 51);
		
		assert(aui.type() == ValType::UNSIGNED_INT);
		assert((unsigned int)(aui()) == 50u);
		aui() = 51;
		assert((unsigned int)(aui()) == 51u);
		
		assert(al.type() == ValType::LONG);
		assert(int(al()) == 50l);
		al() = 51;
		assert(int(al()) == 51l);
		
		assert(aul.type() == ValType::UNSIGNED_LONG);
		assert((unsigned int)(aul()) == 50ul);
		aul() = 51ul;
		assert((unsigned int)(aul()) == 51ul);
		
		assert(af.type() == ValType::FLOAT);
		af() = 51.0f;
	
		assert(ad.type() == ValType::DOUBLE);
		ad() = 51.0;
		
		std::cout << "affect value : ok" << std::endl;
	}
}

#endif
