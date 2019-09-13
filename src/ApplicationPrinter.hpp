#ifndef APPLICATIONPRINTER_HPP
#define APPLICATIONPRINTER_HPP

#include <sapi/sys/Printer.hpp>

class ApplicationPrinter {
public:
	static sys::Printer & printer(){ return m_printer; }

private:
	static sys::Printer m_printer;

};

#endif // APPLICATIONPRINTER_HPP
