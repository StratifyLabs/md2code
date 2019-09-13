#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "ApplicationPrinter.hpp"
#include <sapi/var/ConstString.hpp>
#include <sapi/var/Json.hpp>

class Settings : public ApplicationPrinter {
public:
	Settings();
	int initialize();

	var::JsonArray markdown(){
		return m_settings.at(
					arg::JsonKey("markdown")
					).to_array();
	}

private:
	var::JsonObject m_settings;

	const var::ConstString settings_file_name(){
		return "md2code.json";
	}
};

#endif // SETTINGS_HPP
