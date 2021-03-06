#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cassert>


template <class Container, class Value>
class ContainerBase
{
public:
    ContainerBase(Container& c) : impl_{c} {}
    bool empty() const { return impl_.empty(); }
    size_t size() const { return impl_.size(); }

    class Iterator;
    Iterator begin() const { return impl_.begin(); }
    Iterator end() const { return impl_.end(); }

    class Iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = Value;
        using iterator_category = std::forward_iterator_tag;
        using IteratorImpl = typename Container::IteratorImpl;
        explicit Iterator(IteratorImpl&& impl) : impl_{std::move(impl)} {}

        Value operator * () const { return impl_.get(); }
        Iterator& operator ++ () { impl_.next(); return *this; }
        Iterator  operator ++ (int) { Iterator tmp{*this}; ++*this; return tmp; }
        bool operator == (const Iterator& rhs) const { return impl_.isEnd() || rhs.impl_.isEnd() ? impl_.isEnd() && rhs.impl_.isEnd(): impl_.get() == rhs.impl_.get(); }
        bool operator != (const Iterator& rhs) const { return !(*this == rhs); }

    private:
        IteratorImpl impl_;
    };
private:
    Container& impl_;
};

template<typename Value>
class MemoryContainer : public ContainerBase<MemoryContainer<Value>, Value>
{
public:
    using Base = ContainerBase<MemoryContainer<Value>, Value>;
    using Iterator = typename Base::Iterator;

    MemoryContainer() : Base{*this} {}
    
    void add(Value&& newValue) {cont_.push_back(std::move(newValue));}

    bool empty() const { return cont_.empty(); }
    size_t size() const { return cont_.size(); }

    Iterator begin() const { return Iterator{IteratorImpl{cont_}}; }
    Iterator end() const { return Iterator{IteratorImpl{}}; }

    class IteratorImpl
    {
    public:
        explicit IteratorImpl(const std::vector<Value>& c) : cCurrent_{c.begin()}, cEnd_{c.end()} {}
        explicit IteratorImpl() = default;

        Value get() const { return cCurrent_ != cEnd_ ? *cCurrent_: Value{}; }
        void next() { if (cCurrent_ != cEnd_) {++cCurrent_;} }
        bool isEnd() const { return !(cCurrent_ != cEnd_); }

    private:
        using const_iterator = typename std::vector<Value>::const_iterator;
        const_iterator cCurrent_;
        const_iterator cEnd_;
    };

private:
    std::vector<Value> cont_{};
};

struct Check
{
  std::string name;
};
bool operator == (const Check& lhs, const Check& rhs) { return lhs.name == rhs.name; }
std::ostream& operator<<(std::ostream& os, const Check& check) { os << check.name; return os;}
using CheckMemoryContainer = MemoryContainer<Check>;

struct Message
{
  size_t id;
};
bool operator == (const Message& lhs, const Message& rhs) { return lhs.id == rhs.id; }
std::ostream& operator<<(std::ostream& os, const Message& message) { os << message.id; return os;}
using MessageMemoryContainer = MemoryContainer<Message>;

template<class Container>
void print(const Container& container)
{
    assert(3 == container.size());
    for (auto item : container)
    {
        std::cout << item << '\n';
    }
}

int main()
{
    printf("Print check container\n");
    CheckMemoryContainer cmc{};
    cmc.add({"a"});
    cmc.add({"b"});
    cmc.add({"c"});
    print<CheckMemoryContainer>(cmc);

    printf("\nPrint message container\n");
    MessageMemoryContainer mmc{};
    mmc.add({1});
    mmc.add({2});
    mmc.add({3});
    print<MessageMemoryContainer>(mmc);
  
    return 0;
}
