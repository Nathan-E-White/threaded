#include <iostream>
#include <thread>
#include <execution>
#include <numbers>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <cmath>
#include <limits>
#include <concepts>
#include <ranges>
#include <functional>
#include <random>
#include <iomanip>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <span>
#include <optional>
#include <variant>
#include <any>
#include <typeindex>
#include <condition_variable>
#include <coroutine>
#include <bitset>
#include <map>


/* Concept for a data structure that can be used as a container for a graph. */
template<typename T>
concept DataStructureQ = requires(T t)
{
    typename T::value_type;
    typename T::size_type;
};


template<typename T>
class AbstractValue {

public:
    virtual T get() = 0;

    virtual void set(T t) = 0;
};


template<typename T, std::memory_order MO>
class Value : public AbstractValue<T> {

    std::atomic<T> value{};

    // we want to put in a little bit of effort to make sure that the value is not
    // modified by another thread while we are reading it

    // make sure the mutex is on the next memory location that is a multiple of 64
    // bytes
    std::mutex mutex{};

    // we also want all STL iterators to be constant time, so we need to make sure
    // that the mutex is not in the same cache line as the value


public:
    constexpr explicit Value(T t) : value(t) {}

    constexpr T get() override {
        std::lock_guard<std::mutex> lock(mutex);
        return value.load(MO);

    }

    void set(T t) override {
        std::lock_guard<std::mutex> lock(mutex);
        value.store(t, MO);
    }
};


/* Concept for a threaded data structure */
template<typename T>
concept ThreadableDataStructure = requires(T t) {
    { t.size() } -> std::same_as<std::size_t>;
    { t[0] } -> std::same_as<int>;
};


/* Concept for an execution policy */
template<typename T>
concept ExecutionPolicy = requires(T t) {
    requires std::is_execution_policy_v<T>;
};

enum class ATC_Codes : std::uint8_t {
    Generic_Operation_Success = 0,
    Generic_Operation_Failure = 1,

};


template<template<typename...> class T, typename... Args>
class TemplateFunction {
private:
    std::function<T<Args...>()> m_function = nullptr;

public:
    /* Standard constructor */
    constexpr explicit TemplateFunction(std::function<T<Args...>()> function) : m_function(function) {}

    /* Default No-Op constructor */
    constexpr TemplateFunction() noexcept = default;

    /* Default copy constructor */
    constexpr TemplateFunction(TemplateFunction const &) noexcept = default;

    /* Default move constructor */
    constexpr TemplateFunction(TemplateFunction &&) noexcept = default;

    /* Default copy assignment operator */
    constexpr TemplateFunction &operator=(TemplateFunction const &) noexcept = default;

    /* Default move assignment operator */
    constexpr TemplateFunction &operator=(TemplateFunction &&) noexcept = default;

    /* Default destructor */
    virtual ~TemplateFunction() noexcept = default;

    /* Call operator */
    constexpr T<Args...> operator()(Args... args) {
        return m_function(args...);
    }

    /* Call operator (const) */
    constexpr T<Args...> operator()(Args... args) const {
        return m_function(args...);
    }


    constexpr auto get_function() const -> std::add_cv_t<std::reference_wrapper<std::function<T<Args...>()>>> {
        return std::ref(m_function);
    }

    constexpr auto set_function(std::add_cv_t<std::reference_wrapper<std::function<T<Args...>()>>> f) -> void {
        m_function = f;
    }


};


template<std::size_t NumThreads>
class AbstractThreadedClass {

private:
    std::array<std::thread, NumThreads> threads = {};
    std::mutex mtx = {};
    std::condition_variable cv{};

    std::atomic_flag lock = ATOMIC_FLAG_INIT;
    std::atomic_flag done = ATOMIC_FLAG_INIT;
    std::atomic_flag ready = ATOMIC_FLAG_INIT;

    std::atomic<std::bitset<NumThreads>> ready_threads = 0;
    std::atomic<std::bitset<NumThreads>> done_threads = 0;
    std::atomic<std::bitset<NumThreads>> lock_threads = 0;
    std::atomic<std::bitset<NumThreads>> unlock_threads = 0;
    std::atomic<std::bitset<NumThreads>> wait_threads = 0;
    std::atomic<std::bitset<NumThreads>> notify_threads = 0;

    std::function<void()> init_func = nullptr;
    std::function<void()> work_func = nullptr;
    std::function<void()> cleanup_func = nullptr;

public:


protected:
    AbstractThreadedClass();

protected:
    constexpr AbstractThreadedClass(AbstractThreadedClass const &);

protected:
    constexpr AbstractThreadedClass(AbstractThreadedClass &&);

protected:
    constexpr AbstractThreadedClass &operator=(AbstractThreadedClass const &);

protected:
    constexpr AbstractThreadedClass &operator=(AbstractThreadedClass &&);

public:
    virtual ~AbstractThreadedClass();

    template<ThreadableDataStructure T>
    void threadFunction(T &data, std::size_t start, std::size_t end, std::size_t threadNum);


    void set_init_func(std::function<void()> func);

    void set_work_func(std::function<void()> func);

    void set_cleanup_func(std::function<void()> func);

    void set_mutex(std::mutex &mtx);

    void set_condition_variable(std::condition_variable &cv);

    void toggle_global_lock(std::atomic_flag &lock);

    void toggle_global_lock();

    void toggle_global_done(std::atomic_flag &done);

    void toggle_global_done();

    void toggle_global_ready(std::atomic_flag &ready);

    auto toggle_global_ready() -> std::atomic_flag &;

    void toggle_thread_lock(std::atomic<std::bitset<NumThreads>> &lock_threads, std::size_t threadNum);

    void toggle_thread_done(std::atomic<std::bitset<NumThreads>> &done_threads, std::size_t threadNum);

    void toggle_thread_ready(std::atomic<std::bitset<NumThreads>> &ready_threads, std::size_t threadNum);

    void toggle_thread_wait(std::atomic<std::bitset<NumThreads>> &wait_threads, std::size_t threadNum);

    void toggle_thread_notify(std::atomic<std::bitset<NumThreads>> &notify_threads, std::size_t threadNum);

    void toggle_thread_unlock(std::atomic<std::bitset<NumThreads>> &unlock_threads, std::size_t threadNum);

    void wait_for_threads(std::atomic<std::bitset<NumThreads>> &threads, std::size_t threadNum);


    std::function<void()> get_init_func();

    std::function<void()> get_work_func();

    std::function<void()> get_cleanup_func();


    void run();
};

template<
        std::size_t NumThreads,
        typename T,
        typename ThreadableDataStructure,
        /* Execution Policy: Default--Parallel Out of Order */
        ExecutionPolicy auto Exec = std::execution::parallel_unsequenced_policy(),
        /* Require that the data structure is a ThreadableDataStructure over type T */
        typename = std::enable_if_t<std::is_base_of_v<AbstractThreadedClass<NumThreads>, T>>
>
class ThreadedClass : public AbstractThreadedClass<NumThreads> {

private:
    ThreadableDataStructure &data;
    T &func;

public:
    constexpr ThreadedClass(ThreadableDataStructure &data, T &func) : data(data), func(func) {}

    void operator()() {
        std::for_each(Exec, data.begin(), data.end(), func);
    }
};


template<std::size_t Base = 2>
class ArithmeticRadix {

};


template<std::size_t Base = 2>
class ArithmeticMantissa {

};

template<typename T>
class ArithmeticExponent {

};


struct Enum2 {

    int m_value = 0x0;

public:
    // we are trying to emulate an enum here
    // so we need to be able to convert to and from int
    constexpr explicit operator int() const noexcept { return m_value; }

    constexpr Enum2() noexcept = default;


    // need a way to convert from int to Enum2

    constexpr explicit Enum2(int value) noexcept: m_value(value) {
        // check that the value is valid
        // if not, throw an exception
        this->m_value = value;
    }

    constexpr Enum2(Enum2 const &) noexcept = default;

    constexpr Enum2(Enum2 &&) noexcept = default;

    constexpr Enum2 &operator=(Enum2 const &) noexcept = default;

    constexpr Enum2 &operator=(Enum2 &&) noexcept = default;


    // we also need to be able to compare to other MyEnum values
    constexpr bool operator==(Enum2 other) const noexcept { return m_value == other.m_value; }

    constexpr bool operator!=(Enum2 other) const noexcept { return m_value != other.m_value; }

    constexpr bool operator<(Enum2 other) const noexcept { return m_value < other.m_value; }

    constexpr bool operator>(Enum2 other) const noexcept { return m_value > other.m_value; }

    constexpr bool operator<=(Enum2 other) const noexcept { return m_value <= other.m_value; }

    constexpr bool operator>=(Enum2 other) const noexcept { return m_value >= other.m_value; }

    // we also need to be able to compare to int values
    constexpr bool operator==(int other) const noexcept { return m_value == other; }

    constexpr bool operator!=(int other) const noexcept { return m_value != other; }

    constexpr bool operator<(int other) const noexcept { return m_value < other; }

    constexpr bool operator>(int other) const noexcept { return m_value > other; }

    constexpr bool operator<=(int other) const noexcept { return m_value <= other; }

    constexpr bool operator>=(int other) const noexcept { return m_value >= other; }

    // we also need to be able to assign from int values
    constexpr Enum2 &operator=(int other) noexcept {
        m_value = other;
        return *this;
    }

    // we also need to be able to add and subtract int values
    constexpr Enum2 &operator+=(int other) noexcept {
        m_value += other;
        return *this;
    }

    constexpr Enum2 &operator-=(int other) noexcept {
        m_value -= other;
        return *this;
    }

    constexpr Enum2 operator+(int other) const noexcept { return Enum2(m_value + other); }

    constexpr Enum2 operator-(int other) const noexcept { return Enum2(m_value - other); }

    // need named constructors for the values we want to use
    static constexpr Enum2 ZERO() { return Enum2{0}; }

    static constexpr Enum2 ONE() { return Enum2{1}; }
};

class Enum3 : protected Enum2 {

public:

    constexpr explicit Enum3(int i) : Enum2(i) {
        if (i < 0 || i > 2) {
            throw std::invalid_argument("Enum3 must be between 0 and 2");
        }

        this->m_value = i;
    }

    static constexpr Enum3 TWO() { return Enum3(2); }
};

class Enum4 : protected Enum3 {
public:
    constexpr explicit Enum4(int i) : Enum3(i) {
        if (i < 0 || i > 3) {
            throw std::invalid_argument("Enum4 must be between 0 and 3");
        }

        this->m_value = i;
    }

    static constexpr Enum4 THREE() { return Enum4(3); }
};

class Enum5 : protected Enum4 {
public:
    constexpr explicit Enum5(int i) : Enum4(i) {
        if (i < 0 || i > 4) {
            throw std::invalid_argument("Enum5 must be between 0 and 4");
        }

        this->m_value = i;
    }

    static constexpr Enum5 FOUR() { return Enum5(4); }
};

class Enum6 : protected Enum5 {
public:
    constexpr explicit Enum6(int i) : Enum5(i) {
        if (i < 0 || i > 5) {
            throw std::invalid_argument("Enum6 must be between 0 and 5");
        }

        this->m_value = i;
    }

    static constexpr Enum6 FIVE() { return Enum6(5); }
};

class Enum7 : protected Enum6 {
public:
    constexpr explicit Enum7(int i) : Enum6(i) {
        if (i < 0 || i > 6) {
            throw std::invalid_argument("Enum7 must be between 0 and 6");
        }

        this->m_value = i;
    }

    static constexpr Enum7 SIX() { return Enum7(6); }
};

class Enum8 : protected Enum7 {
public:
    constexpr explicit Enum8(int i) : Enum7(i) {
        if (i < 0 || i > 7) {
            throw std::invalid_argument("Enum8 must be between 0 and 7");
        }

        this->m_value = i;
    }

    static constexpr Enum8 SEVEN() { return Enum8(7); }
};

class Enum9 : protected Enum8 {
public:
    constexpr explicit Enum9(int i) : Enum8(i) {
        if (i < 0 || i > 8) {
            throw std::invalid_argument("Enum9 must be between 0 and 8");
        }

        this->m_value = i;
    }

    static constexpr Enum9 EIGHT() { return Enum9(8); }
};

class Enum10 : protected Enum9 {
public:
    constexpr explicit Enum10(int i) : Enum9(i) {
        if (i < 0 || i > 9) {
            throw std::invalid_argument("Enum10 must be between 0 and 9");
        }

        this->m_value = i;
    }

    static constexpr Enum10 NINE() { return Enum10(9); }
};

class Enum11 : protected Enum10 {
public:
    constexpr explicit Enum11(int i) : Enum10(i) {
        if (i < 0 || i > 10) {
            throw std::invalid_argument("Enum11 must be between 0 and 10");
        }

        this->m_value = i;
    }

    static constexpr Enum11 TEN() { return Enum11(10); }
};

class Enum12 : protected Enum11 {
public:
    constexpr explicit Enum12(int i) : Enum11(i) {
        if (i < 0 || i > 11) {
            throw std::invalid_argument("Enum12 must be between 0 and 11");
        }

        this->m_value = i;
    }

    static constexpr Enum12 ELEVEN() { return Enum12(11); }
};

class Enum13 : protected Enum12 {
public:
    constexpr explicit Enum13(int i) : Enum12(i) {
        if (i < 0 || i > 12) {
            throw std::invalid_argument("Enum13 must be between 0 and 12");
        }

        this->m_value = i;
    }

    static constexpr Enum13 TWELVE() { return Enum13(12); }
};

class Enum14 : protected Enum13 {
public:
    constexpr explicit Enum14(int i) : Enum13(i) {
        if (i < 0 || i > 13) {
            throw std::invalid_argument("Enum14 must be between 0 and 13");
        }

        this->m_value = i;
    }

    static constexpr Enum14 THIRTEEN() { return Enum14(13); }
};

class Enum15 : protected Enum14 {
public:
    constexpr explicit Enum15(int i) : Enum14(i) {
        if (i < 0 || i > 14) {
            throw std::invalid_argument("Enum15 must be between 0 and 14");
        }

        this->m_value = i;
    }

    static constexpr Enum15 FOURTEEN() { return Enum15(14); }
};

class Enum16 : protected Enum15 {
public:
    constexpr explicit Enum16(int i) : Enum15(i) {
        if (i < 0 || i > 15) {
            throw std::invalid_argument("Enum16 must be between 0 and 15");
        }

        this->m_value = i;
    }

    static constexpr Enum16 FIFTEEN() { return Enum16(15); }
};


template<typename T>
class RadixData {
#ifndef ND
#define ND [[nodiscard]]
#endif
#ifndef MU
#define MU [[maybe_unused]]
#endif
private:

    T m_data = 0;
    std::size_t m_radix = 0;
    std::size_t m_mantissa = 0;
    std::size_t m_exponent = 0;

public:

    enum class Radix2 : T {
        ZERO MU, ONE MU
    };

    /* Implement a ternary enum */
    /* use Radix2 as a starting point */
    enum class Radix3 : T {
        ZERO MU,
        ONE MU,
        TWO MU
    };

    /* Implement a quaternary enum */
    /* use Radix3 as a starting point */
    enum class Radix4 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU
    };

    /* Implement a quinary enum */
    /* use Radix4 as a starting point */
    enum class Radix5 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU
    };

    /* Implement a senary enum */
    /* use Radix5 as a starting point */
    enum class Radix6 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU
    };

    /* Implement a septenary enum */
    /* use Radix6 as a starting point */
    enum class Radix7 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU
    };

    /* Implement an octal enum */
    /* use Radix7 as a starting point */
    enum class Radix8 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU
    };

    /* Implement a nonary enum */
    /* use Radix8 as a starting point */
    enum class Radix9 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU
    };

    /* Implement a decimal enum */
    /* use Radix9 as a starting point */
    enum class Radix10 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU,
        NINE MU
    };

    /* Implement an undecimal enum */
    /* use Radix10 as a starting point */
    enum class Radix11 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU,
        NINE MU,
        TEN MU
    };

    /* Implement an duodecimal enum */
    /* use Radix11 as a starting point */
    enum class Radix12 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU,
        NINE MU,
        TEN MU,
        ELEVEN MU
    };

    /* Implement a tridecimal enum */
    /* use Radix12 as a starting point */
    enum class Radix13 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU,
        NINE MU,
        TEN MU,
        ELEVEN MU,
        TWELVE MU
    };

    /* Implement a tetradecimal enum */
    /* use Radix13 as a starting point */
    enum class Radix14 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU,
        NINE MU,
        TEN MU,
        ELEVEN MU,
        TWELVE MU,
        THIRTEEN MU
    };

    /* Implement a pentadecimal enum */
    /* use Radix14 as a starting point */
    enum class Radix15 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU,
        NINE MU,
        TEN MU,
        ELEVEN MU,
        TWELVE MU,
        THIRTEEN MU,
        FOURTEEN MU
    };

    /* Implement a hexadecimal enum */
    /* use Radix15 as a starting point */
    enum class Radix16 : T {
        ZERO MU,
        ONE MU,
        TWO MU,
        THREE MU,
        FOUR MU,
        FIVE MU,
        SIX MU,
        SEVEN MU,
        EIGHT MU,
        NINE MU,
        TEN MU,
        ELEVEN MU,
        TWELVE MU,
        THIRTEEN MU,
        FOURTEEN MU,
        FIFTEEN MU
    };

#undef ND
#undef MU
};


using SyntheticRadix = std::variant<
        RadixData<std::size_t>::Radix2,
        RadixData<std::size_t>::Radix3,
        RadixData<std::size_t>::Radix4,
        RadixData<std::size_t>::Radix5,
        RadixData<std::size_t>::Radix6,
        RadixData<std::size_t>::Radix7,
        RadixData<std::size_t>::Radix8,
        RadixData<std::size_t>::Radix9,
        RadixData<std::size_t>::Radix10,
        RadixData<std::size_t>::Radix11,
        RadixData<std::size_t>::Radix12,
        RadixData<std::size_t>::Radix13,
        RadixData<std::size_t>::Radix14,
        RadixData<std::size_t>::Radix15,
        RadixData<std::size_t>::Radix16
>;


using rd = RadixData<std::size_t>;
using r2 = rd::Radix2;
using r3 = rd::Radix3;
using r4 = rd::Radix4;
using r5 = rd::Radix5;
using r6 = rd::Radix6;
using r7 = rd::Radix7;
using r8 = rd::Radix8;
using r9 = rd::Radix9;
using r10 = rd::Radix10;
using r11 = rd::Radix11;
using r12 = rd::Radix12;
using r13 = rd::Radix13;
using r14 = rd::Radix14;
using r15 = rd::Radix15;
using r16 = rd::Radix16;


std::vector<std::map<std::pair<std::size_t, std::size_t>, std::vector<SyntheticRadix>>> synthetic_radixes = {

        /* Radix Two */
        {
                {{2, 0}, {r2::ZERO}},
                {{3, 0}, {r3::ZERO}},
                {{4, 0}, {r4::ZERO}},
                {{5, 0}, {r5::ZERO}},
                {{6, 0}, {r6::ZERO}},
                {{7, 0}, {r7::ZERO}},
                {{8, 0}, {r8::ZERO}},
                {{9, 0}, {r9::ZERO}},
                {{10, 0}, {r10::ZERO}},
                {{11, 0}, {r11::ZERO}},
                {{12, 0}, {r12::ZERO}},
                {{13, 0}, {r13::ZERO}},
                {{14, 0}, {r14::ZERO}},
                {{15, 0}, {r15::ZERO}},
                {{16, 0}, {r16::ZERO}},

                {{2, 1}, {r2::ONE}},
                {{3, 1}, {r3::ONE}},
                {{4, 1}, {r4::ONE}},
                {{5, 1}, {r5::ONE}},
                {{6, 1}, {r6::ONE}},
                {{7, 1}, {r7::ONE}},
                {{8, 1}, {r8::ONE}},
                {{9, 1}, {r9::ONE}},
                {{10, 1}, {r10::ONE}},
                {{11, 1}, {r11::ONE}},
                {{12, 1}, {r12::ONE}},
                {{13, 1}, {r13::ONE}},
                {{14, 1}, {r14::ONE}},
                {{15, 1}, {r15::ONE}},
                {{16, 1}, {r16::ONE}},

                {{2, 2}, {r2::ONE, r2::ZERO}},
                {{3, 2}, {r3::ZERO, r3::TWO}},
                {{4, 2}, {r4::ZERO, r4::TWO}},
                {{5, 2}, {r5::ZERO, r5::TWO}},
                {{6, 2}, {r6::ZERO, r6::TWO}},
                {{7, 2}, {r7::ZERO, r7::TWO}},
                {{8, 2}, {r8::ZERO, r8::TWO}},
                {{9, 2}, {r9::ZERO, r9::TWO}},
                {{10, 2}, {r10::ZERO, r10::TWO}},
                {{11, 2}, {r11::ZERO, r11::TWO}},
                {{12, 2}, {r12::ZERO, r12::TWO}},
                {{13, 2}, {r13::ZERO, r13::TWO}},
                {{14, 2}, {r14::ZERO, r14::TWO}},
                {{15, 2}, {r15::ZERO, r15::TWO}},
                {{16, 2}, {r16::ZERO, r16::TWO}}
        }
};

// pair (radix, value_to_convert)
std::vector<std::map<std::pair<std::size_t, std::size_t>,
        SyntheticRadix>> radix_map = {

        // radix 2
        {
                {{2,  0}, RadixData<std::size_t>::Radix2::ZERO},
                {{2,  1}, RadixData<std::size_t>::Radix2::ONE}
        },

        // radix 3
        {
                {{3,  0}, RadixData<std::size_t>::Radix3::ZERO},
                {{3,  1}, RadixData<std::size_t>::Radix3::ONE},
                {{3,  2}, RadixData<std::size_t>::Radix3::TWO}
        },

        // radix 4
        {
                {{4,  0}, RadixData<std::size_t>::Radix4::ZERO},
                {{4,  1}, RadixData<std::size_t>::Radix4::ONE},
                {{4,  2}, RadixData<std::size_t>::Radix4::TWO},
                {{4,  3}, RadixData<std::size_t>::Radix4::THREE}
        },

        // radix 5
        {
                {{5,  0}, RadixData<std::size_t>::Radix5::ZERO},
                {{5,  1}, RadixData<std::size_t>::Radix5::ONE},
                {{5,  2}, RadixData<std::size_t>::Radix5::TWO},
                {{5,  3}, RadixData<std::size_t>::Radix5::THREE},
                {{5,  4}, RadixData<std::size_t>::Radix5::FOUR}
        },

        // radix 6
        {
                {{6,  0}, RadixData<std::size_t>::Radix6::ZERO},
                {{6,  1}, RadixData<std::size_t>::Radix6::ONE},
                {{6,  2}, RadixData<std::size_t>::Radix6::TWO},
                {{6,  3}, RadixData<std::size_t>::Radix6::THREE},
                {{6,  4}, RadixData<std::size_t>::Radix6::FOUR},
                {{6,  5}, RadixData<std::size_t>::Radix6::FIVE}
        },

        // radix 7
        {
                {{7,  0}, RadixData<std::size_t>::Radix7::ZERO},
                {{7,  1}, RadixData<std::size_t>::Radix7::ONE},
                {{7,  2}, RadixData<std::size_t>::Radix7::TWO},
                {{7,  3}, RadixData<std::size_t>::Radix7::THREE},
                {{7,  4}, RadixData<std::size_t>::Radix7::FOUR},
                {{7,  5}, RadixData<std::size_t>::Radix7::FIVE},
                {{7,  6}, RadixData<std::size_t>::Radix7::SIX}
        },

        // radix 8
        {
                {{8,  0}, RadixData<std::size_t>::Radix8::ZERO},
                {{8,  1}, RadixData<std::size_t>::Radix8::ONE},
                {{8,  2}, RadixData<std::size_t>::Radix8::TWO},
                {{8,  3}, RadixData<std::size_t>::Radix8::THREE},
                {{8,  4}, RadixData<std::size_t>::Radix8::FOUR},
                {{8,  5}, RadixData<std::size_t>::Radix8::FIVE},
                {{8,  6}, RadixData<std::size_t>::Radix8::SIX},
                {{8,  7}, RadixData<std::size_t>::Radix8::SEVEN}
        },

        // radix 9
        {
                {{9,  0}, RadixData<std::size_t>::Radix9::ZERO},
                {{9,  1}, RadixData<std::size_t>::Radix9::ONE},
                {{9,  2}, RadixData<std::size_t>::Radix9::TWO},
                {{9,  3}, RadixData<std::size_t>::Radix9::THREE},
                {{9,  4}, RadixData<std::size_t>::Radix9::FOUR},
                {{9,  5}, RadixData<std::size_t>::Radix9::FIVE},
                {{9,  6}, RadixData<std::size_t>::Radix9::SIX},
                {{9,  7}, RadixData<std::size_t>::Radix9::SEVEN},
                {{9,  8}, RadixData<std::size_t>::Radix9::EIGHT}
        },

        // radix 10
        {
                {{10, 0}, RadixData<std::size_t>::Radix10::ZERO},
                {{10, 1}, RadixData<std::size_t>::Radix10::ONE},
                {{10, 2}, RadixData<std::size_t>::Radix10::TWO},
                {{10, 3}, RadixData<std::size_t>::Radix10::THREE},
                {{10, 4}, RadixData<std::size_t>::Radix10::FOUR},
                {{10, 5}, RadixData<std::size_t>::Radix10::FIVE},
                {{10, 6}, RadixData<std::size_t>::Radix10::SIX},
                {{10, 7}, RadixData<std::size_t>::Radix10::SEVEN},
                {{10, 8}, RadixData<std::size_t>::Radix10::EIGHT},
                {{10, 9}, RadixData<std::size_t>::Radix10::NINE}
        },

        // radix 11
        {
                {{11, 0}, RadixData<std::size_t>::Radix11::ZERO},
                {{11, 1}, RadixData<std::size_t>::Radix11::ONE},
                {{11, 2}, RadixData<std::size_t>::Radix11::TWO},
                {{11, 3}, RadixData<std::size_t>::Radix11::THREE},
                {{11, 4}, RadixData<std::size_t>::Radix11::FOUR},
                {{11, 5}, RadixData<std::size_t>::Radix11::FIVE},
                {{11, 6}, RadixData<std::size_t>::Radix11::SIX},
                {{11, 7}, RadixData<std::size_t>::Radix11::SEVEN},
                {{11, 8}, RadixData<std::size_t>::Radix11::EIGHT},
                {{11, 9}, RadixData<std::size_t>::Radix11::NINE},
                {{11, 10}, RadixData<std::size_t>::Radix11::TEN}
        },

        // radix 12
        {
                {{12, 0}, RadixData<std::size_t>::Radix12::ZERO},
                {{12, 1}, RadixData<std::size_t>::Radix12::ONE},
                {{12, 2}, RadixData<std::size_t>::Radix12::TWO},
                {{12, 3}, RadixData<std::size_t>::Radix12::THREE},
                {{12, 4}, RadixData<std::size_t>::Radix12::FOUR},
                {{12, 5}, RadixData<std::size_t>::Radix12::FIVE},
                {{12, 6}, RadixData<std::size_t>::Radix12::SIX},
                {{12, 7}, RadixData<std::size_t>::Radix12::SEVEN},
                {{12, 8}, RadixData<std::size_t>::Radix12::EIGHT},
                {{12, 9}, RadixData<std::size_t>::Radix12::NINE},
                {{12, 10}, RadixData<std::size_t>::Radix12::TEN},
                {{12, 11}, RadixData<std::size_t>::Radix12::ELEVEN}
        },

        // radix 13
        {
                {{13, 0}, RadixData<std::size_t>::Radix13::ZERO},
                {{13, 1}, RadixData<std::size_t>::Radix13::ONE},
                {{13, 2}, RadixData<std::size_t>::Radix13::TWO},
                {{13, 3}, RadixData<std::size_t>::Radix13::THREE},
                {{13, 4}, RadixData<std::size_t>::Radix13::FOUR},
                {{13, 5}, RadixData<std::size_t>::Radix13::FIVE},
                {{13, 6}, RadixData<std::size_t>::Radix13::SIX},
                {{13, 7}, RadixData<std::size_t>::Radix13::SEVEN},
                {{13, 8}, RadixData<std::size_t>::Radix13::EIGHT},
                {{13, 9}, RadixData<std::size_t>::Radix13::NINE},
                {{13, 10}, RadixData<std::size_t>::Radix13::TEN},
                {{13, 11}, RadixData<std::size_t>::Radix13::ELEVEN},
                {{13, 12}, RadixData<std::size_t>::Radix13::TWELVE}
        },

        // radix 14
        {
                {{14, 0}, RadixData<std::size_t>::Radix14::ZERO},
                {{14, 1}, RadixData<std::size_t>::Radix14::ONE},
                {{14, 2}, RadixData<std::size_t>::Radix14::TWO},
                {{14, 3}, RadixData<std::size_t>::Radix14::THREE},
                {{14, 4}, RadixData<std::size_t>::Radix14::FOUR},
                {{14, 5}, RadixData<std::size_t>::Radix14::FIVE},
                {{14, 6}, RadixData<std::size_t>::Radix14::SIX},
                {{14, 7}, RadixData<std::size_t>::Radix14::SEVEN},
                {{14, 8}, RadixData<std::size_t>::Radix14::EIGHT},
                {{14, 9}, RadixData<std::size_t>::Radix14::NINE},
                {{14, 10}, RadixData<std::size_t>::Radix14::TEN},
                {{14, 11}, RadixData<std::size_t>::Radix14::ELEVEN},
                {{14, 12}, RadixData<std::size_t>::Radix14::TWELVE},
                {{14, 13}, RadixData<std::size_t>::Radix14::THIRTEEN}
        },

        // radix 15
        {
                {{15, 0}, RadixData<std::size_t>::Radix15::ZERO},
                {{15, 1}, RadixData<std::size_t>::Radix15::ONE},
                {{15, 2}, RadixData<std::size_t>::Radix15::TWO},
                {{15, 3}, RadixData<std::size_t>::Radix15::THREE},
                {{15, 4}, RadixData<std::size_t>::Radix15::FOUR},
                {{15, 5}, RadixData<std::size_t>::Radix15::FIVE},
                {{15, 6}, RadixData<std::size_t>::Radix15::SIX},
                {{15, 7}, RadixData<std::size_t>::Radix15::SEVEN},
                {{15, 8}, RadixData<std::size_t>::Radix15::EIGHT},
                {{15, 9}, RadixData<std::size_t>::Radix15::NINE},
                {{15, 10}, RadixData<std::size_t>::Radix15::TEN},
                {{15, 11}, RadixData<std::size_t>::Radix15::ELEVEN},
                {{15, 12}, RadixData<std::size_t>::Radix15::TWELVE},
                {{15, 13}, RadixData<std::size_t>::Radix15::THIRTEEN},
                {{15, 14}, RadixData<std::size_t>::Radix15::FOURTEEN}
        },

        // radix 16
        {
                {{16, 0}, RadixData<std::size_t>::Radix16::ZERO},
                {{16, 1}, RadixData<std::size_t>::Radix16::ONE},
                {{16, 2}, RadixData<std::size_t>::Radix16::TWO},
                {{16, 3}, RadixData<std::size_t>::Radix16::THREE},
                {{16, 4}, RadixData<std::size_t>::Radix16::FOUR},
                {{16, 5}, RadixData<std::size_t>::Radix16::FIVE},
                {{16, 6}, RadixData<std::size_t>::Radix16::SIX},
                {{16, 7}, RadixData<std::size_t>::Radix16::SEVEN},
                {{16, 8}, RadixData<std::size_t>::Radix16::EIGHT},
                {{16, 9}, RadixData<std::size_t>::Radix16::NINE},
                {{16, 10}, RadixData<std::size_t>::Radix16::TEN},
                {{16, 11}, RadixData<std::size_t>::Radix16::ELEVEN},
                {{16, 12}, RadixData<std::size_t>::Radix16::TWELVE},
                {{16, 13}, RadixData<std::size_t>::Radix16::THIRTEEN},
                {{16, 14}, RadixData<std::size_t>::Radix16::FOURTEEN},
                {{16, 15}, RadixData<std::size_t>::Radix16::FIFTEEN}
        },
};


template<typename T>
constexpr auto change_radix(T value, std::size_t src_rad, std::size_t dst_rad) {

    std::vector<T> digs_src;
    std::vector<std::vector<bool>> digs_bin;
    std::vector<std::vector<SyntheticRadix>> digs_dst;

    /* Lambda to convert a value to a vector of digits in a given radix */
    auto to_radix = [](T value, std::size_t radix) {
        std::vector<T> digs;
        while (value > 0) {
            digs.push_back(value % radix);
            value /= radix;
        }
        return digs;
    };

    auto shift_peel = [](T val, std::size_t num_shifts) -> T {

        constexpr const std::size_t peel_mask = 0b1;
        std::shift_right(val, num_shifts);

        return val & peel_mask;

    };

    /* Lambda to convert a value to a vector of digits in binary */
    auto to_binary = [](T value) {
        std::vector<std::vector<bool>> digs;

        while (value > 0) {
            digs.push_back(std::vector<bool>{shift_peel(value, 0), shift_peel(value, 1), shift_peel(value, 2),
                                             shift_peel(value, 3)});
            value >>= 4;
        }

        return digs;
    };

}


template<
        typename T, std::size_t N
>
class Numeric {
private:

    enum class bit_pos {
        SIGN_BIT,
        ZERO_BIT,
        RADIX_LSB,
        RADIX_MSB,
        EXPONENT_LSB,
        EXPONENT_MSB,
        MANTISSA_LSB,
        MANTISSA_MSB,
        NUM_BITS
    };

};


int main() {

    std::any a = 5;
    std::cout << std::any_cast<int>(a) << std::endl;


    std::cout << "Hello, World!" << std::endl;
    return 0;
}
