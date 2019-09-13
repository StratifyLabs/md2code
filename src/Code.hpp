#ifndef CODE_HPP
#define CODE_HPP

#include "Object.hpp"

class Code : public Object {
public:
	Code();

	int generate(
			arg::SourceFilePath snippets_path
			);

private:
	int create_code_project(
			arg::SourceDirectoryPath template_path,
			arg::DestinationDirectoryPath destination_path
			);

	int insert_code_snippets(
			arg::SourceJsonValue code_snippets,
			arg::DestinationDirectoryPath destination_path
			);

	int build_code(
			arg::SourceDirectoryPath build_directory
			);

};

#endif // CODE_HPP
