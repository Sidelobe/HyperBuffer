
#include <memory>
#include <vector>

#include "HyperBufferData.hpp"
#include "TemplateUtils.hpp"

// MARK: - HyperBuffer declaration
template<typename T, int N>
class HyperBuffer
{
    using size_type = int;
    using data_type = typename add_pointers_to_type<T, N>::type;

public:
    /** Construct from dimesions sizes, allocate data */
    template<typename... I>
    explicit HyperBuffer(I... i)
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
        m_dataObject = std::make_unique<HyperBufferDataOwning<T, N>>(i...);
        m_dataPointers = m_dataObject->getDataPointers();
    }

    /** Construct from pre-allocated, multi-dimensional data */
    template<typename... I>
    explicit HyperBuffer(data_type preAllocatedData, I... i)
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
        m_dataObject = std::make_unique<HyperBufferDataPreAlloc<T, N>>(preAllocatedData, i...);
        m_dataPointers = m_dataObject->getDataPointers();
    }
    
    /** Construct from pre-allocated, flat (1D) data */
    template<typename... I, int V=N, typename std::enable_if<V!=1>::type>
    explicit HyperBuffer(T* preAllocatedDataFlat, I... i)
    {
        // TODO:
    }

    decltype(auto) operator[] (size_type i) { return m_dataPointers[i]; }

    template<int... I>
    decltype(auto) operator() (size_type i...)
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
        return m_dataPointers[i];
    }

    data_type data() const { return m_dataPointers; }
    decltype(auto) at(size_type offset) const;
    
    int* dims() const { return m_dataObject->m_dimensions.data(); }
    int dim(int i) const { return m_dataObject->m_dimensions[i]; }

private:
    std::unique_ptr<HyperBufferData<T, N>> m_dataObject;
    
    data_type m_dataPointers;
};
