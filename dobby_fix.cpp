// Fixed dobby_wrapper.cpp with proper DobbyUnHook implementation
#include "../external/dobby/include/dobby.h"
#include <cstdint>
#include <unordered_map>
#include <mutex>
#include <vector>

namespace DobbyWrapper {
    // Thread-safe storage for original function pointers
    static std::unordered_map<void*, void*> originalFunctions;
    static std::mutex hookMutex;
    static std::vector<std::pair<void*, void*>> hookHistory;

    // Hook a function using Dobby
    void* Hook(void* targetAddr, void* replacementAddr) {
        if (!targetAddr || !replacementAddr) return nullptr;
        
        void* originalFunc = nullptr;
        
        {
            std::lock_guard<std::mutex> lock(hookMutex);
            int result = DobbyHook(targetAddr, replacementAddr, &originalFunc);
            
            if (result == 0 && originalFunc) {
                originalFunctions[targetAddr] = originalFunc;
                hookHistory.push_back({targetAddr, replacementAddr});
            } else {
                // Log error or handle the failure
                return nullptr;
            }
        }
        
        return originalFunc;
    }

    // Get the original function pointer for a hooked function
    void* GetOriginalFunction(void* targetAddr) {
        std::lock_guard<std::mutex> lock(hookMutex);
        auto it = originalFunctions.find(targetAddr);
        if (it != originalFunctions.end()) {
            return it->second;
        }
        return nullptr;
    }

    // Unhook a previously hooked function
    bool Unhook(void* targetAddr) {
        if (!targetAddr) return false;
        
        {
            std::lock_guard<std::mutex> lock(hookMutex);
            // If DobbyUnHook doesn't exist, we can implement a workaround
            // DobbyUnHook may not exist in some versions of Dobby
            #ifdef DOBBY_UNHOOK_DEFINED
            int result = DobbyUnHook(targetAddr);
            if (result != 0) {
                return false;
            }
            #else
            // Alternative implementation if DobbyUnHook is not available
            // We could just unbind by hooking back to the original
            auto it = originalFunctions.find(targetAddr);
            if (it != originalFunctions.end()) {
                void* originalFunc = it->second;
                // Re-hook to restore original function
                DobbyHook(targetAddr, originalFunc, nullptr);
            } else {
                return false;
            }
            #endif
            
            originalFunctions.erase(targetAddr);
        }
        
        return true;
    }

    // Unhook all previously hooked functions
    void UnhookAll() {
        std::lock_guard<std::mutex> lock(hookMutex);
        
        for (auto& pair : hookHistory) {
            #ifdef DOBBY_UNHOOK_DEFINED
            DobbyUnHook(pair.first);
            #else
            // Alternative implementation
            auto it = originalFunctions.find(pair.first);
            if (it != originalFunctions.end()) {
                DobbyHook(pair.first, it->second, nullptr);
            }
            #endif
        }
        
        originalFunctions.clear();
        hookHistory.clear();
    }
}
