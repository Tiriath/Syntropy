
// ProtoDW.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <random>

#include "unit1.h"

#include "vm/virtual_machine.h"
#include "vm/intrinsics.h"

#include "memory/memory.h"
#include "memory/memory_units.h"
#include "memory/memory_manager.h"

#include "memory/allocators/std_allocator.h"
#include "memory/allocators/segregated_allocator.h"
#include "memory/allocators/stack_allocator.h"
#include "memory/allocators/master_allocator.h"

#include "memory/memory_meta.h"

#include "diagnostics/log.h"
#include "diagnostics/log_channels.h"
#include "diagnostics/diagnostics_meta.h"

#include "serialization/json/json.h"

#include "platform/system.h"

syntropy::diagnostics::Context Root;

struct FooSmall
{
    char buffer[256];
};

struct FooMedium
{
    char buffer[257];
};

struct FooLarge
{
    char buffer[258];
};

int main()
{

    using syntropy::diagnostics::LogManager;
    using syntropy::diagnostics::Severity;
    using syntropy::diagnostics::FileLogChannel;

    // Initialize log

    SYNTROPY_UNUSED(syntropy::reflection::ClassOf<FileLogChannel>());

    syntropy::diagnostics::ImportLogConfigurationFromJSON("log.cfg");

    auto& mm = syntropy::MemoryManager::GetInstance();

    Tester t;
    t.Do();

    // Initialization of the memory manager

    SYNTROPY_UNUSED(syntropy::reflection::ClassOf<syntropy::LinearSegregatedFitAllocator>());
    SYNTROPY_UNUSED(syntropy::reflection::ClassOf<syntropy::ExponentialSegregatedFitAllocator>());
    SYNTROPY_UNUSED(syntropy::reflection::ClassOf<syntropy::TwoLevelSegregatedFitAllocator>());

    std::ifstream file("memory.cfg");

    nlohmann::json json;

    file >> json;

    auto allocators = syntropy::serialization::DeserializeObjectFromJSON<std::vector<std::unique_ptr<syntropy::Allocator>>>(json);

    SYNTROPY_UNUSED(allocators);

    auto& small_allocator = mm.AddAllocator(std::make_unique<syntropy::LinearSegregatedFitAllocator>("small", 512_MiBytes, 8_Bytes, 32, 16_KiBytes));
    auto& large_allocator = mm.AddAllocator(std::make_unique<syntropy::ExponentialSegregatedFitAllocator>("large", 160_GiBytes, 64_KiBytes, 10));
    mm.AddAllocator(std::make_unique<syntropy::MasterAllocator>("master1", 8_GiBytes, small_allocator, large_allocator));
    mm.AddAllocator(std::make_unique<syntropy::MasterAllocator>("master2", 8_GiBytes, small_allocator, large_allocator));

    mm.SetDefaultAllocator("master1");

    //

    void* p;
    void* q;
    void* r;

    {
        syntropy::MemoryContext ctx1("master2");

        p = SYNTROPY_MM_ALLOC(23_Bytes);

        {
            syntropy::MemoryContext ctx2("master1");

            q = SYNTROPY_MM_ALLOC(24_KiBytes);
            r = SYNTROPY_MM_ALLOC(2_MiBytes);
        }

        SYNTROPY_MM_FREE(p);        // TODO: quirk! p was allocated by "small", the actual allocator who handled the call was "master2". Incidentally also "master1" references "small", so it will take care of its deallocation. (that's correct but a little bit obscure)
        SYNTROPY_MM_FREE(q);
        SYNTROPY_MM_FREE(r);
    }

    //

    system("pause");

}
