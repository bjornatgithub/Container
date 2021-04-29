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

namespace impl
{
    class ContainerIt
    {
    public:
        virtual Check get() const = 0;
        virtual void next() = 0;
        virtual bool isEnd() const = 0;
        
        bool isEqual(const ContainerIt& rhs) const { return isEnd() || rhs.isEnd() ? isEnd() && rhs.isEnd() : get() == rhs.get(); }
    };
}

class ContainerBase
{
public:
    virtual ~ContainerBase() = default;
    
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    
    class Iterator;
    using const_iterator = Iterator;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
    
    class Iterator
    {
    public:
        explicit Iterator(std::unique_ptr<impl::ContainerIt> impl) : impl_{std::move(impl)} {}

        Check operator * () const { return impl_->get(); }
        Iterator& operator ++ () { impl_->next(); return *this; }
        bool operator == (const Iterator& rhs) const { return impl_->isEqual(*rhs.impl_.get()); }
        bool operator != (const Iterator& rhs) const { return !(*this == rhs); }

        std::unique_ptr<impl::ContainerIt> impl_; //
    };
};

class MemoryContainer : public ContainerBase
{
public:
    using container = std::vector<Check>;

    bool empty() const override { return checks_.empty(); }
    size_t size() const override { return checks_.size(); }

    class ItMem;
    const_iterator begin() const override { return const_iterator{std::unique_ptr<impl::ContainerIt>{new ItMem{checks_}}}; }
    const_iterator end() const override { return const_iterator{std::unique_ptr<impl::ContainerIt>{new ItMem{}}}; }

    class ItMem : public impl::ContainerIt
    {
    public:
        ItMem(const container& c) : cCurrent_{c.begin()}, cEnd_{c.end()} {}
        ItMem() = default;

        Check get() const override { return cCurrent_ != cEnd_ ? *cCurrent_: Check{}; }
        void next() override { if (cCurrent_ != cEnd_) {++cCurrent_;} }
        bool isEnd() const override { return !(cCurrent_ != cEnd_); }

    private:
        container::const_iterator cCurrent_;
        container::const_iterator cEnd_;
    };

private:
    container checks_{{"a"}, {"b"}, {"c"}};
};

void print(const ContainerBase& container)
{
    assert(3 == container.size());
    for (Check check : container)
    {
        std::cout << check.name << '\n';
    }
}

int main()
{
    printf("Print check containers\n");
    print(MemoryContainer{});

    return 0;
}
