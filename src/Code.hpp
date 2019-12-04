#ifndef CODE_HPP
#define CODE_HPP

#include "Object.hpp"

class Code : public Object {
public:
	Code();

	int generate(const var::String & snippets_path);

private:
	int create_code_project(
			fs::File::SourcePath template_path,
			fs::File::DestinationPath destination_path
			);

	int insert_code_snippets(
			const var::JsonValue & code_snippets,
			const var::String & destination_path
			);

	int build_code(
			const var::String & build_directory
			);

};

#endif // CODE_HPP
