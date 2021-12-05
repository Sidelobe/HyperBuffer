//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2021 Lorenz Bucher - all rights reserved

#pragma once

#include "HyperBufferStoragePolicies.hpp"

// Macros to restrict a function declaration to certain use cases, e.g. 1-dimensional, higher-dimensional, ...
#define FOR_N1 template<int M=N, std::enable_if_t<(M==1), int> = 0>
#define FOR_Nx template<int M=N, std::enable_if_t<(M>1), int> = 0>

// For N>1 and any number / exactly N-1 variable arguments
#define FOR_Nx_V template<int M=N, typename... I, std::enable_if_t<(M>1 && sizeof...(I)<M-1), int> = 0>
#define FOR_Nx_N template<int M=N, typename... I, std::enable_if_t<(M>1 && sizeof...(I)==M-1), int> = 0>

namespace slb
{

template<typename T, int N, class StoragePolicy>
class HyperBuffer
{
    
protected:
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;
    using subdim_pointer_type       = typename remove_pointers_from_type<pointer_type, 1>::type;
    using subdim_const_pointer_type = typename remove_pointers_from_type<const_pointer_type, 1>::type;
    
public:
    /** Generic Constructor that forwards all arguments to the storage policy constructor  */
    template<typename... I>
    explicit HyperBuffer(I... i) : m_storage(i...) {}
    
    /**
     * Copy constructor from an object with a different storage policy. Enabled only Policy differs from current one
     * (to avoid hijacking the 'normal' copy constructor).
     * @attention This is dangerous. It's meant to allow a 'view' to be constructed from an 'owning' buffer only and is
     * implemented using a special ctor in the corresponding StoragePolicy.
     */
    template<typename U, int M, class AnotherStoragePolicy, typename std::enable_if<!std::is_same<AnotherStoragePolicy, StoragePolicy>::value, int>::type = 0>
    explicit HyperBuffer(const HyperBuffer<U, M, AnotherStoragePolicy>& other) : m_storage(other.m_storage) {}

//    explicit HyperBuffer(const HyperBuffer<T, N, StoragePolicy>&& other) noexcept { this->m_storage = std::move(other.m_storage); }
    
    ~HyperBuffer() = default;
    
    // MARK: dimension extents
    int size(int i) const { return m_storage.size(i); }
    const std::array<int, N>& sizes() const noexcept { return m_storage.sizes(); }
    
    // MARK: data() -- raw pointer to beginning of underlying storage (pointers or data, depending on dimension)
    FOR_Nx const_pointer_type data() const noexcept { return m_storage.getDataPointer_Nx(); }
    FOR_Nx       pointer_type data()       noexcept { return m_storage.getDataPointer_Nx(); }
    FOR_N1           const T* data() const noexcept { return m_storage.getDataPointer_N1(); }
    FOR_N1                 T* data()       noexcept { return m_storage.getDataPointer_N1(); }
    
    // MARK: operator[] -- raw data/pointer access; returns pointer N>1, reference for N=1
    FOR_Nx subdim_const_pointer_type operator[] (size_type i) const { return m_storage.getDataPointer_Nx()[i]; }
    FOR_Nx       subdim_pointer_type operator[] (size_type i)       { return m_storage.getDataPointer_Nx()[i]; }
    FOR_N1                  const T& operator[] (size_type i) const { return m_storage.getDataPointer_N1()[i]; }
    FOR_N1                        T& operator[] (size_type i)       { return m_storage.getDataPointer_N1()[i]; }

    // MARK: at(...) -- Exists only for call with N parameters, returns data
    FOR_Nx_N decltype(auto) at(size_type dn, I... i) const { return createSubBuffer(dn).at(i...); }
    FOR_N1         const T& at(size_type i)          const { return m_storage.getDataPointer_N1()[i]; }
    FOR_N1               T& at(size_type i)                { return m_storage.getDataPointer_N1()[i]; }
    
    // MARK: subView(...) -- returns Derived<T,N-1> instance
    FOR_Nx_V decltype(auto) subView(size_type dn, I... i) const { return createSubBuffer(dn).subView(i...); }
    FOR_Nx   decltype(auto) subView(size_type dn)         const { return createSubBuffer(dn); }
    FOR_Nx   decltype(auto) subView(size_type dn)               { return std::as_const(*this).subView(dn); }
    
private:
    HyperBuffer<T, N-1, typename StoragePolicy::SubBufferPolicy> createSubBuffer(size_type index) const
    {
        auto subViewData = m_storage.getSubDimData(index);
        auto subViewDims = StdArrayOperations::shaveOffFirstElement(sizes());
        return HyperBuffer<T, N-1, typename StoragePolicy::SubBufferPolicy>(subViewData, subViewDims);
    }
    
private:
    // Allow instances with other template arguments to access private members
    template<typename U, int M, class AnotherStoragePolicy> friend class HyperBuffer;
    
    StoragePolicy m_storage;
    
};


} // namespace slb
