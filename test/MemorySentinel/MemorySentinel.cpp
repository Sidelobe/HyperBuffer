
#include "MemorySentinel.hpp"

#include <future>
#include <string>


// Note: malloc overwrite only supported on GCC / Clang
#ifdef __APPLE__
    #include <dlfcn.h>
#endif

std::atomic<MemorySentinel::TransgressionBehaviour> MemorySentinel::m_transgressionBehaviour(TransgressionBehaviour::LOG);

MemorySentinel& MemorySentinel::getInstance()
{
    thread_local MemorySentinel instance;
    return instance;
}

// NOTE: make sure this function is allocation-free, or it could lead tonasty infinite loops!
static inline void handleTransgression(const char* optionalMsg = "")
{
    if (MemorySentinel::getInstance().isArmed() == false) {
        return;
    }
    
    MemorySentinel::getInstance().registerTransgression();
    
    switch (MemorySentinel::getTransgressionBehaviour())
    {
        case MemorySentinel::TransgressionBehaviour::THROW_EXCEPTION: {
            throw std::bad_alloc();
        }
        case MemorySentinel::TransgressionBehaviour::LOG: {
            printf("[MemorySentinel]: !!Transgression detected!! %s\n", optionalMsg);
            break;
        }
        case MemorySentinel::TransgressionBehaviour::SILENT: {
            break;
        }
    }
}

// MARK: - new
void* operator new(std::size_t sz) throw (std::bad_alloc)
{
    handleTransgression("allocation with new");
    return std::malloc(sz);
}

// MARK: - new[]
void* operator new[](std::size_t sz) throw (std::bad_alloc)
{
    handleTransgression("allocation with new[]");
    return std::malloc(sz);
}

// MARK: - new nothrow
void* operator new(std::size_t sz, std::nothrow_t const &) throw ()
{
    handleTransgression("allocation with new nothrow");
    return std::malloc(sz);
}

// MARK: - new[] nothrow
void* operator new[](std::size_t sz, std::nothrow_t const &) throw ()
{
    handleTransgression("allocation with new[] nothrow");
    return std::malloc(sz);
}

// MARK: - delete
void operator delete(void* ptr) throw()
{
    handleTransgression("deallocation with delete");
    std::free(ptr);
}

// MARK: - delete
void operator delete[](void* ptr) throw()
{
    handleTransgression("deallocation with delete[]");
    std::free(ptr);
}

#ifdef __APPLE__
// Note: malloc overwrite only supported on GCC / Clang
// SOURCE: https://stackoverflow.com/a/6083624
static void* (*realMalloc)(size_t) = nullptr;

static void initRealMalloc()
{
    realMalloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
    if (realMalloc == nullptr) {
        printf("Error in dlsym: malloc not found!\n");
    }
}

void* malloc(size_t sz)
{
    if (realMalloc == nullptr) {
        initRealMalloc();
    }

    handleTransgression("allocation with malloc");
    return realMalloc(sz);
}
#endif
