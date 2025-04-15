// Memory access utilities for iOS
#pragma once

#include "../objc_isolation.h"
#include <cstdint>
#include <vector>

// Include platform-specific headers
#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_init.h>
#include <mach/mach_interface.h>
#include <mach/task.h>
// Instead of including mach_vm.h which is unsupported, we define what we need
#ifndef MACH_VM_INCLUDED
#define MACH_VM_INCLUDED
typedef vm_address_t mach_vm_address_t;
typedef vm_size_t mach_vm_size_t;
#endif // MACH_VM_INCLUDED
#else
// Include mach_compat.h for non-Apple platforms
#include "mach_compat.h"
#endif // __APPLE__

namespace iOS {
    class MemoryAccess {
    public:
        // Read memory from a process
        static bool ReadMemory(void* address, void* buffer, size_t size);
        
        // Write memory to a process
        static bool WriteMemory(void* address, const void* buffer, size_t size);
        
        // Change memory protection
        static bool SetMemoryProtection(void* address, size_t size, int protection);
        
        // Allocate memory
        static void* AllocateMemory(size_t size);
        
        // Free memory
        static bool FreeMemory(void* address, size_t size);
        
        // Find memory region
        static void* FindMemoryRegion(const char* pattern, size_t size, void* startAddress = nullptr, void* endAddress = nullptr);
    };
}
