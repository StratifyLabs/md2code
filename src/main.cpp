#include <cstdio>

#include <sapi/fs.hpp>
#include <sapi/sys.hpp>
#include <sapi/var.hpp>
#include "ApplicationPrinter.hpp"
#include "Application.hpp"

int main(int argc, char * argv[]){
	sys::Cli cli(argc, argv);
	cli.set_publisher("Stratify Labs, Inc");
	cli.handle_version();

	ApplicationPrinter::printer().set_verbose_level(
				cli.get_option(
					arg::OptionName("verbose"),
					arg::OptionDescription("verbosity level use <debug|message|info|warning|error>")
					)
				);


	Application().run();





	return 0;
}

