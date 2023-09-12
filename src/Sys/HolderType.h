// Sys/HolderType.h - helper for store template type
#pragma once
namespace Sys { 
class HolderType {
	const int m_iTypeId;
public:
	template<class T, typename = typename std::enable_if_t< std::is_base_of_v< DxVerBase, T > > >
	explicit HolderType(T unused_SFINAE_for_deduced_only) 
		: m_iTypeId( T::getId( ) )
	{}
	template<class T>
	typename std::enable_if_t< std::is_base_of_v< DxVerBase, T >, bool >
	static 
	has(const HolderType &holder) {
		if ( T::getId( ) == holder.m_iTypeId )
			return true;
		return false;
	}
};
} // namespace Sys
