//
// Created by Nathan White on 12/5/22.
//

#ifndef THREADED_ADDITIONOVERFLOWCHECK_TCC
#define THREADED_ADDITIONOVERFLOWCHECK_TCC

#include <type_traits>
#include <cmath>
#include <functional>
#include <algorithm>
#include <execution>
#include <vector>
#include <thread>
#include <hash_map>
#include <optional>


template<typename N> requires std::is_arithmetic_v<N>
class AdditionOverflowCheck {

    static const
    constinit auto
    pos_inf = std::numeric_limits<N>::infinity();

    static const
    constinit auto
    neg_inf = std::minus<N>()(std::numeric_limits<N>::infinity());
    static const
    constinit auto
    nan = std::numeric_limits<N>::quiet_NaN();

    static const
    constinit auto
    pos_inf_ptr = std::make_optional(pos_inf);

    static const
    constinit auto
    neg_inf_ptr = std::make_optional(neg_inf);

    static const
    constinit auto
    nan_ptr = std::make_optional(nan);

public:

    static auto operator()(N lhs, N rhs) -> bool {
        if (lhs == pos_inf || rhs == pos_inf) {
            return true;
        } else if (lhs == neg_inf || rhs == neg_inf) {
            return true;
        } else if (lhs == nan || rhs == nan) {
            return true;
        } else {
            /* Check if adding lhs and rhs will cause an overflow */
            if (lhs > 0 && rhs > 0 && lhs > pos_inf - rhs) {
                return true;
            } else if (lhs < 0 && rhs < 0 && lhs < neg_inf - rhs) {
                return true;
            } else {
                return false;
            }
        }
    }
};


#endif //THREADED_ADDITIONOVERFLOWCHECK_TCC
