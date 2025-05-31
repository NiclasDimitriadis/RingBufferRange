#pragma once

#include <algorithm>
#include <bit>
#include <cstddef>
#include <iterator>
#include <memory>
#include <new>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>

namespace RingBufferRange {

template<typename ContentType, size_t length>
requires std::is_default_constructible_v<ContentType> && std::is_nothrow_copy_assignable_v<ContentType> && (std::has_single_bit(length))
struct RingBufferRange {
private:
   std::int64_t n_enqueues = 0;
   std::int64_t n_dequeues = 0;
   const std::unique_ptr<ContentType[]> mem_ptr;
   const std::span<ContentType, length> mem_span;

public:
   struct Iterator {
   public:
      using iterator_category = std::forward_iterator_tag;
      using difference_type = std::ptrdiff_t;
      using value_type = ContentType;
      using Pointer = ContentType*;
      using Reference = ContentType&;
      explicit Iterator() = default;
      Iterator(Pointer const, Pointer const);
      Iterator(const Iterator&) = default;
      Iterator(Iterator&&) = default;
      Iterator& operator=(const Iterator&) = default;
      Iterator& operator=(Iterator&&) = default;
      Reference operator*() const noexcept;
      Iterator& operator++() noexcept;
      Iterator operator++(int) noexcept;
      bool operator==(const Iterator& other) const noexcept;

   private:
      using SpanType = const std::span<ContentType, length>;
      Pointer base_ptr;
      difference_type position;
   };

   explicit RingBufferRange();
   ~RingBufferRange() = default;
   RingBufferRange(RingBufferRange&) = delete;
   RingBufferRange(RingBufferRange&&) = delete;
   RingBufferRange& operator=(RingBufferRange&) = delete;
   RingBufferRange& operator=(RingBufferRange&&) = delete;
   Iterator begin() const noexcept;
   Iterator end() const noexcept;
   [[nodiscard]] __attribute__((flatten)) std::uint64_t n_entries() const noexcept;
   __attribute__((flatten)) bool pop() noexcept;
   [[nodiscard]] __attribute__((flatten)) bool enqueue(const ContentType& enqueue_content) noexcept;
   [[nodiscard]] __attribute__((flatten)) std::optional<ContentType> dequeue() noexcept;

   template<typename... Args>
   requires std::is_nothrow_constructible_v<ContentType, Args...> && std::is_move_assignable_v<ContentType>
   [[nodiscard]] __attribute__((flatten)) bool emplace(Args...) noexcept;
};
}; // namespace RingBufferRange

#define TEMPL_PARAMS template<typename ContentType, size_t length> \
requires std::is_default_constructible_v<ContentType> && std::is_nothrow_copy_assignable_v<ContentType> && (std::has_single_bit(length))
#define BUFFER_RANGE RingBufferRange::RingBufferRange<ContentType, length>
#define ITERATOR RingBufferRange::RingBufferRange<ContentType, length>::Iterator

TEMPL_PARAMS
ITERATOR::Iterator(Pointer const pos, Pointer const base):
    base_ptr{base}, position{pos - base} {};

TEMPL_PARAMS
ITERATOR::Reference ITERATOR::operator*() const noexcept {
   SpanType buffer_span{this->base_ptr, length};
   return buffer_span[position];
};

TEMPL_PARAMS
ITERATOR& ITERATOR::operator++() noexcept {
   this->position = (this->position + 1) % length;
   return *this;
};

TEMPL_PARAMS
ITERATOR ITERATOR::operator++(int) noexcept {
   Iterator tmp_iter = *this;
   ++(*this);
   return tmp_iter;
};

TEMPL_PARAMS
bool ITERATOR::operator==(const Iterator& other) const noexcept {
   return (this->base_ptr == other.base_ptr) && (this->position == other.position);
};

TEMPL_PARAMS
BUFFER_RANGE::RingBufferRange():
    mem_ptr{new(std::align_val_t{std::max((size_t)64, alignof(ContentType))}) ContentType[length]()}, mem_span{mem_ptr.get(), length} {};

TEMPL_PARAMS
ITERATOR BUFFER_RANGE::begin() const noexcept {
   return Iterator(&this->mem_span[n_dequeues % length], &this->mem_span[0]);
};

TEMPL_PARAMS
ITERATOR BUFFER_RANGE::end() const noexcept {
   return Iterator(&this->mem_span[n_enqueues % length], &this->mem_span[0]);
};

TEMPL_PARAMS
std::uint64_t BUFFER_RANGE::n_entries() const noexcept {
  return this->n_enqueues - this->n_dequeues;
};

TEMPL_PARAMS
bool BUFFER_RANGE::pop() noexcept {
   const bool not_empty = this->n_dequeues < this->n_enqueues;
   this->n_dequeues += not_empty;
   return not_empty;
};

TEMPL_PARAMS
bool BUFFER_RANGE::enqueue(const ContentType& enqueue_content) noexcept {
   const bool slot_available = this->n_enqueues < (this->n_dequeues + length);
   this->mem_span[this->n_enqueues % length] = slot_available ? enqueue_content : this->mem_span[n_enqueues % length];
   this->n_enqueues += slot_available;
   return slot_available;
};

TEMPL_PARAMS
std::optional<ContentType> BUFFER_RANGE::dequeue() noexcept {
   const bool not_empty = n_enqueues > n_dequeues;
   const std::optional<ContentType> ret = not_empty ? std::make_optional(this->mem_span[n_dequeues % length]) : std::nullopt;
   this->n_dequeues += not_empty;
   return ret;
};

TEMPL_PARAMS
template<typename... Args>
requires std::is_nothrow_constructible_v<ContentType, Args...> && std::is_move_assignable_v<ContentType>
bool BUFFER_RANGE::emplace(Args... args) noexcept {
   const bool slot_available = this->n_enqueues < (this->n_dequeues + length);
   ContentType* const construct_location = &this->mem_span[this->n_enqueues % length];
   *construct_location = slot_available ? ContentType(std::forward<Args>(args)...) : *construct_location;
   this->n_enqueues += slot_available;
   return slot_available;
};

#undef TEMPL_PARAMS
#undef BUFFER_RANGE
#undef ITERATOR
