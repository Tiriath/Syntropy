/// \file os.h
/// \brief This header is part of the syntropy HAL (hardware abstraction layer) system. This file is meant to be used as a meta-header to include os-specific headers.
///
/// IMPORTANT: Include this header as a part of a .cpp file. Do not include this header in any header (other than private ones).
///
/// \author Raffaele D. Facendola - May 2017

#pragma once

#define SYNTROPY_OS_INCLUDE_GUARD

#ifdef _WIN64

#include "windows_os.h"

#endif

#undef SYNTROPY_OS_INCLUDE_GUARD