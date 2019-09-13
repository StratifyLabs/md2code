#ifndef MARKDOWNPARSER_HPP
#define MARKDOWNPARSER_HPP

#include "Object.hpp"

class MarkdownParser : public Object {
public:
	MarkdownParser();

	var::JsonObject extract_code_snippets(
			arg::SourceFilePath file_path,
			var::JsonObject properties
			);


private:


};

#endif // MARKDOWNPARSER_HPP
