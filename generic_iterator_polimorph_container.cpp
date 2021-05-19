#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <cassert>

struct Check
{
  std::string name;
};

bool operator == (const Check& lhs, const Check& rhs) { return lhs.name == rhs.name; }

template <class IteratorImpl, class Value>
class Iterator
{
public:
    explicit Iterator(std::unique_ptr<IteratorImpl> impl) : impl_{std::move(impl)} {}

    Value operator * () const { return impl_->get(); }
    Iterator& operator ++ () { impl_->next(); return *this; }
    bool operator == (const Iterator& rhs) const { return impl_->get() == rhs.impl_->get(); }
    bool operator != (const Iterator& rhs) const { return !(*this == rhs); }
    
private:
    std::unique_ptr<IteratorImpl> impl_; //
};
    
class ICheckRepository
{
public:
    virtual ~ICheckRepository() = default;
    
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    
    class CheckIteratorBase
    {
    public:
        virtual Check get() const = 0;
        virtual void next() = 0;
        virtual bool isEnd() const = 0;
    };
    
    using const_iterator = Iterator<CheckIteratorBase, Check>;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};

class MemoryCheckRepository final : public ICheckRepository
{
public:
    using ICheckRepository::ICheckRepository;
    using container = std::vector<Check>;

    bool empty() const override { return checks_.empty(); }
    size_t size() const override { return checks_.size(); }

    class CheckIterator final : public CheckIteratorBase
    {
    public:
        CheckIterator(const container& c) : cCurrent_{c.begin()}, cEnd_{c.end()} {}
        CheckIterator() = default;

        Check get() const override { return cCurrent_ != cEnd_ ? *cCurrent_: Check{}; }
        void next() override { if (cCurrent_ != cEnd_) {++cCurrent_;} }
        bool isEnd() const override { return !(cCurrent_ != cEnd_); }

    private:
        container::const_iterator cCurrent_;
        container::const_iterator cEnd_;
    };
    
    const_iterator begin() const override { return const_iterator{std::unique_ptr<CheckIteratorBase>{new CheckIterator{checks_}}}; }
    const_iterator end() const override { return const_iterator{std::unique_ptr<CheckIteratorBase>{new CheckIterator{}}}; }

private:
    container checks_{{"a"}, {"b"}, {"c"}};
};

void print(const ICheckRepository& checks)
{
    assert(3 == checks.size());
    for (Check check : checks)
    {
        std::cout << check.name << '\n';
    }
}

int main()
{
    printf("Print check containers\n");
    print(MemoryCheckRepository{});

    return 0;
}





