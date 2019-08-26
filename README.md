# md2code

md2code is a C++ application that will pull C/C++ code snippets out of markdown. The purpose is to allow the compilation of code snippets so that they can be posted free of compilation errors.

The snippets are placed into a directory structure that can be used to build and optionally execute the code.

The snippet must be marked as follows

```
//md2code: <snippet|program>
```

For example:

```
//md2code: program

#include <stdio.h>

int main(int argc, char * argv[]){
   printf("Hello World\n");
}
```

A snippet example:

```
//md2code: snippet

#include <sapi/fs.hpp>

File f;
f.open(arg::SourceFile("/home/test.txt"));
f.close();
```
