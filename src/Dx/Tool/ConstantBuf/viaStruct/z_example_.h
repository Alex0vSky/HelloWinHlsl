// Dx/ConstantBuf/viaStruct/z_example_.h - example, for internal using
#pragma once
namespace prj_3d::HelloWinHlsl::z_example_ {
static void structPack() {
#pragma pack( push, 1 )
	struct Aligned1 { 
		int		f0__;
		char	f1__;
		char	f1_1;
		float	f2__;
		float	f2_1;
		double	f3__;
	};
#pragma pack( pop )
	size_t ar1OffsFldAligned1[] = {
			  offsetof( Aligned1, f0__ )
			, offsetof( Aligned1, f1__ )
			, offsetof( Aligned1, f1_1 )
			, offsetof( Aligned1, f2__ )
			, offsetof( Aligned1, f2_1 )
			, offsetof( Aligned1, f3__ )
		};
	size_t ar1SizeFldAligned1[] = {
			  sizeof( Aligned1::f0__ )
			, sizeof( Aligned1::f1__ )
			, sizeof( Aligned1::f1_1 )
			, sizeof( Aligned1::f2__ )
			, sizeof( Aligned1::f2_1 )
			, sizeof( Aligned1::f3__ )
		};
	Aligned1 ar1ValueFldAligned1{ 0, 1, 2, 3, 4, 5 };

#pragma pack( push, 4 )
	struct Aligned4 { 
		int		f0__;
		char	f1__;
		char	f1_1;
		float	f2__;
		float	f2_1;
		double	f3__;
	};
#pragma pack( pop )
	size_t ar1OffsFldAligned4[] = {
			  offsetof( Aligned4, f0__ )
			, offsetof( Aligned4, f1__ )
			, offsetof( Aligned4, f1_1 )
			, offsetof( Aligned4, f2__ )
			, offsetof( Aligned4, f2_1 )
			, offsetof( Aligned4, f3__ )
		};
	size_t ar1SizeFldAligned4[] = {
			  sizeof( Aligned4::f0__ )
			, sizeof( Aligned4::f1__ )
			, sizeof( Aligned4::f1_1 )
			, sizeof( Aligned4::f2__ )
			, sizeof( Aligned4::f2_1 )
			, sizeof( Aligned4::f3__ )
		};
	Aligned4 ar1ValueFldAligned4{ 0, 1, 2, 3, 4, 5 };

	__debugbreak( );
	__nop( );
}

static void structAlignof() {
	typedef struct { int a; double b; } S;
	static_assert( alignof(S) == 8, "by the largest element in the structure, its 'double b'" );
	static_assert( sizeof( double ) == 8, "largest element in the structure" );
	__debugbreak( );
}

static void structDeclspecAlignNonDynMem() {
	struct AlignDef {
		char	f0__;
		char	f1__;
	};
	size_t ar1OffsFldAlignDef[] = {
			  offsetof( AlignDef, f0__ )
			, offsetof( AlignDef, f1__ )
		};
	size_t ar1SizeFldAlignDef[] = {
			  sizeof( AlignDef::f0__ )
			, sizeof( AlignDef::f1__ )
		};
	auto siTsAlignDef = sizeof( AlignDef );

	__declspec(align(16))
	struct Align16 {
		char	f0__;
		char	f1__;
	};
	size_t ar1OffsFldAlign16[] = {
			  offsetof( Align16, f0__ )
			, offsetof( Align16, f1__ )
		};
	static_assert( offsetof( Align16, f0__ ) == 0 );
	static_assert( offsetof( Align16, f1__ ) == 1, "eq prev sizeof( char )" );
	size_t ar1SizeFldAlign16[] = {
			  sizeof( Align16::f0__ )
			, sizeof( Align16::f1__ )
		};
	static_assert( sizeof( Align16::f0__ ) == 1 );
	static_assert( sizeof( Align16::f1__ ) == 1 );

	auto siTsAlign16 = sizeof( Align16 );
	static_assert( sizeof( Align16 ) == 16, "sizeof(char)*2 not great 16" );

	__declspec(align(16))
	struct AlignAll16 {
		__declspec(align(16)) char	f0__;
		__declspec(align(16)) char	f1__;
	};
	size_t ar1OffsFldAlignAll16[] = {
			  offsetof( AlignAll16, f0__ )
			, offsetof( AlignAll16, f1__ )
		};
	static_assert( offsetof( AlignAll16, f0__ ) == 0 );
	static_assert( offsetof( AlignAll16, f1__ ) == 16, "ne prev sizeof( char ), because of modifiers" );
	size_t ar1SizeFldAlignAll16[] = {
			  sizeof( AlignAll16::f0__ )
			, sizeof( AlignAll16::f1__ )
		};
	static_assert( sizeof( AlignAll16::f0__ ) == 1 );
	static_assert( sizeof( AlignAll16::f1__ ) == 1 );
	auto siTsAlignAll16 = sizeof( AlignAll16 );
	static_assert( sizeof( AlignAll16 ) == 16*2, "each by 16" );

	__debugbreak( );
}

#ifdef BOOST_PFR_ENABLED
template<class T>
static void structAnalize(const T &st) {
	std::vector<size_t> veSizeof;
	std::vector<std::string> veTypename;
	// Returns only values ​​and types. I don't know how to get the names in order to calculate the offsets.
	boost::pfr::for_each_field(st , [&veSizeof, &veTypename]
		(const auto& field) {
			typename std::decay<decltype( field )>::type x{};
			// MSVS get FAIL on offsetof() func
			veSizeof.push_back( sizeof( field ) );
			veTypename.push_back( typeid( field ).name( ) ); // RTTI aka /GR
			// I think it will work if a type that is not a multiple of 4 is followed by a type that is not a multiple of 4.
			// it is also possible here it is worth checking the output of the last field for 16 bytes, although it is unlikely.
		} );
	std::stringstream ss;
	auto siSize = veSizeof.size( );
	size_t i = 0;
	for( ; i < siSize; ++i )
	{
		if ( veSizeof[ i ] < 4 ) {
			ss << "[HlSh error] less 4. Field number " << i << " (count from zero), type is " << veTypename[ i ] << std::endl;
			::OutputDebugStringA( ss.str( ).c_str( ) );
			break;
		}
		//veTypename[ i ];
	}
	if ( i < siSize )
		__nop( ); // TODO: FAIL
	__nop( );
}
template<class T>
static void structAnalize() {
	structAnalize( T{} );
}
#endif // BOOST_PFR_ENABLED

} // namespace prj_3d::HelloWinHlsl::z_example_
