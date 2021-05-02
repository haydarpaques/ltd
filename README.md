# {Ltd.}
A suite of C++ library and build tools.

Sometimes, at least in my case, it can be quite difficult to build a team of C++ 
programmers with even levels of proficiencies. This will lead to inconsistent ways
of doing certain tasks in our codebase. 

I.e, in a lower spectrum, some programmers might use "\n", and other might use 
`std::endl`. In other spectrum some might always use `std::shared_ptr`, some might
avoid them altogether. Some might use exception for error handling, some might use
return values to signal errors. 

This library should address this issue by introducing some restrictions, hence the 
name, and framework in doing things to ensure that there will not be too many 
variations in techniques used to perform basic and routine tasks in our code base. 

On the flip side, {Ltd.} also provides some advance features such as asynch, which
is basically just a framework, a wrapper, around `std::thread`.

I am not saying that this is to improve C++. It is simply to make it easier for
a less experienced coder to join a C++ team.