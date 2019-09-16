# md2code

md2code is a C++ application that will pull C/C++ code snippets out of markdown. The purpose is to allow the compilation of code snippets so that they can be posted free of compilation errors.

The snippets are placed into a directory structure that can be used to build and optionally execute the code.

The snippet must be marked as follows

```
//md2code:<section>
```

For example:

```
//md2code:include
#include <sapi/var.hpp>
```

```
//md2code:main
Data a = Data(arg::Size(64));
```

Becomes:

```
//md2code:include
#include <sapi/var.hpp>

int main(int argc, char * argv[]){
  {
    //md2code:main
    Data a = Data(arg::Size(64));
  }
  return 0;
}
```
