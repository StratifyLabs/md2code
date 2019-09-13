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
			arg::DestinationDirectoryPath destination_path
			);

};

#endif // CODE_HPP
