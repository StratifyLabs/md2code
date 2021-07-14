#include "Code.hpp"
#include <sapi/var.hpp>
#include <sapi/calc.hpp>
#include <sapi/fs.hpp>

Code::Code(){

}


int Code::generate(const var::String & snippets_path){

	JsonObject project_object = JsonDocument().load(
				fs::File::Path(snippets_path)
				).to_object();

	JsonArray snippets_array =
			project_object.at("snippets").to_array();

	printer().debug(
				"loaded %d snippets from '%s'",
				snippets_array.count(),
				snippets_path.cstring()
				);

	String destination_directory =
			project_object.at("destinationDirectory").to_string();

	printer().debug(
				"generate code in destination directory %s",
				destination_directory.cstring()
				);

	String build_directory =
			project_object.at("buildDirectory").to_string();

	printer().debug(
				"build code in directory %s",
				build_directory.cstring()
				);

	create_code_project(
				fs::File::SourcePath(
					project_object.at("template").to_string()
					),
				fs::File::DestinationPath(destination_directory)
				);

	insert_code_snippets(
				snippets_array,
				destination_directory
				);

	String is_build = cli().get_option(
				"build",
				sys::Cli::Description("Do not build the output code")
				);

	if( is_build.is_empty() ){
		is_build = "true";
	}

	if( is_build == "true" ){

		printf("\n--------------------Start Build Output-------------------\n");
		build_code(
					String()
					<< destination_directory
					<< "/"
					<< build_directory
					);
		printf("\n--------------------End Build Output-------------------\n");
	}

	return 0;
}

int Code::create_code_project(
		fs::File::SourcePath template_path,
		fs::File::DestinationPath destination_path
		){

	printer().debug(
				"create code in directory '%s'",
				destination_path.argument().cstring()
				);

	if( Dir::create(
			 destination_path.argument(),
			 Permissions::all_access(),
			 Dir::IsRecursive(true)
			 ) < 0 ){
		printer().error(
					"failed to create directory for '%s'",
					destination_path.argument().cstring()
					);
	}

	//recursive copy the template path to the destination path
	if( Dir::copy(
			 fs::File::SourcePath(template_path),
			 fs::File::DestinationPath(destination_path)
			 ) < 0 ){

		printer().error(
					"failed to create destination directory '%s' from template path '%s'",
					destination_path.argument().cstring(),
					template_path.argument().cstring()
					);

		return -1;
	}


	return 0;
}

int Code::insert_code_snippets(const JsonValue & code_snippets,
										 const var::String & destination_path
										 ){

	JsonArray snippets_array = code_snippets.to_array();

	File main_file;
	String main_content;
	String main_file_path;

	main_file_path =
			String()
			<< destination_path
			<< "/src/main.cpp";

	if( main_file.open(
			 main_file_path,
			 OpenFlags::read_only()
			 ) < 0 ){

		printer().error(
					"failed to open main.cpp file in '%s/src'",
					destination_path.cstring()
					);

		return -1;
	}

	String line;

	printer().debug(
				"insert %d snippets in '%s'",
				snippets_array.count(),
				main_file_path.cstring()
				);

	while(
			((line = main_file.gets()).is_empty() == false)
			&& line.find("//md2code:include") == String::npos
			){
		main_content << line;
	}

	if( line.is_empty() ){
		printer().error("no '//md2code:include' marker in template");
		return -1;
	}

	for(u32 i=0; i < snippets_array.count(); i++){
		JsonObject snippet_object = snippets_array.at(i).to_object();

		String section =
				snippet_object.at("section").to_string();

		if( section == "include" ){
			String encoded_string =
					snippet_object.at("code").to_string();

			printer().debug(
						"insert block in include - '%s'",
						encoded_string.cstring()
						);

			Data code_block_data = Base64::decode(encoded_string);
			main_content << String(code_block_data);
		}
	}

	while(
			((line = main_file.gets()).is_empty() == false)
			&& line.find("//md2code:main") == String::npos
			){
		main_content << line;
	}

	if( line.is_empty() ){
		printer().error("no '//md2code:main' marker in template");
		return -1;
	}

	for(u32 i=0; i < snippets_array.count(); i++){
		JsonObject snippet_object = snippets_array.at(i).to_object();

		String section =
				snippet_object.at("section").to_string();

		if( section == "main" ){
			String encoded_string =
					snippet_object.at("code").to_string();

			printer().debug(
						"insert block in main - '%s'",
						encoded_string.cstring()
						);

			Data code_block_data = Base64::decode(encoded_string);
			main_content << "  {\n";
			String code_block_string = String(code_block_data);
			code_block_string.replace(
						String::ToErase("\n"),
						String::ToInsert("\n    ")
						);
			main_content << "    " << code_block_string;
			main_content << "\n  }\n";
		}
	}

	while(
			((line = main_file.gets()).is_empty() == false)
			){
		main_content << line;
	}

	main_file.close();

	if( main_file.create(
			 String()
			 << destination_path
			 << "/src/main.cpp",
			 File::IsOverwrite(true)
			 ) < 0 ){

		printer().error(
					"failed to open main.cpp file in '%s/src'",
					destination_path.cstring()
					);

		return -1;
	}

	main_file << main_content;
	main_file.close();

	return 0;

}

int Code::build_code(const var::String & build_directory
							){

	//make sure the build directory exists

	if( Dir::exists(build_directory) == false ){
		if( Dir::create(build_directory) < 0 ){
			printer().warning(
						"failed to create build directory '%s'",
						build_directory.cstring()
						);
			return -1;
		}
	}

	String execute;

	execute << "cmake -E chdir " << build_directory << " cmake ";

#if 0
	if( api::ApiInfo::is_windows() ){
		if( is_msys() == true ){
			printer().debug("setting MSYS Makefiles generator");
			execute << "-G \"MSYS Makefiles\" ";
		} else {
			printer().debug("setting MinGW Makefiles generator");
			execute << "-G \"MinGW Makefiles\" ";
		}
	}
#endif

	execute << "..";
	system(execute.cstring());
	execute.clear();

	execute << "cmake --build " << build_directory <<  " -- -j8";
	system(execute.cstring());
	execute.clear();

	return 0;
}
