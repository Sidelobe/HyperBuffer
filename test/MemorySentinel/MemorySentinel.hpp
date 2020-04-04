
#pragma once

#include <atomic>
#include <cassert>

/**
 * Singleton that hijacks all calls on new, new[], delete and delete[] as well as malloc/free.
 * This is useful to detect whether memory has been allocated in unit tests.
 */
class MemorySentinel {
public:
    enum class TransgressionBehaviour
    {
        LOG,
        THROW_EXCEPTION,
        SILENT,
    };

    /** Returns a MemorySentinel for the current thread. */
    static MemorySentinel& getInstance();
    
    void setArmed(bool value) { m_allocationForbidden.store(value); }
    bool isArmed() const { return m_allocationForbidden.load(); }

    static void setTransgressionBehaviour(TransgressionBehaviour b) noexcept { m_transgressionBehaviour.store(b); }
    static TransgressionBehaviour getTransgressionBehaviour() noexcept { return m_transgressionBehaviour.load(); }

    void registerTransgression() { m_transgressionOccured.store(true); }
    void clearTransgressions() { m_transgressionOccured.exchange(false); }
    
    /** NOTE: this clear the transgression upon call */
    bool getAndClearTransgressionsOccured() noexcept
    {
        bool result = m_transgressionOccured.load();
        clearTransgressions();
        return result;
    }

private:
    MemorySentinel() = default; // Singleton = private ctor
    
    static std::atomic<TransgressionBehaviour> m_transgressionBehaviour;
    std::atomic<bool> m_allocationForbidden { false };
    std::atomic<bool> m_transgressionOccured { false };
};


class ScopedMemorySentinel
{
public:
    ScopedMemorySentinel()
    {
        auto& sentinel = MemorySentinel::getInstance();
        sentinel.setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::LOG);
        sentinel.clearTransgressions();
        sentinel.setArmed(true);
    }

    ~ScopedMemorySentinel()
    {
        auto& sentinel = MemorySentinel::getInstance();
        sentinel.setArmed(false);
        if (sentinel.getAndClearTransgressionsOccured()) {
            assert(false && "MemorySentinel was triggered!");
        }
    }
};
