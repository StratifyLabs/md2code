#include "Code.hpp"
#include <sapi/var.hpp>
#include <sapi/calc.hpp>
#include <sapi/fs.hpp>

Code::Code(){

}


int Code::generate(
		arg::SourceFilePath snippets_path
		){

	JsonObject project_object = JsonDocument().load(
				snippets_path
				).to_object();

	JsonArray snippets_array =
			project_object.at(
				arg::JsonKey("snippets")
				).to_array();

	printer().debug(
				"loaded %d snippets from '%s'",
				snippets_array.count(),
				snippets_path.argument().cstring()
				);

	String destination_directory =
			project_object.at(
				arg::JsonKey("destinationDirectory")
				).to_string();

	String build_directory =
			project_object.at(
				arg::JsonKey("buildDirectory")
				).to_string();

	create_code_project(
				arg::SourceDirectoryPath(
					project_object.at(
						arg::JsonKey("template")
						).to_string()
					),
				arg::DestinationDirectoryPath(destination_directory)
				);

	insert_code_snippets(
				arg::SourceJsonValue(
					snippets_array
					),
				arg::DestinationDirectoryPath(destination_directory)
				);

	String is_build = cli().get_option(
				arg::OptionName("build"),
				arg::OptionDescription("Do not build the output code")
				);

	if( is_build.is_empty() ){
		is_build = "true";
	}

	if( is_build == "true" ){

		printf("\n--------------------Start Build Output-------------------\n");
		build_code(
					arg::SourceDirectoryPath(
						String()
						<< destination_directory
						<< "/"
						<< build_directory
						)
					);
		printf("\n--------------------End Build Output-------------------\n");
	}

	return 0;
}

int Code::create_code_project(
		arg::SourceDirectoryPath template_path,
		arg::DestinationDirectoryPath destination_path
		){

	printer().debug(
				"create destination directory '%s'",
				destination_path.argument().cstring()
				);

	if( Dir::create(
			 arg::DestinationDirectoryPath(
				 destination_path.argument()
				 ),
			 Permissions::all_access(),
			 arg::IsRecursive(true)
			 ) < 0 ){
		printer().error(
					"failed to create directory for '%s'",
					destination_path.argument().cstring()
					);
	}

	//recursive copy the template path to the destination path
	if( Dir::copy(
			 arg::SourceDirectoryPath(template_path),
			 arg::DestinationDirectoryPath(destination_path)
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

int Code::insert_code_snippets(
		arg::SourceJsonValue code_snippets,
		arg::DestinationDirectoryPath destination_path
		){

	JsonArray snippets_array = code_snippets.argument().to_array();

	File main_file;
	String main_content;

	if( main_file.open(
			 arg::FilePath(
				 String()
				 << destination_path.argument()
				 << "/src/main.cpp"),
			 OpenFlags::read_only()
			 ) < 0 ){

		printer().error(
					"failed to open main.cpp file in '%s/src'",
					destination_path.argument().cstring()
					);

		return -1;
	}

	String line;
	while(
			((line = main_file.gets()).is_empty() == false)
			&& line.find(
				arg::StringToFind("//md2code:include")
				) == String::npos
			){
		main_content << line;
	}

	if( line.is_empty() ){
		printer().error("no '//md2code:include' marker in template");
		return -1;
	}

	for(u32 i=0; i < snippets_array.count(); i++){
		JsonObject snippet_object = snippets_array.at(i).to_object();

		String type = snippet_object.at(
					arg::JsonKey("type")
					).to_string();

		if( type == "include" ){
			String encoded_string = snippet_object.at(
						arg::JsonKey("code")
						).to_string();

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
			&& line.find(
				arg::StringToFind("//md2code:main")
				) == String::npos
			){
		main_content << line;
	}

	if( line.is_empty() ){
		printer().error("no '//md2code:main' marker in template");
		return -1;
	}

	for(u32 i=0; i < snippets_array.count(); i++){
		JsonObject snippet_object = snippets_array.at(i).to_object();

		String type = snippet_object.at(
					arg::JsonKey("type")
					).to_string();

		if( type == "main" ){
			String encoded_string = snippet_object.at(
						arg::JsonKey("code")
						).to_string();

			printer().debug(
						"insert block in main - '%s'",
						encoded_string.cstring()
						);

			Data code_block_data = Base64::decode(encoded_string);
			main_content << "{\n";
			main_content << String(code_block_data);
			main_content << "}\n";
		}
	}

	while(
			((line = main_file.gets()).is_empty() == false)
			){
		main_content << line;
	}

	main_file.close();

	if( main_file.create(
			 arg::DestinationFilePath(
				 String()
				 << destination_path.argument()
				 << "/src/main.cpp"),
			 arg::IsOverwrite(true)
			 ) < 0 ){

		printer().error(
					"failed to open main.cpp file in '%s/src'",
					destination_path.argument().cstring()
					);

		return -1;
	}

	main_file << main_content;
	main_file.close();

	return 0;

}

int Code::build_code(
		arg::SourceDirectoryPath build_directory
		){

	//make sure the build directory exists

	if( Dir::exists(
			 arg::SourceDirectoryPath(build_directory.argument())
			 ) == false ){
		if( Dir::create(
				 arg::DestinationDirectoryPath(build_directory.argument())
				 ) < 0 ){
			printer().warning(
						"failed to create build directory '%s'",
						build_directory.argument().cstring()
						);
			return -1;
		}
	}

	String execute;

	execute << "cmake -E chdir " << build_directory.argument() << " cmake ";

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

	execute << "cmake --build " << build_directory.argument() <<  " -- -j8";
	system(execute.cstring());
	execute.clear();

	return 0;
}
