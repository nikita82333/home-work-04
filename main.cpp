#include <iostream>
#include <list>
#include <vector>
#include <tuple>
#include <type_traits>

/// <summary>
/// Home work 04 - SFINAE
/// </summary>

template<class T>
using remove_cr_t = typename std::remove_const_t<std::remove_reference_t<T>>;

template <typename> struct is_tuple : std::false_type {};
template <typename ...T> struct is_tuple<std::tuple<T...>> : std::true_type {};

/// <summary>
/// Iteration by tuple.
/// </summary>
/// <typeparam name="T">Tuple type.</typeparam>
/// <typeparam name="U">Tuple element type.</typeparam>
/// <param name="Is">Ellipsis (of index).</param>
/// <returns>Vector of tuple elements.</returns>
template <typename T, typename U, std::size_t... Is>
std::vector<U> tuple_to_vector_impl(const T& tuple, std::index_sequence<Is...>) {
    std::vector<U> vector;
    auto add_to_vector = [&](const auto& elem) {
        static_assert(std::is_same_v<U, remove_cr_t<decltype(elem)>>);
        vector.push_back(elem);
    };
    (add_to_vector(std::get<Is>(tuple)), ...);
    return vector;
}

/// <summary>
/// Convert tuple to vector.
/// </summary>
/// <typeparam name="T">Tuple type.</typeparam>
/// <typeparam name="U">Tuple element type.</typeparam>
/// <param name="Size">Tuple size.</param>
/// <returns>std::vector of tuple elements.</returns>
template <typename T, typename U, std::size_t Size = std::tuple_size_v<T>>
std::vector<U> tuple_to_vector(const T& tuple) {
    return tuple_to_vector_impl<T, U>(tuple, std::make_index_sequence<Size>{});
}

/// <summary>
/// Function <c>print_ip</c>.
/// Implementation 1.
/// </summary>
/// <typeparam name="T">Container (except for the string).</typeparam>
template <typename T>
std::enable_if_t<
            std::is_same_v<decltype(begin(std::declval<T>()), end(std::declval<T>()), void()), void> &&
            not std::is_same_v<std::string, T>
>
print_ip(const T& value) {
    std::size_t index = 0;
    for (auto elem : value) {
        std::cout << elem;
        if (index++ < value.size() - 1)
            std::cout << ".";
    }
    std::cout << std::endl;
}

/// <summary>
/// Function <c>print_ip</c>.
/// Implementation 2.
/// </summary>
/// <typeparam name="T">String.</typeparam>
template <typename T>
std::enable_if_t<std::is_same_v<std::string, T>, void>
print_ip(const T& value) {
    std::cout << value << std::endl;
}

/// <summary>
/// Function <c>print_ip</c>.
/// Implementation 3.
/// </summary>
/// <typeparam name="T">Tuple.</typeparam>
template <typename T>
std::enable_if_t<is_tuple<T>{}, void>
print_ip(const T& value) {
    using elem_type = remove_cr_t<decltype(std::get<0>(value))>;
    auto ip = tuple_to_vector<T, elem_type>(value);
    print_ip(ip);
}

/// <summary>
/// Function <c>print_ip</c>.
/// Implementation 4.
/// </summary>
/// <typeparam name="T">Integer type.</typeparam>
template <typename T>
std::enable_if_t<std::is_integral_v<T>, void>
print_ip(const T& value) {
    std::list<int> ip;
    for (std::size_t i = 0; i < sizeof(value); ++i) {
        auto byte = ((std::byte*)(&value))[i];
        ip.push_front(static_cast<int>(byte));
    }
    print_ip(ip);
}

int main() {

    print_ip( int8_t{-1} ); // 255w
    print_ip( int16_t{0} ); // 0.0
    print_ip( int32_t{2130706433} ); // 127.0.0.1
    print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41
    print_ip( std::string{"Hello, World!"} ); // Hello, World!
    print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400
    print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100
    print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0


    return 0;
}