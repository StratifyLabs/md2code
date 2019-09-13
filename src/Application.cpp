#include "Application.hpp"
#include <sapi/var.hpp>
#include <sapi/sys.hpp>
#include <sapi/fs.hpp>

#include "MarkdownParser.hpp"
#include "Code.hpp"

Application::Application()
{

}

int Application::run(){
	settings().initialize();

	if( parse_markdown() < 0 ){
		return -1;
	}

	if( generate_code() < 0 ){
		return -1;
	}


	return 0;
}


int Application::parse_markdown(){
	JsonArray markdown = settings().markdown();
	MarkdownParser parser;

	for(u32 i=0; i < markdown.count(); i++){
		JsonObject markdown_entry = markdown.at(i).to_object();
		String input_path =  markdown_entry.at(
					arg::JsonKey("input")
					).to_string();

		String output_path =  markdown_entry.at(
					arg::JsonKey("output")
					).to_string();

		output_path << "/snippets";

		if( Dir::create(
				 arg::DestinationDirectoryPath(output_path),
				 Permissions::all_access(),
				 arg::IsRecursive(true)
				 ) < 0 ){

			printer().error(
						"failed to create output directory '%s'",
						output_path.cstring()
						);
		}

		Vector<String> input_file_list =
				Dir::read_list(
					arg::SourceDirectoryPath(input_path)
					);

		for(auto file_path: input_file_list){

			if( file_path == "." ){ continue; }
			if( file_path == ".." ){ continue; }

			JsonObject code_blocks =
					parser.extract_code_snippets(
						arg::SourceFilePath(
							String()
							<< input_path
							<< "/"
							<< file_path
							),
						markdown_entry
						);

			if( code_blocks.is_empty() == false ){
				String output_file_path;

				file_path.replace(
							arg::StringToErase(".md"),
							arg::StringToInsert("")
							);

				output_file_path
						<< output_path
						<< "/"
						<< file_path
						<< ".json";

				JsonDocument document;

				document.set_flags(JsonDocument::INDENT_2);

				if( document.save(
						 arg::SourceJsonValue(code_blocks),
						 arg::DestinationFilePath(output_file_path)
						 ) < 0 ){
					printer().error(
								"failed to save snippet file as '%s'",
								output_file_path.cstring()
								);
				}
			}
		}
	}
	return 0;
}

int Application::generate_code(){
	JsonArray markdown = settings().markdown();
	Code code;

	for(u32 i=0; i < markdown.count(); i++){
		JsonObject markdown_entry = markdown.at(i).to_object();
		String output_path =	markdown_entry.at(
					arg::JsonKey("output")
					).to_string();

		String snippets_path =
				String()
				<< output_path
				<< "/snippets";

		Vector<String> snippet_files =
				Dir::read_list(
					arg::SourceDirectoryPath(snippets_path)
					);

		for(auto input_file_name: snippet_files){

			if( input_file_name == "." ){ continue; }
			if( input_file_name == ".." ){ continue; }

			String input_file_path;
			input_file_path
					<< snippets_path
					<< "/"
					<< input_file_name;

			printer().debug(
						"generate code for '%s'",
						input_file_path.cstring()
						);

			code.generate(
						arg::SourceFilePath(input_file_path)
						);

		}

	}

	return 0;
}

int Application::build_code(){

	return 0;
}
