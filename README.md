# {Ltd.}
A suite of C++ build tool and library.

{Ltd.}'s purpose is to provide environment to help writing safe and performant
C++ code. It does so by providing:
- a build tools that reduces the complexity of various C/C++ build tools and 
  toolchains 
- a library with a framework that encourages consistent and good practices in 
  writing safe and performant code.
  
## Building

```
> mkdir ltd_home
> cd ltd_home
> git clone https://github.com/benniadham/ltd.git
> git clone https://github.com/benniadham/ltd_sandbox.git
> mkdir caches
> cd caches
> mkdir ltd
> cd ltd
> cmake ../../ltd/ .
> cmake --build .
> ./ltd build ltd_sanbox
```

## Testing
The ltd framework collaborate features on CMake test and its own framework. 
It provides test_unit class that would let the user to create test unit and 
the test unit will communicate with CMake's test framework through stdout.

I.e. to use the test framework, one might use it as follow:

```C++
#include <ltd.h>

using namespace ltd;

auto main(int argc, char** argv) -> int 
{
    test_unit tu;

    tu.test([&tu](){
        tu.expect(true, "Expected true");
    });

    tu.test([&tu](){
        tu.expect(true, "Expected true");
    });

    tu.test([&tu](){
        tu.expect(true, "Expected true");
    });
    
    tu.run(argc, argv);
    
    return 0;
}
```

Each test correlate with 1 test in CMake's test. The number of the test case 
will be determined by calling the test binary. When the test binary called without
arguments, it will return the number of tests available. To run the test, specify
the test id in the cli argument.

```
>./mytest 1
```

Test id starts from 0. In this example, the program will run the second test case.

When test command runs, ltd will call ctest with -VV as parameter argument in 
the project cache path.
