#ifndef THREADED_ADDITIONUNDERFLOWCHECK_TCC
#define THREADED_ADDITIONUNDERFLOWCHECK_TCC

#include <cmath>
#include <functional>
#include <algorithm>
#include <execution>
#include <vector>
#include <thread>
#include <hash_map>
#include <optional>
#include <limits>
#include <numeric>
#include <numbers>
#include <ranges>
#include <concepts>

template<typename N> requires std::is_arithmetic_v<N>
class AdditionUnderflowCheck {

private: /* Private Members */

    /// The positive infinity value for the given type N
    static const constinit auto
    pos_inf = std::numeric_limits<N>::infinity();

    /// The negative infinity value for the given type N
    static const constinit auto
    neg_inf = std::minus<N>()(std::numeric_limits<N>::infinity());

    /// The NaN value for the given type N
    static const constinit auto
    nan = std::numeric_limits<N>::quiet_NaN();

public: /* Public Methods */

    /// @brief Default constructor
    constexpr AdditionUnderflowCheck() noexcept = default;

    /// @brief Copy constructor
    constexpr AdditionUnderflowCheck(AdditionUnderflowCheck const &) noexcept = default;

    /// @brief Move constructor
    constexpr AdditionUnderflowCheck(AdditionUnderflowCheck &&) noexcept = default;

    /// @brief Copy assignment operator
    constexpr AdditionUnderflowCheck &operator=(AdditionUnderflowCheck const &) noexcept = default;

    /// @brief Move assignment operator
    constexpr AdditionUnderflowCheck &operator=(AdditionUnderflowCheck &&) noexcept = default;

    /// @brief Destructor
    constexpr virtual ~AdditionUnderflowCheck() noexcept = default;

    /* static scalar methods */
    constexpr static auto operator()(N lhs, N rhs) -> bool;

    /* static vector methods */
    constexpr static auto operator()(std::vector<N> const &lhs, std::vector<N> const &rhs) -> std::vector<bool>;

    constexpr static auto operator()(std::vector<N> const &lhs, N rhs) -> std::vector<bool>;

    constexpr static auto operator()(N lhs, std::vector<N> const &rhs) -> std::vector<bool>;

};



#endif
