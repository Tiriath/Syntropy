
/// \file system.h
/// \brief This header is part of the Syntropy platform module. It contains classes used to query system's capabilities.
///
/// \author Raffaele D. Facendola - 2016

#pragma once

#include "syntropy/core/string.h"
#include "syntropy/containers/vector.h"
#include "syntropy/memory/bytes.h"

namespace syntropy
{
    /************************************************************************/
    /* CPU ARCHITECTURE                                                     */
    /************************************************************************/

    /// \brief CPU architectures.
    /// \author Raffaele D. Facendola - December 2016.
    enum class CPUArchitecture
    {
        /// \brief x86 architecture.
        kx86,

        /// \brief x64 architecture.
        kx64,

        /// \brief ARM architecture.
        kARM,

        /// \brief Unknown architecture.
        kUnknown,
    };

    /************************************************************************/
    /* CPU INFO                                                             */
    /************************************************************************/

    /// \brief Describes a CPU.
    /// \author Raffaele D. Facendola - December 2016.
    struct CPUInfo
    {
        /// \brief Core frequency, in Hz.
        std::int64_t frequency_;

        /// \brief Number of logical cores.
        std::int64_t cores_;

        /// \brief Underlying architecture.
        CPUArchitecture architecture_;
    };

    /************************************************************************/
    /* DRIVE INFO                                                           */
    /************************************************************************/

    /// \brief Describes a drive.
    /// \author Raffaele D. Facendola - December 2016.
    struct DriveInfo 
    {
        /// \brief Unit letter or label.
        String label_;

        /// \brief Total drive space.
        Bytes total_space_;

        /// \brief Available drive space.
        Bytes available_space_;
    };

    /************************************************************************/
    /* STORAGE INFO                                                         */
    /************************************************************************/

    /// \brief Describes the storage capabilities of the system.
    /// \author Raffaele D. Facendola - December 2016.
    struct StorageInfo
    {
        /// \brief List of available physical drives.
        Vector<DriveInfo> drives_;
    };

    /************************************************************************/
    /* MEMORY INFO                                                          */
    /************************************************************************/

    /// \brief Describes the system memory.
    /// \author Raffaele D. Facendola - December 2016.
    struct MemoryInfo
    {
        /// \brief Total physical memory.
        Bytes total_physical_memory_;

        /// \brief Total virtual address space for this process.
        Bytes total_virtual_memory_;

        /// \brief Total page memory.
        Bytes total_page_memory_;

        /// \brief Available physical memory.
        Bytes available_physical_memory_;

        /// \brief Available virtual address space for the process.
        Bytes available_virtual_memory_;

        /// \brief Available page memory.
        Bytes available_page_memory_;
    };

    /************************************************************************/
    /* MONITOR INFO                                                         */
    /************************************************************************/

    /// \brief Describes a monitor.
    /// \author Raffaele D. Facendola - December 2016
    struct MonitorInfo
    {
        /// \brief Monitor name.
        String device_name_;

        /// \brief Name of the adapter (GPU) the monitor is attached to.
        String adapter_name_;

        /// \brief Horizontal resolution.
        std::int64_t width_;

        /// \brief Vertical resolution.
        std::int64_t height_;

        /// \brief Refresh rate, in Hz.
        float refresh_rate_;

        /// \brief Whether the monitor is the primary one.
        bool is_primary_;
    };

    /************************************************************************/
    /* DISPLAY INFO                                                         */
    /************************************************************************/

    /// \brief Describes the system display capabilities.
    /// \author Raffaele D. Facendola - December 2016
    struct DisplayInfo
    {
        /// \brief Available monitors.
        Vector<MonitorInfo> monitors_;
    };

    /************************************************************************/
    /* OPERATING SYSTEM                                                     */
    /************************************************************************/

    /// \brief Operating system.
    /// \author Raffaele D. Facendola - December 2016
    enum class OperatingSystem 
    {
        /// \brief Windows OS.
        kWindows,
    };

    /************************************************************************/
    /* PLATFORM INFO                                                        */
    /************************************************************************/

    /// \brief Describes the platform capabilities.
    /// \author Raffaele D. Facendola - December 2016
    struct PlatformInfo
    {
        /// \brief Operating system.
        OperatingSystem operating_system_;
    };

    /************************************************************************/
    /* SYSTEM                                                               */
    /************************************************************************/

    /// \brief Exposes methods to query system's capabilities.
    /// \author Raffaele D. Facendola
    namespace System
    {
        /// \brief Get the current CPU infos.
        CPUInfo GetCPUInfo();

        /// \brief Get the current storage infos.
        StorageInfo GetStorageInfo();

        /// \brief Get the current memory infos.
        MemoryInfo GetMemoryInfo();

        /// \brief Get the current desktop infos.
        DisplayInfo GetDisplayInfo();

        /// \brief Get the current platform infos.
        PlatformInfo GetPlatformInfo();
    };

}