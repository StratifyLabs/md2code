#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <sapi/sys/Cli.hpp>

#include "ApplicationPrinter.hpp"
#include "Settings.hpp"

class Object {
public:
	Object();
	static sys::Printer & printer(){
		return ApplicationPrinter::printer();
	}

	static Settings & settings(){ return m_settings; }

	static void set_cli(sys::Cli & cli){
		m_cli = &cli;
	}

	static sys::Cli & cli(){
		if( m_cli == nullptr ){
			printer().fatal("cli not set");
			exit(1);
		}
		return *m_cli;
	}

private:

	static sys::Cli * m_cli;
	static Settings m_settings;

};

#endif // OBJECT_HPP
