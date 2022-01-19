#include <string.h>

#include <ltd.h>

using namespace ltd;

int counter = 0;
class test_class
{
public:
    test_class() {
        std::cout << "ctor\n";
        counter++;
    }

    ~test_class() {
        std::cout << "dtor\n";
        counter--;
    }
};

namespace ltd
{
    namespace memory
    {
        class global_allocatori
        {
        public:
            global_allocatori() {};
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
    

    tu.run(argc, argv);

    return 0;
}