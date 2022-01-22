#include <iostream>

#include <string.h>

#include <ltd.h>

using namespace ltd;

int counter = 0;
class test_class
{
public:
    test_class() {
        std::cout << "test_class::ctor\n";
        counter++;
    }

    ~test_class() {
        std::cout << "test_class::dtor\n";
        counter--;
    }
};

namespace ltd
{
    namespace memory
    {
        class global_allocator
        {
            heap_allocator allocator;
        public:
            ret<block,error> allocate(size_t allocation_size)
            {
                std::cout << "Allocate " << allocation_size << std::endl;
                return allocator.allocate(allocation_size);
            }

            ret<block,error> allocate_all()
            {
                return allocator.allocate_all();
            }

            error deallocate(block allocated_block)
            {
                std::cout << "Deallocate " << allocated_block.size << std::endl;
                return allocator.deallocate_all();
            }

            error deallocate_all()
            {
                return allocator.deallocate_all();
            }

            error expand(block& allocated_block, size_t delta)
            {
                return allocator.expand(allocated_block, delta);
            }

            ret<bool,error> owns(block mem_block)
            {
                return allocator.owns(mem_block);
            }
        };
    }
}

auto main(int argc, char** argv) -> int
{       
    test_unit tu;

    tu.test([&tu] () -> void {
        {            
            auto obj1 = make_object<test_class>();
            tu.expect(counter == 1, "Step 1 counter = 1");   
            {
                auto obj2 = make_object<test_class>();
                tu.expect(counter == 2, "Step 2 counter = 2");   
            }
            tu.expect(counter == 1, "Step 3 counter = 1");         
        }
        tu.expect(counter == 0, "Step 4 counter = 0");         
    });

    tu.test([&tu] () -> void {
        {
            test_class *tc = new test_class();
            object<test_class> o(tc);
            tu.expect(counter == 1, "Step 1 counter = 1");        
        }
        tu.expect(counter == 0, "Step 2 counter = 0");        
    });

    tu.test([&tu] () -> void {
                
        {
            auto obj1 = make_object<test_class>();
            tu.expect(counter == 1, "Step 1 counter = 1");    
        }
        tu.expect(counter == 0, "Step 2 counter = 0");        
    });
    

    tu.run(argc, argv);

    return 0;
}