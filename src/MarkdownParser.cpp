#include <sapi/fs.hpp>
#include <sapi/var.hpp>
#include <sapi/calc.hpp>

#include "MarkdownParser.hpp"

MarkdownParser::MarkdownParser(){

}


var::JsonObject MarkdownParser::extract_code_snippets(
		const var::String & file_path,
		var::JsonObject properties
		){

	File markdown_input_file;

	if( markdown_input_file.open(
			 file_path,
			 OpenFlags::read_only()
			 ) < 0 ){
		printer().error(
					"failed to open markdown source file '%s'",
					file_path.cstring()
					);
		return JsonObject();
	}

	String line;
	int line_number = 1;
	int start_code_line = 1;
	int code_block_line = 0;
	bool is_on_code_block = false;
	String code_block_string;
	String code_block_header;
	JsonObject result;
	JsonArray code_block_array;

	while(
			(line = markdown_input_file.gets()).is_empty() == false
			){

		if( is_on_code_block &&
			 (line.find("```") == String::npos)
			 ){
			if( code_block_line == 0 ){
				code_block_header = line;
				start_code_line = line_number;
			}
			code_block_string << line;
			code_block_line++;
		}

		if( line.find("```") == 0
			 ){
			if( is_on_code_block ){
				//this is the end of the code block
				JsonObject code_block_object;
				Tokenizer header(
							code_block_header,
							Tokenizer::Delimeters(":\n")
							);

				if( (header.count() >= 2) &&
					 header.at(0).find("//md2code") != String::npos ){

					//header is : //md2code:<section>

					code_block_object.insert(
								"section",
								JsonString(header.at(1))
								);

					code_block_object.insert(
								"line",
								JsonInteger(start_code_line)
								);

					code_block_object.insert(
								"code",
								JsonString(
									Base64::encode(code_block_string)
									)
								);



					code_block_array.append(code_block_object);
				}
			} else {
				code_block_line = 0;
				code_block_string.clear();
			}

			is_on_code_block = !is_on_code_block;
		}
		line_number++;
	}

	if( code_block_array.count() > 0 ){



		String name = File::name(file_path);

		name.replace(
					String::ToErase(".md"),
					String::ToInsert("")
					);

		result.insert(
					"name",
					JsonString(name)
					);

		result.insert(
					"destinationDirectory",
					JsonString(
						properties
						.at("output")
						.to_string() << "/code/" << name
						)
					);

		result.insert(
					"source",
					JsonString(file_path)
					);

		result.insert(
					"template",
					properties.at("template")
					);

		result.insert(
					"buildDirectory",
					properties.at("buildDirectory")
					);

		result.insert(
					"cmakeOptions",
					properties.at("cmakeOptions")
					);

		result.insert(
					"makeOptions",
					properties.at("makeOptions")
					);

		result.insert(
					"snippets",
					code_block_array
					);
	}

	return result;
}
