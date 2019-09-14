#include <sapi/fs.hpp>
#include <sapi/var.hpp>
#include <sapi/calc.hpp>

#include "MarkdownParser.hpp"

MarkdownParser::MarkdownParser(){

}


var::JsonObject MarkdownParser::extract_code_snippets(
		arg::SourceFilePath file_path,
		var::JsonObject properties
		){

	File markdown_input_file;

	if( markdown_input_file.open(
			 arg::FilePath(file_path.argument()),
			 OpenFlags::read_only()
			 ) < 0 ){
		printer().error(
					"failed to open markdown source file '%s'",
					file_path.argument().cstring()
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
			 (line.find(
				  arg::StringToFind("```")
				  ) == String::npos)
			 ){
			if( code_block_line == 0 ){
				code_block_header = line;
				start_code_line = line_number;
			}
			code_block_string << line;
			code_block_line++;
		}

		if( line.find(
				 arg::StringToFind("```")
				 ) == 0
			 ){
			if( is_on_code_block ){
				//this is the end of the code block
				JsonObject code_block_object;
				Tokenizer header(
							arg::TokenEncodedString(code_block_header),
							arg::TokenDelimeters(":\n")
							);

				if( (header.count() >= 2) &&
					 header.at(0).find(
						 arg::StringToFind("//md2code")
						 ) != String::npos ){

					//header is : //md2code:<section>

					code_block_object.insert(
								arg::JsonKey("section"),
								JsonString(header.at(1))
								);

					code_block_object.insert(
								arg::JsonKey("line"),
								JsonInteger(start_code_line)
								);


					DataReference code_block_data_reference =
							DataReference(
								arg::ReadOnlyBuffer(code_block_string.to_const_void()),
								arg::Size(code_block_string.length())
								);

					code_block_object.insert(
								arg::JsonKey("code"),
								JsonString(Base64::encode(
												  arg::SourceData(code_block_data_reference)
												  )
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



		String name = File::name(file_path.argument());

		name.replace(
					arg::StringToErase(".md"),
					arg::StringToInsert("")
					);

		result.insert(
					arg::JsonKey("name"),
					JsonString(name)
					);

		result.insert(
					arg::JsonKey("destinationDirectory"),
					JsonString(
						properties.at(
							arg::JsonKey("output")
							).to_string() << "/code/" << name
						)
					);

		result.insert(
					arg::JsonKey("source"),
					JsonString(file_path.argument())
					);

		result.insert(
					arg::JsonKey("template"),
					properties.at(
						arg::JsonKey("template")
						)
					);

		result.insert(
					arg::JsonKey("buildDirectory"),
					properties.at(
						arg::JsonKey("buildDirectory")
						)
					);

		result.insert(
					arg::JsonKey("cmakeOptions"),
					properties.at(
						arg::JsonKey("cmakeOptions")
						)
					);

		result.insert(
					arg::JsonKey("makeOptions"),
					properties.at(
						arg::JsonKey("makeOptions")
						)
					);

		result.insert(
					arg::JsonKey("snippets"),
					code_block_array
					);
	}

	return result;
}
