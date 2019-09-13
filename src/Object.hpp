#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "ApplicationPrinter.hpp"
#include "Settings.hpp"

class Object {
public:
	Object();
	static sys::Printer & printer(){
		return ApplicationPrinter::printer();
	}

	static Settings & settings(){ return m_settings; }

private:

	static Settings m_settings;

};

#endif // OBJECT_HPP
