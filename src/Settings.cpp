#include "Settings.hpp"

#include <sapi/var.hpp>

Settings::Settings(){

}

int Settings::initialize(){

	printer().debug(
				"load settings from '%s'",
				settings_file_name().cstring()
				);

	JsonDocument document;
	m_settings = document.load(
				fs::File::Path(settings_file_name())
				).to_object();

	if( m_settings.is_empty() ){
		printer().debug("error is '%s'",
							 document.error().text().cstring()
							 );
		printer().error(
					"failed to load settings file '%s'",
					settings_file_name().cstring()
					);
		return -1;
	}

	printer().output() << m_settings;

	return 0;
}
