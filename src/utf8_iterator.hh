#ifndef utf8_iterator_hh_INCLUDED
#define utf8_iterator_hh_INCLUDED

#include "utf8.hh"

#include <iterator>

namespace Kakoune
{

namespace utf8
{

// adapter for an iterator on bytes which permits to iterate
// on unicode codepoints instead.
template<typename BaseIt,
         typename CodepointType = Codepoint,
         typename DifferenceType = CharCount,
         typename InvalidPolicy = utf8::InvalidPolicy::Pass>
class iterator : public std::iterator<std::bidirectional_iterator_tag,
                                      CodepointType, DifferenceType>
{
public:
    iterator() = default;
    constexpr static bool noexcept_policy = noexcept(InvalidPolicy{}(0));

    iterator(BaseIt it, BaseIt begin, BaseIt end) noexcept
        : m_it{std::move(it)}, m_begin{std::move(begin)}, m_end{std::move(end)}
    {}

    template<typename Container>
    iterator(BaseIt it, const Container& c) noexcept
        : m_it{std::move(it)}, m_begin{std::begin(c)}, m_end{std::end(c)}
    {}

    iterator& operator++() noexcept
    {
        utf8::to_next(m_it, m_end);
        invalidate_value();
        return *this;
    }

    iterator operator++(int) noexcept
    {
        iterator save = *this;
        ++*this;
        return save;
    }

    iterator& operator--() noexcept
    {
        utf8::to_previous(m_it, m_begin);
        invalidate_value();
        return *this;
    }

    iterator operator--(int) noexcept
    {
        iterator save = *this;
        --*this;
        return save;
    }

    iterator operator+(DifferenceType count) const noexcept
    {
        if (count < 0)
            return operator-(-count);

        iterator res = *this;
        while (count--)
            ++res;
        return res;
    }

    iterator operator-(DifferenceType count) const noexcept
    {
        if (count < 0)
            return operator+(-count);

        iterator res = *this;
        while (count--)
            --res;
        return res;
    }

    bool operator==(const iterator& other) const noexcept { return m_it == other.m_it; }
    bool operator!=(const iterator& other) const noexcept { return m_it != other.m_it; }

    bool operator< (const iterator& other) const noexcept { return m_it < other.m_it; }
    bool operator<= (const iterator& other) const noexcept { return m_it <= other.m_it; }

    bool operator> (const iterator& other) const noexcept { return m_it > other.m_it; }
    bool operator>= (const iterator& other) const noexcept { return m_it >= other.m_it; }

    bool operator==(const BaseIt& other) noexcept { return m_it == other; }
    bool operator!=(const BaseIt& other) noexcept { return m_it != other; }

    bool operator< (const BaseIt& other) const noexcept { return m_it < other; }
    bool operator<= (const BaseIt& other) const noexcept { return m_it <= other; }

    bool operator> (const BaseIt& other) const noexcept { return m_it > other; }
    bool operator>= (const BaseIt& other) const noexcept { return m_it >= other; }

    DifferenceType operator-(const iterator& other) const noexcept(noexcept_policy)
    {
        return (DifferenceType)utf8::distance<InvalidPolicy>(other.m_it, m_it);
    }

    CodepointType operator*() const noexcept(noexcept_policy)
    {
        return get_value();
    }

    const BaseIt& base() const noexcept(noexcept_policy) { return m_it; }

private:
    void invalidate_value() noexcept { m_value = -1; }
    CodepointType get_value() const noexcept(noexcept_policy)
    {
        if (m_value == (CodepointType)-1)
            m_value = (CodepointType)utf8::codepoint<InvalidPolicy>(m_it, m_end);
        return m_value;
    }

    BaseIt m_it;
    BaseIt m_begin;
    BaseIt m_end;
    mutable CodepointType m_value = -1;
};

}

}
#endif // utf8_iterator_hh_INCLUDED
