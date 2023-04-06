//
// Created by Nathan White on 12/5/22.
//

#ifndef THREADED_NPLUS_TCC
#define THREADED_NPLUS_TCC


#include <cmath>
#include <functional>
#include <algorithm>
#include <execution>
#include <vector>
#include <thread>
#include <hash_map>
#include <optional>
#include <concepts>
#include <type_traits>
#include <utility>
#include <numeric>
#include <execution>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <variant>

/* Template: typename T, template <typename> class Container, template <typename> class Allocator, std::size_t N = null */
template<typename T>
class NPlus {

private: /* Private types */

    template<
            typename Key,
            typename Tp,
            template<typename, typename, typename, typename, typename> class Container,
            typename Allocator = std::allocator<T>,
            typename Hash = std::hash<T>,
            typename KeyEqual = std::equal_to<T>,
            typename Compare = std::less<T>
    >
    using ContainerType = Container<Tp, Allocator, Hash, KeyEqual, Compare>;


private: /* Private variables */
    static constinit auto pos_inf = std::numeric_limits<T>::infinity();
    static constinit auto neg_inf = std::minus<T>()(std::numeric_limits<T>::infinity());
    static constinit auto nan = std::numeric_limits<T>::quiet_NaN();
    static constinit auto pos_inf_ptr = std::make_optional(pos_inf);
    static constinit auto neg_inf_ptr = std::make_optional(neg_inf);
    static constinit auto nan_ptr = std::make_optional(nan);


public: /* Constructors */


private: /* Private methods */

    static auto will_overflow(T lhs, T rhs) -> bool;
    auto safe_add(T lhs, T rhs) -> T;


public: /* Public methods */

    template<
            template<typename, typename> class Container,
            template<typename> class Allocator = std::allocator,
            std::size_t N = 0>
    static constexpr Container<T, Allocator<T>> &operator()(
            std::add_const<std::add_rvalue_reference<Container<T, Allocator<T>>>> LHS,
            std::add_const<std::add_rvalue_reference<Container<T, Allocator<T>>>> RHS) {
        return std::transform(std::execution::par, LHS.begin(), LHS.end(), RHS.begin(), LHS.begin(), std::plus<T>());
    }
};

template<typename T>
auto NPlus<T>::will_overflow(T lhs, T rhs) -> bool {


    /* Check for overflow */
    if (lhs > 0 && rhs > 0 && lhs > pos_inf - rhs) {
        // if lhs + rhs > +inf, then lhs + rhs will overflow
        // or       rhs > +inf - lhs, then lhs + rhs will overflow
        return rhs > NPlus<T>::pos_inf_ptr.value() - lhs;
    }

    /* Check if a + b would underflow */
    if (lhs < 0 && rhs < 0 && lhs < neg_inf - rhs) {
        // if lhs + rhs < -inf, then lhs + rhs will overflow
        // or       rhs < -inf - lhs, then lhs + rhs will overflow
        return rhs < NPlus<T>::neg_inf_ptr.value() - lhs;
    }

    if (lhs < 0 && rhs > 0 && lhs < neg_inf + rhs) {
        // if lhs + rhs < -inf, then lhs + rhs will overflow
        // or       rhs > -inf + lhs, then lhs + rhs will overflow
        return rhs > NPlus<T>::neg_inf_ptr.value() + lhs;
    }

    if (lhs > 0 && rhs < 0 && lhs > pos_inf + rhs) {
        // if lhs + rhs > +inf, then lhs + rhs will overflow
        // or       rhs < +inf + lhs, then lhs + rhs will overflow
        return rhs < NPlus<T>::pos_inf_ptr.value() + lhs;
    }

    return false;
}

template<typename T>
auto NPlus<T>::safe_add(T lhs, T rhs) -> T {
    if (std::isinf(lhs) && std::isinf(rhs)) {
        if (lhs == rhs) {
            return lhs;
        } else {
            return nan;
        }
    } else if (std::isinf(lhs) || std::isinf(rhs)) {
        return nan;
    } else if (std::isnan(lhs) || std::isnan(rhs)) {
        return nan;
    } else {
        return lhs + rhs;
    }
}


#endif
