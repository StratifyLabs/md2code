#include "Code.hpp"
#include <sapi/var.hpp>
#include <sapi/calc.hpp>
#include <sapi/fs.hpp>

Code::Code()
{

}


int Code::generate(
		arg::SourceFilePath snippets_path
		){

	JsonArray snippets_array = JsonDocument().load(
				snippets_path
				).to_object().at(
				arg::JsonKey("codeBlocks")
				).to_array();

	printer().debug(
				"loaded %d snippets from '%s'",
				snippets_array.count(),
				snippets_path.argument().cstring()
				);

	for(u32 i=0; i < snippets_array.count(); i++){

		JsonObject snippet_object =
				snippets_array.at(i).to_object();

		String language = snippet_object.at(
					arg::JsonKey("language")
					).to_string();


		String encoded_code_string =
				snippet_object.at(
					arg::JsonKey("code")
					).to_string();

		printer().key(
					"encoding",
					encoded_code_string
					);

		Data code_data_block =
				Base64::decode(
					arg::Base64EncodedString(
						encoded_code_string
						)
					);

		String code_block(code_data_block);
		printer() << code_block;


	}
	return 0;
}

int Code::create_code_project(
		arg::DestinationDirectoryPath destination_path
		){


	if( Dir::create(
			 arg::DestinationDirectoryPath(
				 String()
				 << destination_path.argument()
				 << "/src"
				 ),
			 Permissions::all_access(),
			 arg::IsRecursive(true)
			 ) < 0 ){
		printer().error(
					"failed to create directory for '%s'",
					destination_path.argument().cstring()
					);
	}



}
