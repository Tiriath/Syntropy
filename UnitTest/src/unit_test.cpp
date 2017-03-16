// ProtoDW.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <fstream>

#include "vm/virtual_machine.h"
#include "vm/intrinsics.h"

#include "memory/std_allocator.h"
#include "memory/segregated_allocator.h"
#include "memory/stack_allocator.h"

#include "diagnostics/log.h"

syntropy::diagnostics::Context Root;

struct FooSmall
{
    char buffer[3214];
};

struct FooMedium
{
    char buffer[4096];
};

struct FooLarge
{
    char buffer[6845];
};

int main()
{

    // Initialize log

    std::ofstream log_file;

    log_file.open("log.txt");

    auto& log_manager = syntropy::diagnostics::LogManager::GetInstance();

    auto format = "[{date} {time}] [{severity}] [thread: {thread}] [{context}]: {message}";
    auto stream = log_manager.CreateStream<syntropy::diagnostics::StreamLog>(log_file, format);
    
    stream->BindContext({ Root });
    stream->SetVerbosity(syntropy::diagnostics::Severity::kInformative);

    //
    syntropy::MemoryPool pool(0x4000000, 1);

    {

        //syntropy::LinearSegregatedFitAllocator tlsfa("tiny", pool, 8, 64, 16384);
        //syntropy::TwoLevelSegregatedFitAllocator tlsfa("linear", pool, 2);
        syntropy::ExponentialSegregatedFitAllocator tlsfa("exponential", pool, 4096, 5);
            
        auto p = SYNTROPY_NEW(tlsfa) FooSmall();
        auto q = SYNTROPY_NEW(tlsfa) FooMedium();
        auto r = SYNTROPY_NEW(tlsfa) FooLarge();

        SYNTROPY_DELETE(tlsfa, p);

        p = SYNTROPY_NEW(tlsfa) FooSmall();

        SYNTROPY_DELETE(tlsfa, p);
        SYNTROPY_DELETE(tlsfa, q);

        q = SYNTROPY_NEW(tlsfa) FooMedium();

        SYNTROPY_DELETE(tlsfa, q);
        SYNTROPY_DELETE(tlsfa, r);

        r = SYNTROPY_NEW(tlsfa) FooLarge();

        SYNTROPY_DELETE(tlsfa, r);

    }

    //

    log_file.close();

    system("pause");

}