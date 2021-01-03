/// \file main.cpp
/// \brief Defines the entry point of the unit test application.
///
/// \author Raffaele D. Facendola - June 2020.

#include <iostream>
#include <string>

#include "syntropy/language/foundation/foundation.h"

#include "syntropy/language/templates/type_traits.h"
#include "syntropy/language/templates/sequence.h"
#include "syntropy/language/templates/ratio.h"
#include "syntropy/language/templates/math.h"
#include "syntropy/language/templates/concepts.h"

#include "syntropy/language/preprocessor/macro.h"

#include "syntropy/language/support/initializer_list.h"
#include "syntropy/language/support/ignore.h"
#include "syntropy/language/support/swap.h"

#include "syntropy/core/support/scope_guard.h"
#include "syntropy/core/support/visitor.h"

#include "syntropy/core/concepts/ntuple.h"
#include "syntropy/core/concepts/range.h"

#include "syntropy/core/foundation/tuple.h"
#include "syntropy/core/foundation/reverse_range.h"
#include "syntropy/core/foundation/zip_range.h"
#include "syntropy/core/foundation/span.h"

#include "syntropy/memory/byte.h"
#include "syntropy/memory/address.h"
#include "syntropy/memory/size.h"
#include "syntropy/memory/alignment.h"
#include "syntropy/memory/byte_span.h"
#include "syntropy/memory/unique_ptr.h"

#include "syntropy/allocators/allocator.h"
#include "syntropy/allocators/system_allocator.h"

#include "syntropy/diagnostics/debugger.h"
#include "syntropy/diagnostics/assert.h"

#include "syntropy/core/support/event.h"

struct Foo
{
    int f = 1999;
};

int main(int argc, char **argv)
{
    std::cout << "Hello Syntropy!\n";

    {
        auto l = Syntropy::Listener{};
        auto e = Syntropy::Event<int>{};

        {
            auto m = Syntropy::Listener{};

            l += e.Subscribe([](int x)
                {
                    std::cout << "L event 0 " << x << "\n";
                });

            e.Notify(0);
            std::cout << "\n";

            l += e.Subscribe([](int x)
                {
                    std::cout << "L event 1 " << x << "\n";
                });

            e.Notify(1);
            std::cout << "\n";

            m += e.Subscribe([](int x)
                {
                    std::cout << "M event 1 " << x << "\n";
                });

            e.Notify(2);
            std::cout << "\n";
        }

        e.Notify(3);
        std::cout << "\n";
    }

    system("pause");

}


