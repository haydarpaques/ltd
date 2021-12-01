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
> ./ltd build ltd_sandbox
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

## Features

... or a lack thereof.

- **Error Handling**
  
  Errors are handled by returning status in every functions in the code base. There 
  should not be try-and-catch error handling used in the library.
  
  There is a class under the ltd namespace called `error`. You can use the provided
  error in the class' static constant member or use `error make_error(const char *message)`
  to create custom error.
  
- **Multiple Return Values**

  To support error handling by returning error status, the library supports multiple 
  return values. I.e. when the function needs to return value(s) along with the error 
  status.
  
  ```c++
  ret<int,error> do_something() 
  {
    return {0, error::no_error};
  }
  ```

- **Application Framework**

  Application framework encapsulate functionalities in creating application using C++.
  It also gives an underlying functionalities in creating test unit framework.
  
  ```C++
  class my_app : public application
  {
  public:
    int main()
    {
      // Do something...
      return 0;
    }
  };
  
  my_app the_app;
  
  ```

- **Pointers**

  `std`'s smart pointers are powerful. But it can lead into several performance and
  stability problems if the programmer does not have enough experience with them. 
  Therefore `ltd` only provide one model of smart pointer. It only provides functionalities 
  similar with `std::unique_ptr`. The use of shared pointers and weak pointers are 
  not advised with `ltd`.

  Ownership of a pointer is underlined very strongly in `ltd`. Pointers can be owned 
  by a class, a cointainer or local variable. When passing a raw pointer to a function, 
  the function should not claim ownership to the pointer. When a function expecting 
  to own a pointer, it should specify r-value to allow the use of `std::move()`.

- **Removal of [] operator**

  To prevent array out of bound error and to help with map get operation, the [] operator
  is removed from `ltd`'s containers. It will use `ret<T,error> at(K)` instead. Where T
  is the type of the value and K is the type of the index or key. Expect `error::not_found` 
  for key-value containers when the element requested does not exist or `error::index_out_of_bound`
  for index based containers.

- **Containers: array, slice and map**

  `ltd` supports basic containers with its own flavors. 2 major difference with `std`'s 
  containers are memory layout and API. They supports `std::iterator`s, but they do 
  not support [] operator.

  `ltd::array` provides random read and write access to a typed buffer with static size.
  It will not call default constructor of its value type while being created. Hence, 
  the elements of array are not initialized. It allows the insertion of an object anywhere
  between 0 to N-1 from the get go. When insertin an object, it check whether the index
  given contains an object. If it's empty, it will place the object in the given index. 
  When it is occupied by an object, the existing object will be destroyed and then the 
  new empty space will be filled by the new object.

  `ltd::slice` provides dynamic array functionalities similar with `std::vector`. It 
  supports append and pop back operations. Initially, it's size is 0. The size increases 
  when append operation performed. If the size is > 0 then it allows read and write 
  access, similar with `ltd::array`, from 0 to size - 1. `ltd::slize` has the ability
  to do storage sharing among its instances and copy-on-write.

- **Asynch and threading framework**

  TBD.
  
## Directory Structure

In this example 'myproject1' has multiple applications and multiple library. 'myproject2' only
has 1 application and 1 library. The names of the binaries are 'myproject2' and 'myproject2.a'
consecutively.

```
$LTD_HOME
  +- pkgs
  |    +- myproject1
  |         +- inc
  |         +- bin
  |
  +- caches
  |
  +- myproject1
  |    +- apps
  |    |  +- myapp1
  |    |  +- myapp2
  |    +- doc
  |    +- inc
  |    +- libs
  |    |  +- mylib1
  |    |  +- mylib2
  |    +- tests
  |
  +- myproject2
       +- app
       +- doc
       +- lib
       +- tests
```
