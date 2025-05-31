#define DOCTEST_CONFIG_IMPLEMENT

#include "RingBufferRange.hpp"
#include "doctest.h"


TEST_CASE("RingBufferRange::RingBufferRange") {
   struct TestClass{
   private:
      int a;
      int b;
   public:
      TestClass() = default;
      TestClass(int arg1, int arg2) noexcept: a{arg1}, b{arg2}{};
      TestClass(const TestClass&) = default;
      TestClass& operator=(const TestClass& other) noexcept{
         this->a = other.a;
         this->b = other.b;
         return *this;
      };
      TestClass(const TestClass&&) = delete;
      TestClass& operator=(const TestClass&& other){
         this->a = other.a;
         this->b = other.b;
         return *this;
      };
      bool operator==(const TestClass&) const = default;
   };
   using TestBufferClass4 = RingBufferRange::RingBufferRange<TestClass, 4>;
   using TestBufferClass8 = RingBufferRange::RingBufferRange<TestClass, 8>;
   using TestBuffer32Int = RingBufferRange::RingBufferRange<int, 32>;
   using TestBuffer8Int = RingBufferRange::RingBufferRange<int, 8>;
   using TestBuffer4Int = RingBufferRange::RingBufferRange<int, 4>;

   SUBCASE("testing simple enqueueing and dequeueing"){
      TestBuffer32Int test_buffer;
      const bool enq_res = test_buffer.enqueue(658);
      CHECK(enq_res == true);
      const auto deq_res = test_buffer.dequeue();
      CHECK(deq_res.has_value());
      CHECK(deq_res.value() == 658);
      const auto deq_res2 = test_buffer.dequeue();
      CHECK(!deq_res2.has_value());
   };

   SUBCASE("testing emplace method"){
      TestBufferClass4 test_buffer;
      const auto test_object = TestClass(22, 66);
      const bool enq_res = test_buffer.emplace(22, 66);
      CHECK(enq_res == true);
      const auto deq_res = test_buffer.dequeue();
      CHECK(deq_res.has_value());
      CHECK(deq_res.value() == test_object);
   };

   SUBCASE("testing iterator and range functionality"){
      TestBuffer8Int test_buffer;
      CHECK(std::ranges::distance(test_buffer) == 0);
      CHECK(test_buffer.n_entries() == 0);
      for(int i = 0; i <= 6; ++i){
         const bool enq_res = test_buffer.enqueue(i);
      };
      CHECK(test_buffer.n_entries() == 7);
      auto it1 = test_buffer.begin();
      auto it2 = test_buffer.end();
      CHECK(std::ranges::distance(test_buffer) == 7);
      CHECK(*test_buffer.begin() == 0);
      CHECK(*(++test_buffer.begin()) == 1);
      for(int i = 0; i <= 6; ++i){
         const auto deq_res = test_buffer.dequeue();
      };
      CHECK(std::ranges::distance(test_buffer) == 0);
      for(int i = 0; i <= 5; ++i){
         const bool enq_res = test_buffer.enqueue(i);
      };
      CHECK(std::ranges::distance(test_buffer) == 6);
   };
};

int main() {
   doctest::Context context;
   context.run();
};
