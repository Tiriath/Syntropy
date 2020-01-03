
/// \file stream_vector.h
/// \brief This header is part of the syntropy containers. It contains classes used to handle structure-of-arrays.
///
/// \author Raffaele D. Facendola - 2016

#pragma once

#include <cstdint>
#include <tuple>
#include <vector>
#include <type_traits>

#include "syntropy/containers/vector_view.h"

#include "syntropy/type_traits.h"

namespace syntropy
{
    /************************************************************************/
    /* STREAM VECTOR                                                        */
    /************************************************************************/

    /// \brief Packs together multiple parallel vectors, each referring to a single attribute of an element.
    /// This vector is used to quickly iterate through an element attribute, allowing the data to be organized according to the structure-of-arrays paradigm.
    /// \author Raffaele D. Facendola - December 2018
    template <typename... TStreams>
    class StreamVector
    {
        static_assert(sizeof...(TStreams) > 0);

    public:

        /// \brief Default constructor.
        StreamVector() = default;

        /// \brief Default copy constructor.
        StreamVector(const StreamVector& rhs) = default;

        /// \brief Default move constructor.
        StreamVector(StreamVector&& rhs) = default;

        /// \brief Default assignment operator.
        StreamVector& operator=(StreamVector& rhs) = default;

        /// \brief Default destructor.
        ~StreamVector() = default;

        /// \brief Returns a reference to the element at specified index in a given stream.
        /// \return Returns the reference to the requested element in the specified stream.
        template <std::size_t kStream>
        auto& GetElementAt(std::size_t index);

        /// \brief Returns a reference to the element at specified index in a given stream.
        /// \return Returns the reference to the requested element in the specified stream.
        template <std::size_t kStream>
        const auto& GetElementAt(std::size_t index) const;

        /// \brief Returns a reference to the element at specified index in a given stream type.
        /// \return Returns the reference to the requested element in the specified stream.
        template <typename TElement>
        TElement& GetElementAt(std::size_t index);

        /// \brief Returns a reference to the element at specified index in a given stream type.
        /// \return Returns the reference to the requested element in the specified stream.
        template <typename TElement>
        const TElement& GetElementAt(std::size_t index) const;

        /// \brief Returns a reference to the first element of a given stream.
        /// \return Returns the reference to the first element in the specified stream.
        template <std::size_t kStream>
        auto& GetFront();

        /// \brief Returns a reference to the first element of a given stream.
        /// \return Returns the reference to the first element in the specified stream.
        template <std::size_t kStream>
        const auto& GetFront() const;

        /// \brief Returns a reference to the first element of a given stream, by stream type.
        /// \return Returns the reference to the first element in the specified stream.
        template <typename TElement>
        TElement& GetFront();

        /// \brief Returns a reference to the first element of a given stream, by stream type.
        /// \return Returns the reference to the first element in the specified stream.
        template <typename TElement>
        const TElement& GetFront() const;

        /// \brief Returns a reference to the last element of a given stream.
        /// \return Returns the reference to the last element in the specified stream.
        template <std::size_t kStream>
        auto& GetBack();

        /// \brief Returns a reference to the last element of a given stream.
        /// \return Returns the reference to the last element in the specified stream.
        template <std::size_t kStream>
        const auto& GetBack() const;

        /// \brief Returns a reference to the last element of a given stream, by stream type.
        /// \return Returns the reference to the last element in the specified stream.
        template <typename TElement>
        TElement& GetBack();

        /// \brief Returns a reference to the last element of a given stream, by stream type.
        /// \return Returns the reference to the last element in the specified stream.
        template <typename TElement>
        const TElement& GetBack() const;

        /// \brief Check whether the container is empty.
        /// \return Returns true if the container is empty, returns false otherwise.
        bool IsEmpty() const noexcept;

        /// \brief Get the number of elements in the container.
        /// \return Returns the number of elements in the container.
        std::size_t GetSize() const noexcept;

        /// \brief Increase the capacity of the vector to a value that's greater or equal to capacity. If capacity is greater than the current GetCapacity(), new storage is allocated, otherwise the method does nothing.
        void Reserve(std::size_t capacity);

        /// \brief Get the number of elements that can be contained in the current allocation storage.
        std::size_t GetCapacity() const noexcept;

        /// \brief Reduce memory usage by reducing unused memory.
        void ShrinkToFit();

        /// \brief Erase all elements from the container.
        void Clear() noexcept;

        /// \brief Emplace an element at the end of the container.
        template <typename... TValues>
        void PushBack(TValues&&... values);

        /// \brief Remove the last element of the container.
        void PopBack();

        /// \brief Remove the index-th element of the container and replaces it with the last element in the container.
        /// The order of the container is not preserved as a result of this call.
        /// /// \param index Index of the element to remove.
        void EraseSwap(std::size_t index);

        /// \brief Execute an operation on each element of the given streams.
        template <std::size_t... kStreams, typename TOperation>
        void ForEach(TOperation&& operation);

        /// \brief Execute an operation on each element of the given streams.
        template <std::size_t... kStreams, typename TOperation>
        void ForEach(TOperation&& operation) const;

        /// \brief Execute an operation on each element of the given streams, by stream type.
        template <typename... TStreamTypes, typename TOperation>
        void ForEach(TOperation&& operation);

        /// \brief Execute an operation on each element of the given streams, by stream type.
        template <typename... TStreamTypes, typename TOperation>
        void ForEach(TOperation&& operation) const;

        /// \brief Get a view into a stream by index.
        template <std::size_t kStream>
        auto GetStream();

        /// \brief Get a view into a constant stream by index.
        template <std::size_t kStream>
        auto GetStream() const;

        /// \brief Get a view into a constant stream by index.
        template <std::size_t kStream>
        auto GetConstStream() const;

        /// \brief Get a view into a stream by type.
        template <typename TStream>
        VectorView<TStream> GetStream();

        /// \brief Get a view into a constant stream by type.
        template <typename TStream>
        VectorView<const TStream> GetStream() const;

        /// \brief Get a view into a constant stream by type.
        template <typename TStream>
        VectorView<const TStream> GetConstStream() const;

    private:

        /// \brief Appends the given element values to the end of a given stream.
        template <std::size_t kStream, typename TValue, typename... TValues>
        void PushBack(TValue&& value, TValues&&... values);

        /// \brief Appends the given element value to the end of a given stream.
        template <std::size_t kStream>
        void PushBack();

        /// \brief Execute an operation on each stream.
        template <typename TOperation>
        void ForEachStream(TOperation&& operation);

        /// \brief Execute an operation on each indexed stream.
        template <typename TOperation, std::size_t kStreamIndex, std::size_t... kStreamIndexes>
        void ForEachStream(TOperation&& operation, std::index_sequence<kStreamIndex, kStreamIndexes...>);

        /// \brief Execute an operation on each indexed stream.
        template <typename TOperation>
        void ForEachStream(TOperation&& operation, std::index_sequence<>);

        /// \brief Vector streams.
        /// \remarks Constness is removed to cope with standard requirements. The stream is immutable from outside perspective, though.
        std::tuple<std::vector<std::remove_const_t<TStreams>>...> streams_;
    };

    /************************************************************************/
    /* IMPLEMENTATION                                                       */
    /************************************************************************/

    // StreamVector<T...>.

    template <typename... TStreams>
    template <std::size_t kStream>
    inline auto& StreamVector<TStreams...>::GetElementAt(std::size_t index)
    {
        using TElement = std::tuple_element_t<kStream, std::tuple<TStreams...>>;

        return static_cast<TElement&>(std::get<kStream>(streams_)[index]);
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline const auto& StreamVector<TStreams...>::GetElementAt(std::size_t index) const
    {
        return std::get<kStream>(streams_)[index];
    }

    template <typename... TStreams>
    template <typename TElement>
    inline TElement& StreamVector<TStreams...>::GetElementAt(std::size_t index)
    {
        return GetElementAt<tuple_element_index_v<TElement, std::tuple<TStreams...>>>(index);
    }

    template <typename... TStreams>
    template <typename TElement>
    inline const TElement& StreamVector<TStreams...>::GetElementAt(std::size_t index) const
    {
        return GetElementAt<tuple_element_index_v<TElement, std::tuple<TStreams...>>>(index);
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline auto& StreamVector<TStreams...>::GetFront()
    {
        using TElement = std::tuple_element_t<kStream, std::tuple<TStreams...>>;

        return static_cast<TElement&>(std::get<kStream>(streams_).front());
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline const auto& StreamVector<TStreams...>::GetFront() const
    {
        return std::get<kStream>(streams_).front();
    }

    template <typename... TStreams>
    template <typename TElement>
    inline TElement& StreamVector<TStreams...>::GetFront()
    {
        return GetFront<tuple_element_index_v<TElement, std::tuple<TStreams...>>>();
    }

    template <typename... TStreams>
    template <typename TElement>
    inline const TElement& StreamVector<TStreams...>::GetFront() const
    {
        return GetFront<tuple_element_index_v<TElement, std::tuple<TStreams...>>>();
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline auto& StreamVector<TStreams...>::GetBack()
    {
        using TElement = std::tuple_element_t<kStream, std::tuple<TStreams...>>;

        return static_cast<TElement&>(std::get<kStream>(streams_).back());
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline const auto& StreamVector<TStreams...>::GetBack() const
    {
        return std::get<kStream>(streams_).back();
    }

    template <typename... TStreams>
    template <typename TElement>
    inline TElement& StreamVector<TStreams...>::GetBack()
    {
        return GetBack<tuple_element_index_v<TElement, std::tuple<TStreams...>>>();
    }

    template <typename... TStreams>
    template <typename TElement>
    inline const TElement& StreamVector<TStreams...>::GetBack() const
    {
        return GetBack<tuple_element_index_v<TElement, std::tuple<TStreams...>>>();
    }

    template <typename... TStreams>
    inline bool StreamVector<TStreams...>::IsEmpty() const noexcept
    {
        return std::get<0>(streams_).empty();
    }

    template <typename... TStreams>
    inline std::size_t StreamVector<TStreams...>::GetSize() const noexcept
    {
        return std::get<0>(streams_).size();
    }

    template <typename... TStreams>
    inline void StreamVector<TStreams...>::Reserve(std::size_t capacity)
    {
        ForEachStream([capacity](auto& stream)
        {
            stream.reserve(capacity);
        });
    }

    template <typename... TStreams>
    inline std::size_t StreamVector<TStreams...>::GetCapacity() const noexcept
    {
        return std::get<0>(streams_).capacity();
    }

    template <typename... TStreams>
    inline void StreamVector<TStreams...>::ShrinkToFit()
    {
        ForEachStream([](auto& stream)
        {
            stream.shrink_to_fit();
        });
    }

    template <typename... TStreams>
    inline void StreamVector<TStreams...>::Clear() noexcept
    {
        ForEachStream([](auto& stream)
        {
            stream.clear();
        });
    }

    template <typename... TStreams>
    template <typename... TValues>
    inline void StreamVector<TStreams...>::PushBack(TValues&&... values)
    {
        static_assert(sizeof...(TValues) == sizeof...(TStreams), "Invalid argument count.");

        PushBack<0>(std::forward<TValues>(values)...);
    }

    template <typename... TStreams>
    inline void StreamVector<TStreams...>::PopBack()
    {
        ForEachStream([](auto& stream)
        {
            stream.pop_back();
        });
    }

    template <typename... TStreams>
    inline void StreamVector<TStreams...>::EraseSwap(std::size_t index)
    {
        ForEachStream([index](auto& stream)
        {
            if (index < stream.size() - 1)
            {
                stream[index] = std::move(stream.back());
            }

            stream.pop_back();
        });
    }

    template <typename... TStreams>
    template <std::size_t... kStreams, typename TOperation>
    inline void StreamVector<TStreams...>::ForEach(TOperation&& operation)
    {
        for (auto index = 0u; index < GetSize(); ++index)
        {
            operation(GetElementAt<kStreams>(index)...);
        }
    }

    template <typename... TStreams>
    template <std::size_t... kStreams, typename TOperation>
    inline void StreamVector<TStreams...>::ForEach(TOperation&& operation) const
    {
        for (auto index = 0u; index < GetSize(); ++index)
        {
            operation(GetElementAt<kStreams>(index)...);
        }
    }

    template <typename... TStreams>
    template <typename... TStreamTypes, typename TOperation>
    inline void StreamVector<TStreams...>::ForEach(TOperation&& operation)
    {
        ForEach<tuple_element_index_v<TStreamTypes, std::tuple<TStreams...>>...>(std::forward<TOperation>(operation));
    }

    template <typename... TStreams>
    template <typename... TStreamTypes, typename TOperation>
    inline void StreamVector<TStreams...>::ForEach(TOperation&& operation) const
    {
        ForEach<tuple_element_index_v<TStreamTypes, std::tuple<TStreams...>>...>(std::forward<TOperation>(operation));
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline auto StreamVector<TStreams...>::GetStream()
    {
        if constexpr (std::is_const_v<std::tuple_element_t<kStream, std::tuple<TStreams...>>>)
        {
            return GetConstStream<kStream>();
        }
        else
        {
            return MakeVectorView(std::get<kStream>(streams_));
        }
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline auto StreamVector<TStreams...>::GetStream() const
    {
        return GetConstStream<kStream>();
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline auto StreamVector<TStreams...>::GetConstStream() const
    {
        return MakeConstVectorView(std::get<kStream>(streams_));
    }

    template <typename... TStreams>
    template <typename TStream>
    inline VectorView<TStream> StreamVector<TStreams...>::GetStream()
    {
        return GetStream<tuple_element_index_v<TStream, std::tuple<TStreams...>>>();
    }

    template <typename... TStreams>
    template <typename TStream>
    inline VectorView<const TStream> StreamVector<TStreams...>::GetStream() const
    {
        return GetConstStream<TStream>();
    }

    template <typename... TStreams>
    template <typename TStream>
    inline VectorView<const TStream> StreamVector<TStreams...>::GetConstStream() const
    {
        return GetConstStream<tuple_element_index_v<TStream, std::tuple<TStreams...>>>();
    }

    template <typename... TStreams>
    template <std::size_t kStream, typename TValue, typename... TValues>
    inline void StreamVector<TStreams...>::PushBack(TValue&& value, TValues&&... values)
    {
        std::get<kStream>(streams_).emplace_back(std::forward<TValue>(value));

        PushBack<kStream + 1>(std::forward<TValues>(values)...);
    }

    template <typename... TStreams>
    template <std::size_t kStream>
    inline void StreamVector<TStreams...>::PushBack()
    {

    }

    template <typename... TStreams>
    template <typename TOperation>
    inline void StreamVector<TStreams...>::ForEachStream(TOperation&& operation)
    {
        ForEachStream(std::forward<TOperation>(operation), std::index_sequence_for<TStreams...>{});
    }

    template <typename... TStreams>
    template <typename TOperation, std::size_t kStreamIndex, std::size_t... kStreamIndexes>
    inline void StreamVector<TStreams...>::ForEachStream(TOperation&& operation, std::index_sequence<kStreamIndex, kStreamIndexes...>)
    {
        operation(std::get<kStreamIndex>(streams_));

        ForEachStream(std::forward<TOperation>(operation), std::index_sequence<kStreamIndexes...>{});
    }

    template <typename... TStreams>
    template <typename TOperation>
    inline void StreamVector<TStreams...>::ForEachStream(TOperation&& operation, std::index_sequence<>)
    {

    }

}
