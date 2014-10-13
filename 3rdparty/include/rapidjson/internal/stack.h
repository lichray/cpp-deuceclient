// Copyright (C) 2011 Milo Yip
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef RAPIDJSON_INTERNAL_STACK_H_
#define RAPIDJSON_INTERNAL_STACK_H_

#include <type_traits>
#include <limits>
#include <algorithm>

#if defined(_MSC_VER)
#define NOMINMAX
#endif

namespace rapidjson {
namespace internal {

template <int N, typename Int>
inline RAPIDJSON_FORCEINLINE Int or_shift(Int n)
{
    auto r = n | (n >> N);

    if (N == 1)
        return r;
    else
        return or_shift<N == 1 ? 1 : N / 2>(r);
}

template <typename Int>
inline auto pow2_roundup(Int n) -> typename std::make_unsigned<Int>::type
{
    typedef typename std::make_unsigned<Int>::type R;

    return or_shift<std::numeric_limits<R>::digits / 2>(R(n) - 1) + 1;
}

///////////////////////////////////////////////////////////////////////////////
// Stack

//! A type-unsafe stack for storing different types of data.
/*! \tparam Allocator Allocator for allocating stack memory.
*/
template <typename Allocator>
class Stack {
public:
    // Optimization note: Do not allocate memory for stack_ in constructor.
    // Do it lazily when first Push() -> Expand() -> Resize().
    Stack(Allocator* allocator, size_t stackCapacity) : allocator_(allocator), ownAllocator(0), stack_(0), stackTop_(0), stackEnd_(0), initialCapacity_(stackCapacity) {
        RAPIDJSON_ASSERT(stackCapacity > 0);
        if (!allocator_)
            ownAllocator = allocator_ = new Allocator();
    }

    ~Stack() {
        Allocator::Free(stack_);
        delete ownAllocator; // Only delete if it is owned by the stack
    }

    void Clear() { stackTop_ = stack_; }

    void ShrinkToFit() { 
        if (Empty()) {
            // If the stack is empty, completely deallocate the memory.
            Allocator::Free(stack_);
            stack_ = 0;
            stackTop_ = 0;
            stackEnd_ = 0;
        }
        else
            Resize(GetSize());
    }

    // Optimization note: try to minimize the size of this function for force inline.
    // Expansion is run very infrequently, so it is moved to another (probably non-inline) function.
    template<typename T>
    RAPIDJSON_FORCEINLINE T* Push(size_t count = 1) {
         // Expand the stack if needed
        if (stackTop_ + sizeof(T) * count >= stackEnd_)
            Expand<T>(count);

        T* ret = reinterpret_cast<T*>(stackTop_);
        stackTop_ += sizeof(T) * count;
        return ret;
    }

    template<typename T>
    T* Pop(size_t count) {
        RAPIDJSON_ASSERT(GetSize() >= count * sizeof(T));
        stackTop_ -= count * sizeof(T);
        return reinterpret_cast<T*>(stackTop_);
    }

    template<typename T>
    T* Top() { 
        RAPIDJSON_ASSERT(GetSize() >= sizeof(T));
        return reinterpret_cast<T*>(stackTop_ - sizeof(T));
    }

    template<typename T>
    T* Bottom() { return (T*)stack_; }

    Allocator& GetAllocator() { return *allocator_; }
    bool Empty() const { return stackTop_ == stack_; }
    size_t GetSize() const { return static_cast<size_t>(stackTop_ - stack_); }
    size_t GetCapacity() const { return static_cast<size_t>(stackEnd_ - stack_); }

private:
    template<typename T>
    void Expand(size_t count) {
        // Only expand the capacity if the current stack exists. Otherwise create a stack based on initial capacity.
        Resize(pow2_roundup(
            std::max(initialCapacity_, GetSize() + sizeof(T) * count)));
    }

    void Resize(size_t newCapacity) {
        const size_t size = GetSize();  // Backup the current size
        stack_ = (char*)allocator_->Realloc(stack_, GetCapacity(), newCapacity);
        stackTop_ = stack_ + size;
        stackEnd_ = stack_ + newCapacity;
    }

    // Prohibit copy constructor & assignment operator.
    Stack(const Stack&);
    Stack& operator=(const Stack&);

    Allocator* allocator_;
    Allocator* ownAllocator;
    char *stack_;
    char *stackTop_;
    char *stackEnd_;
    size_t initialCapacity_;
};

} // namespace internal
} // namespace rapidjson

#endif // RAPIDJSON_STACK_H_
