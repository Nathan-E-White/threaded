#include "PositiveInfinityQ.tcc"

namespace Predicates {

    template<typename N>
    constexpr auto PositiveInfinityQ<N>::operator()(const N &n) -> bool {
        return n == pos_inf;
    }

    template<typename N>
    constexpr auto PositiveInfinityQ<N>::operator()(const N &lhs, const N &rhs) -> bool {
        return PositiveInfinityQ<N>::operator()(lhs) || PositiveInfinityQ<N>::operator()(rhs);
    }

}