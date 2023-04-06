#ifndef THREADED_POSITIVE_INFINITY_Q_TCC
#define THREADED_POSITIVE_INFINITY_Q_TCC

#include <limits>


namespace Predicates {

    template <typename N>
    class PositiveInfinityQ {

    private:
        /// The positive infinity value for the given type N
        static const constinit auto
        pos_inf = std::numeric_limits<N>::infinity();

    public:
        constexpr static auto operator()(N const &n) -> bool;

        constexpr static auto operator()(N const &lhs, N const &rhs) -> bool;

    };
}

#endif