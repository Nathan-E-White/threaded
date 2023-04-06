#ifndef THREADED_NEGATIVE_INFINITY_Q_TCC
#define THREADED_NEGATIVE_INFINITY_Q_TCC

#include <limits>


namespace Predicates {

    template <typename N>
    class NegativeInfinityQ {

    private:
        /// The Negative infinity value for the given type N
        static const constinit auto
        neg_inf = std::numeric_limits<N>::infinity();

    public:
        constexpr static auto operator()(N const &n) -> bool;

        constexpr static auto operator()(N const &lhs, N const &rhs) -> bool;

    };
}

#endif