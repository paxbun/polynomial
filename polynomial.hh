// Copyright (c) 2020 Chanjung Kim. All rights reserved.

#ifndef POLYNOMIAL_HH
#define POLYNOMIAL_HH

#ifndef POLYNOMIAL_CH
#    define POLYNOMIAL_CH 'x'
#endif

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

template <typename CoeffT = double, typename OrdT = std::size_t>
struct Term
{
    static_assert(std::is_floating_point_v<CoeffT>,
                  "CoeffTicient must be floating-point type");
    static_assert(std::is_unsigned_v<OrdT>, "OrdTer must be unsigned");

    CoeffT coeff;
    OrdT   order;

    Term(CoeffT coeff = CoeffT { 0 }, OrdT order = OrdT { 0 }) :
        coeff { coeff },
        order { order }
    {}

    bool operator==(const Term& other) const
    {
        return coeff == other.coeff && order == other.order;
    }

    bool operator!=(const Term& other) const
    {
        return !operator==(other);
    }

    Term operator+() const
    {
        return *this;
    }

    Term& operator+=(const Term& other)
    {
        assert(order == other.order);
        coeff += other.coeff;
        return *this;
    }

    Term operator-() const
    {
        return Term { -coeff, order };
    }

    Term& operator-=(const Term& other)
    {
        assert(order == other.order);
        coeff -= other.coeff;
        return *this;
    }

    Term operator*(const Term& other) const
    {
        return Term { coeff * other.coeff, order + other.order };
    }

    Term& operator*=(const Term& other)
    {
        coeff *= other.coeff;
        order += other.order;
        return *this;
    }

    Term operator*(CoeffT scalar) const
    {
        return Term { coeff * scalar, order };
    }

    friend Term operator*(CoeffT scalar, const Term& term)
    {
        return term * scalar;
    }

    Term operator/(const Term& other) const
    {
        assert(order >= other.order);
        return Term { coeff / other.coeff, order - other.order };
    }

    Term& operator/=(const Term& other)
    {
        assert(order >= other.order);
        coeff /= other.coeff;
        order -= other.order;
        return *this;
    }

    Term operator/(CoeffT scalar) const
    {
        return Term { coeff / scalar, order };
    }

    friend Term pow(const Term& term, OrdT order)
    {
        return Term { std::pow(term.coeff, order), term.order * order };
    }
};

template <typename CoeffT,
          typename OrdT,
          typename CharT,
          typename CharTraitsT = std::char_traits<CharT>>
std::basic_ostream<CharT, CharTraitsT>&
operator<<(std::basic_ostream<CharT, CharTraitsT>& os,
           const Term<CoeffT, OrdT>&               term)
{
    if (term.order == 0 || term.coeff != 1) os << term.coeff;

    static const char characters[][4] = { u8"⁰", u8"¹", u8"²", u8"³", u8"⁴",
                                          u8"⁵", u8"⁶", u8"⁷", u8"⁸", u8"⁹" };

    auto order = term.order;
    if (order != 0)
    {
        os << POLYNOMIAL_CH;
        if (order != 1)
            for (char c : std::to_string(order)) os << characters[c - '0'];
    }
    return os;
}

template <typename CoeffT = double, typename OrdT = std::size_t>
struct Polynomial
{
  public:
    using TermType = Term<CoeffT, OrdT>;
    using MapType  = std::map<OrdT, TermType, std::greater<OrdT>>;

  public:
    template <typename CharT       = char,
              typename CharTraitsT = std::char_traits<char>,
              typename AllocT      = std::allocator<char>>
    static Polynomial
    FromString(const std::basic_string<CharT, CharTraitsT, AllocT>& str)
    {
        std::basic_istringstream<CharT, CharTraitsT, AllocT> iss { str };
        std::vector<TermType>                                terms;

        while (iss)
        {
            CharT space_ck;
            iss >> std::skipws >> space_ck;
            if (!iss) break;
            iss.seekg(-1, std::ios_base::cur);

            CoeffT coeff;
            iss >> std::skipws >> coeff;
            bool noCoeff = !iss;
            if (noCoeff)
            {
                if (space_ck == CharT { '+' } || space_ck == CharT { '-' })
                {
                    iss.clear();
                    CharT num_ck;
                    iss >> std::skipws >> num_ck;
                    if (num_ck < CharT { '0' } || CharT { '9' } < num_ck)
                        throw std::invalid_argument { "invalid_polynomial" };
                    iss.seekg(-1, std::ios_base::cur);
                    iss >> std::skipws >> coeff;
                    if (space_ck == CharT { '-' }) coeff = -coeff;
                    noCoeff = !iss;
                }

                if (noCoeff)
                {
                    iss.clear();
                    coeff = CoeffT { 1 };
                }
            }

            CharT ch;
            iss >> std::skipws >> ch;
            if (!iss || ch != POLYNOMIAL_CH)
            {
                if (noCoeff)
                    throw std::invalid_argument { "invalid polynomial" };
                else
                {
                    terms.push_back(coeff);
                    continue;
                }
            }

            CharT pow;
            iss >> std::skipws >> pow;

            if (!iss)
            {
                terms.push_back(coeff);
                break;
            }
            else if (pow != '^')
            {
                iss.seekg(-1, std::ios_base::cur);
                terms.push_back(TermType { coeff, 1 });
                continue;
            }

            OrdT order;
            iss >> order;
            if (!iss) throw std::invalid_argument { "invalid polynomial" };

            terms.push_back(TermType { coeff, order });
        }
        return Polynomial(terms.begin(), terms.end());
    }

    template <typename CharT = char>
    static Polynomial FromString(const CharT* str)
    {
        return FromString(std::basic_string<CharT>(str));
    }

  private:
    template <typename It>
    static void _Optimize(MapType& map, It begin, It end)
    {
        for (auto it = begin; it != end; ++it)
        {
            auto& term = *it;
            if (auto mit = map.find(term.order); mit != map.end())
            {
                if ((mit->second.coeff += term.coeff) == 0.0) map.erase(mit);
            }
            else if (term.coeff != 0.0)
                map.insert(std::make_pair(term.order, term));
        }
    }

    template <typename It>
    static void _OptimizeWithValues(MapType& map,
                                    It       begin,
                                    It       end,
                                    CoeffT   coeff = CoeffT { 1.0 },
                                    OrdT     order = OrdT { 0 })
    {
        struct IteratorAdapter
        {
            It     it;
            CoeffT coeff;
            OrdT   order;

            void operator++()
            {
                ++it;
            }

            auto operator*()
            {
                auto& [key, value] = *it;
                return value * TermType { coeff, order };
            }

            bool operator!=(const IteratorAdapter& other) const
            {
                return it != other.it;
            }
        };

        _Optimize(map,
                  IteratorAdapter { begin, coeff, order },
                  IteratorAdapter { end, coeff, order });
    }

    template <typename It>
    static MapType _Optimize(It begin, It end)
    {
        MapType rtn;
        _Optimize(rtn, begin, end);
        return rtn;
    }

  public:
    MapType terms;

  public:
    Polynomial() {}
    Polynomial(const std::initializer_list<TermType>& init_list) :
        terms { _Optimize(init_list.begin(), init_list.end()) }
    {}
    template <typename It>
    Polynomial(It begin, It end) : terms { _Optimize(begin, end) }
    {}
    Polynomial(const TermType& term) : terms { { term.order, term } } {}
    Polynomial(CoeffT coeff) : Polynomial { TermType { coeff } } {}
    Polynomial(const Polynomial&)     = default;
    Polynomial(Polynomial&&) noexcept = default;
    Polynomial& operator=(const Polynomial&) = default;
    Polynomial& operator=(Polynomial&&) noexcept = default;

  public:
    bool operator==(const Polynomial& other) const
    {
        for (auto lit = terms.begin(), rit = other.terms.begin();
             lit != terms.end() && rit != other.terms.end();
             ++lit, ++rit)
        {
            if (lit->first != rit->first || lit->second != rit->second)
                return false;
        }
        return true;
    }

    bool operator!=(const Polynomial& other) const
    {
        return !operator==(other);
    }

    Polynomial operator+() const
    {
        return *this;
    }

    Polynomial operator+(const Polynomial& other) const
    {
        Polynomial rtn = *this;
        _OptimizeWithValues(rtn.terms, other.terms.begin(), other.terms.end());
        return rtn;
    }

    Polynomial& operator+=(const Polynomial& other)
    {
        _OptimizeWithValues(terms, other.terms.begin(), other.terms.end());
        return *this;
    }

    Polynomial operator-() const
    {
        return operator*(-1.0);
    }

    Polynomial operator-(const Polynomial& other) const
    {
        Polynomial rtn = *this;
        _OptimizeWithValues(
            rtn.terms, other.terms.begin(), other.terms.end(), -1.0);
        return rtn;
    }

    Polynomial& operator-=(const Polynomial& other)
    {
        _OptimizeWithValues(
            terms, other.terms.begin(), other.terms.end(), -1.0);
        return *this;
    }

    Polynomial operator*(CoeffT coeff) const
    {
        Polynomial rtn;
        _OptimizeWithValues(rtn.terms, terms.begin(), terms.end(), coeff);
        return rtn;
    }

    friend Polynomial operator*(CoeffT coeff, const Polynomial& poly)
    {
        return poly * coeff;
    }

    Polynomial& operator*=(CoeffT coeff)
    {
        for (auto& [key, value] : terms) value.coeff *= coeff;
        return *this;
    }

    Polynomial operator*(const Polynomial& other) const
    {
        Polynomial rtn;
        for (auto& [key, value] : other.terms)
        {
            _OptimizeWithValues(rtn.terms,
                                terms.begin(),
                                terms.end(),
                                value.coeff,
                                value.order);
        }
        return rtn;
    }

    Polynomial& operator*=(const Polynomial& other)
    {
        MapType newTerms;
        for (auto& [key, value] : other.terms)
        {
            _OptimizeWithValues(
                newTerms, terms.begin(), terms.end(), value.coeff, value.order);
        }
        terms = newTerms;
        return *this;
    }

    Polynomial operator/(CoeffT coeff) const
    {
        Polynomial rtn;
        _OptimizeWithValues(rtn.terms, terms.begin(), terms.end(), 1.0 / coeff);
        return rtn;
    }

    Polynomial& operator/=(CoeffT coeff)
    {
        for (auto& [key, value] : terms) value.coeff /= coeff;
        return *this;
    }

    friend Polynomial pow(const Polynomial& term, OrdT order)
    {
        Polynomial rtn { 1 };
        for (OrdT i = 0; i < order; ++i) rtn *= term;
        return rtn;
    }
};

template <typename CoeffT, typename OrdT>
Polynomial<CoeffT, OrdT> operator+(const typename Term<CoeffT, OrdT>& lhs,
                                   const typename Term<CoeffT, OrdT>& rhs)
{
    return Polynomial<CoeffT, OrdT> { lhs, rhs };
}

template <typename CoeffT, typename OrdT>
Polynomial<CoeffT, OrdT> operator-(const typename Term<CoeffT, OrdT>& lhs,
                                   const typename Term<CoeffT, OrdT>& rhs)
{
    return Polynomial<CoeffT, OrdT> { lhs, -rhs };
}

template <typename CoeffT,
          typename OrdT,
          typename CharT,
          typename CharTraitsT = std::char_traits<CharT>>
std::basic_ostream<CharT, CharTraitsT>&
operator<<(std::basic_ostream<CharT, CharTraitsT>& os,
           const Polynomial<CoeffT, OrdT>&         poly)
{
    bool is_first = true;
    for (auto& [ord, term] : poly.terms)
    {
        if (term.coeff > 0 && !is_first) os << '+';
        os << term;
        is_first = false;
    }
    return os;
}

#endif
