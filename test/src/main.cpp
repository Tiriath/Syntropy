
#include <iostream>

#include "syntropy/core/range.h"
#include "syntropy/core/string.h"
#include "syntropy/core/string_stream.h"
#include "syntropy/core/label.h"
#include "syntropy/core/context.h"
#include "syntropy/core/smart_pointers.h"

#include "syntropy/platform/endianness.h"
#include "syntropy/platform/system.h"
#include "syntropy/platform/threading.h"
#include "syntropy/platform/intrinsics.h"

#include "syntropy/language/macro.h"
#include "syntropy/language/scope_guard.h"
#include "syntropy/language/utility.h"
#include "syntropy/language/tuple.h"

#include "syntropy/application/command_line.h"
#include "syntropy/application/command_line_argument.h"

#include "syntropy/containers/array.h"
#include "syntropy/containers/vector.h"
#include "syntropy/containers/map.h"
#include "syntropy/containers/set.h"

#include "syntropy/memory/bytes.h"
#include "syntropy/memory/alignment.h"
#include "syntropy/memory/memory_address.h"
#include "syntropy/memory/memory_range.h"
#include "syntropy/memory/virtual_memory.h"
#include "syntropy/memory/virtual_memory_range.h"
#include "syntropy/memory/memory_buffer.h"

#include "syntropy/allocators/memory_context.h"
#include "syntropy/allocators/memory_resource.h"

#include "syntropy/allocators/null_memory_resource.h"
#include "syntropy/allocators/system_memory_resource.h"
#include "syntropy/allocators/stack_memory_resource.h"
#include "syntropy/allocators/virtual_memory_resource.h"
#include "syntropy/allocators/linear_virtual_memory_resource.h"

#include "syntropy/allocators/fixed_memory_resource.h"
#include "syntropy/allocators/linear_memory_resource.h"
#include "syntropy/allocators/pool_memory_resource.h"

#include "syntropy/allocators/passthrough_memory_resource.h"
#include "syntropy/allocators/fallback_memory_resource.h"
#include "syntropy/allocators/counting_memory_resource.h"

#include "syntropy/allocators/polymorphic_allocator.h"
#include "syntropy/allocators/scope_allocator.h"

#include "syntropy/diagnostics/assert.h"
#include "syntropy/diagnostics/severity.h"
#include "syntropy/diagnostics/verbosity.h"
#include "syntropy/diagnostics/stack_trace.h"
#include "syntropy/diagnostics/debugger.h"
#include "syntropy/diagnostics/log_event.h"
#include "syntropy/diagnostics/log_channel.h"
#include "syntropy/diagnostics/log_manager.h"

#include "syntropy/math/constants.h"
#include "syntropy/math/hash.h"
#include "syntropy/math/metrics.h"

#include "syntropy/time/timer.h"
#include "syntropy/time/date.h"
#include "syntropy/time/time_of_day.h"

int main(int argc, char **argv)
{
    using namespace syntropy::Literals;

    auto t0 = std::tuple<int, float, char, int>{ 1, 2.5f, 'k', 90 };
    auto t1 = std::tuple<int, float, char>{ 2, 6.32f, 'o' };

    auto r = syntropy::LockstepRankV<decltype(t0)>;

    syntropy::LockstepApply([](auto& a, auto& b) { std::swap(a, b); }, t0, t1);

    //syntropy::LockstepApply([](auto& a) { ++a; }, t1);

    return 0;
}