#include "NegativeInfinityQ.tcc"

namespace Predicates {

    template<typename N>
    constexpr auto NegativeInfinityQ<N>::operator()(const N &n) -> bool {
        return n == neg_inf;
    }

    template<typename N>
    constexpr auto NegativeInfinityQ<N>::operator()(const N &lhs, const N &rhs) -> bool {
        return NegativeInfinityQ<N>::operator()(lhs) || NegativeInfinityQ<N>::operator()(rhs);
    }

}