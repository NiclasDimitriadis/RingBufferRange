## RingBufferRange
Simple implementation of a ring buffer that makes its content accessible to STL algorithms including modern range based algorithms first introduced in C++20 

- exposes an `Iterator` class for accessing entries
- only forward iteration is supported

#### RingBufferRange class template
`template<typename ContentType, size_t length>`<br>
`requires std::is_default_constructible_v<ContentType> && std::is_nothrow_copy_assignable_v<ContentType>`<br> `&& (std::has_single_bit(length))`<br>
`struct RingBufferRange`
- `ContentType`: type that will be enqueued
- type constaints ensure that queue elements can be default constructed when the queue object is constructed, (nothrow) copy assigned when an element is enqueued and (nothrow) copy constructed when it's dequeued
- `length`: max number of elements the queue can contain, determines the size of the ring buffer
- constrained to powers of two to prevent costly modulus operations when computing indices in ring buffer

### relevant members and interfaces

#### `Iterator`
- forward iterator type exposed to access content of ring buffer

#### `Iterator::Iterator(ContentType* const, ContentType* const)`
- constructor for iterator member type, takes a pointer to the base of the ring buffer and a pointer to the first element as arguments

#### `Iterator::operator*`
`ContentType& Iterator::operator*() const noexcept`
- dereference operator
- returns a reference to the buffere entry the iterator object currently points to

#### `Iterator::operator++`
`const Iterator& Iterator::operator++() noexcept`
- increments iterator to next position in buffer

#### `Iterator::operator==`
`bool Iterator::operator==(const Iterator&) const noexcept`
- comparison operator
- determines whether two iterators point to the same position within the buffer

#### `begin`
`Iterator begin() const noexcept`
- returns an `Iterator` object pointing to the position after the last dequeued entry within the buffer
- points to the first element if buffer is not empty

#### `end` 
`Iterator end() const noexcept`
- returns an `Iterator` pointing to the position after the last enqueued entry within the buffer
- points to position behind last element if buffer is not empty

#### `n_entries`
`std::uint64_t n_entries() const noexcept`
- returns the number of entries currently currently contained in the ring buffer

#### `pop`
`bool pop() noexcept`
- removes first entry from buffer if buffer is not empty
- returns `false` if buffer was not empty and `true` otherwise

#### `enqueue`
`bool enqueue(const ContentType&) noexcept`
- adds a new entry to the buffer provided its not currently filled to capacity
- returns `true` if entry could be added and `false` otherwise 
 
#### `dequeue`
`std::optional<ContentType> dequeue() noexcept`
- removes the first element from the buffer provided the buffer is not empty and returns it within a `std::optional`
- returns a `std::optional` without a values if buffer was empty

#### `emplace`
`template<typename... Args>`<br>
`requires std::is_nothrow_constructible_v<ContentType, Args...> && std::is_move_assignable_v<ContentType>`<br>
`bool emplace(Args... args) noexcept`
- contructs an entry in-place using `args` as arguments for the constructor of `ContentType`
