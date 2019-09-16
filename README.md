# md2code

md2code is a C++ application that will pull C/C++ code snippets out of markdown and build them as test programs. The purpose is to allow the compilation of all documentation code snippets so the examples never get stale.

The snippets are placed into a directory structure (based on a given template) that can be used to build and optionally execute the code.


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

There is a working example at [StratifyDocs](https://github.com/StratifyLabs/StratifyDocs) with the code at [StratifyDocsCode](https://github.com/StratifyLabs/StratifyDocsCode).
