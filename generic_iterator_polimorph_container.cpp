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

template <class Iterator, class Value>
class IteratorBase
{
public:
    explicit IteratorBase(std::unique_ptr<Iterator> it) : it_{std::move(it)} {}

    Value operator * () const { return it_->get(); }
    IteratorBase& operator ++ () { it_->next(); return *this; }
    bool operator == (const IteratorBase& rhs) const { return it_->get() == rhs.it_->get(); }
    bool operator != (const IteratorBase& rhs) const { return !(*this == rhs); }
    
private:
    std::unique_ptr<Iterator> it_; //
};

namespace impl
{
  template<class Value>
  class Iterator
  {
  public:
    using value_type = Value;
  
    virtual value_type get() const = 0;
    virtual void next() = 0;
    virtual bool isEnd() const = 0;
  };
}
    
class ICheckRepository
{
public:
    virtual ~ICheckRepository() = default;
    
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    
    using Iterator = impl::Iterator<Check>;
    using const_iterator = IteratorBase<Iterator, Check>;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};

class MemoryCheckRepository final : public ICheckRepository
{
public:
    using container = std::vector<Check>;

    bool empty() const override { return checks_.empty(); }
    size_t size() const override { return checks_.size(); }

    class Iterator final : public ICheckRepository::Iterator
    {
    public:
        using value_type = ICheckRepository::Iterator::value_type;
        
        Iterator(const container& c) : cCurrent_{c.begin()}, cEnd_{c.end()} {}
        Iterator() = default;

        value_type get() const override { return cCurrent_ != cEnd_ ? *cCurrent_: Check{}; }
        void next() override { if (cCurrent_ != cEnd_) {++cCurrent_;} }
        bool isEnd() const override { return !(cCurrent_ != cEnd_); }

    private:
        container::const_iterator cCurrent_;
        container::const_iterator cEnd_;
    };
    
    const_iterator begin() const override { return const_iterator{std::unique_ptr<ICheckRepository::Iterator>{new Iterator{checks_}}}; }
    const_iterator end() const override { return const_iterator{std::unique_ptr<ICheckRepository::Iterator>{new Iterator{}}}; }

private:
    container checks_{{"a"}, {"b"}, {"c"}};
};

class StoreCheckRepository final : public ICheckRepository
{
public:
    using container = std::vector<Check>;

    bool empty() const override { return checks_.empty(); }
    size_t size() const override { return checks_.size(); }

    class Iterator final : public ICheckRepository::Iterator
    {
    public:
        using value_type = ICheckRepository::Iterator::value_type;
        
        Iterator(const container& c) : cCurrent_{c.begin()}, cEnd_{c.end()} {}
        Iterator() = default;

        value_type get() const override { return cCurrent_ != cEnd_ ? *cCurrent_: Check{}; }
        void next() override { if (cCurrent_ != cEnd_) {++cCurrent_;} }
        bool isEnd() const override { return !(cCurrent_ != cEnd_); }

    private:
        container::const_iterator cCurrent_;
        container::const_iterator cEnd_;
    };
    
    const_iterator begin() const override { return const_iterator{std::unique_ptr<ICheckRepository::Iterator>{new Iterator{checks_}}}; }
    const_iterator end() const override { return const_iterator{std::unique_ptr<ICheckRepository::Iterator>{new Iterator{}}}; }

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
    print(StoreCheckRepository{});

    return 0;
}




