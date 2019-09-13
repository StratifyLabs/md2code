#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Object.hpp"

class Application : public Object {
public:
	Application();

	int run();

private:

	int parse_markdown();
	int generate_code();
	int build_code();

};

#endif // APPLICATION_HPP
