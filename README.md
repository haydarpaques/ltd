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
