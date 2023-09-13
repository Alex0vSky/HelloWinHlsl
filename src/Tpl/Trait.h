// Tpl/Trait.h - something like class traits
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::Tpl {
struct Trait { 
// For struct initialization
// @insp stackoverflow.com/questions/35298989/using-c-aggregate-initialization-in-stdmake-shared
template<class T>
struct aggregate_adapter : public T {
	template<class... Args>
	aggregate_adapter(Args&&... args) : T{ std::forward<Args>(args)... } {} // NOLINT(runtime/explicit)
};
// For struct initialization in make_unique when empty parrent
template<class T>
struct aggregate_adapter_empty_parent : public T {
	template<class... Args>
	aggregate_adapter_empty_parent(Args&&... args) // NOLINT(runtime/explicit)
		: T{ { }, std::forward<Args>(args)... } 
	{} 
};

// make unique_ptr from tuple, aggregate_adapter allow make unique_ptr from struct
// @insp stackoverflow.com/questions/74809079/how-to-make-from-tuple-on-the-heap
template<typename obj, typename tup>
static 
auto make_unique_struct_from_tuple(tup&& t) {
    return std::apply(
        [] (auto&& ... args) {
            return std::make_unique< Trait::aggregate_adapter< obj > >
                (std::forward<decltype(args)>(args)...);
        },
        std::forward<tup>(t)
    );
}
template<typename obj, typename tup>
static 
auto make_unique_class_from_tuple(tup&& t) {
    return std::apply(
        [] (auto&& ... args) {
            return std::make_unique< obj >
                (std::forward<decltype(args)>(args)...);
        },
        std::forward<tup>(t)
    );
}

// For empty std deleter
struct nop { template <typename T> void operator() (T const &) const noexcept {} };

// For "if constexpr ... else static_assert()"
template<class> inline static constexpr bool always_false_v = false;

// @insp stackoverflow.com/questions/17254425/getting-the-size-in-bytes-of-a-vector
template<class T> static size_t vectorsizeof(const typename std::vector< T >& vec) { 
	return sizeof( T ) * vec.size( ); 
}
};
} // namespace prj_3d::HelloWinHlsl::Tpl
