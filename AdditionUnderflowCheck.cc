#include "AdditionUnderflowCheck.tcc"
#include "PositiveInfinityQ.tcc"
#include "NegativeInfinityQ.tcc"

template<typename N>
requires std::is_arithmetic_v<N>constexpr auto AdditionUnderflowCheck<N>::operator()(N lhs, N rhs) -> bool {

    auto ipf = Predicates::PositiveInfinityQ<N>::operator()(lhs, rhs);
    auto npf = Predicates::NegativeInfinityQ<N>::operator()(lhs, rhs);

    if (ipf || npf) {
        return true;
    }


    if (
            (lhs == neg_inf || rhs == neg_inf) ||
            (lhs == nan || rhs == nan)) {

        /* These could be underflow, they aren't safe to add */
        return true;

    } else {
        /* Check if adding lhs and rhs will cause an underflow */
        if ((lhs > 0 && rhs > 0 && lhs < neg_inf + rhs) ||
            (lhs < 0 && rhs < 0 && lhs < neg_inf + rhs)) {

            /* Underflow */
            return true;

        } else {

            /* Ok to add */
            return false;
        }
    }
}

template<typename N>
requires std::is_arithmetic_v<N>constexpr auto
AdditionUnderflowCheck<N>::operator()(const std::vector<N> &lhs, N rhs) -> std::vector<bool> {
    std::vector<bool> result;
    result.reserve(lhs.size());
    std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), std::back_inserter(result),
                   [rhs](N lhs) { return operator()(lhs, rhs); });
    return result;
}


template<typename N>
requires std::is_arithmetic_v<N>constexpr auto
AdditionUnderflowCheck<N>::operator()(const std::vector<N> &lhs, const std::vector<N> &rhs) -> std::vector<bool> {
    std::vector<bool> result;
    result.reserve(lhs.size());
    std::transform(std::execution::par_unseq, lhs.begin(), lhs.end(), rhs.begin(), std::back_inserter(result),
                   operator());
    return result;
}


template<typename N>
requires std::is_arithmetic_v<N>constexpr auto
AdditionUnderflowCheck<N>::operator()(N lhs, const std::vector<N> &rhs) -> std::vector<bool> {
    return AdditionUnderflowCheck<N>::operator()(rhs, lhs);
}
