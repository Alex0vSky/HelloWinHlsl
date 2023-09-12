// Dx/Tool/ConstantBuf/viaStruct/Converter.h - conversion cpp types to hlsl types
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct {
class Converter {
	template<class T>
	class ParametersPack {
		const std::vector< T > m_holder32BitValuesToSet;
	 public:
		 template<class T, size_t N>
		 explicit ParametersPack(const std::array< T, N > &value)
			 : m_Num32BitValuesToSet( N )
			 , m_holder32BitValuesToSet( value.begin( ), value.end( ) )
			 , m_pSrcData( m_holder32BitValuesToSet.data( ) )
		 {}
		 template<class TValue>
		 explicit ParametersPack(TValue value)
			 : m_Num32BitValuesToSet( 1 )
			 , m_holder32BitValuesToSet{ (T)value }
			 , m_pSrcData( m_holder32BitValuesToSet.data( ) )
		 {}

		ParametersPack(const ParametersPack &) = delete;
		ParametersPack& operator= (const ParametersPack &) = delete;
		ParametersPack(const ParametersPack &&o) 
			: m_holder32BitValuesToSet( std::move( o.m_holder32BitValuesToSet ) )
			, m_Num32BitValuesToSet( std::move( o.m_Num32BitValuesToSet ) )
			, m_pSrcData( m_holder32BitValuesToSet.data( ) )
		{}
		 
		const UINT m_Num32BitValuesToSet;
		const void *m_pSrcData;
	};

	template<class T1, class T2, class T3>
	using fullMixSequence_t = std::deque< std::variant< T1, T2, T3 > >;
	template<class T>
	using singleSeq_t = std::deque< ParametersPack< T > >;
	typedef fullMixSequence_t< 
			singleSeq_t< float >, singleSeq_t< int > , singleSeq_t< uint32_t > 
		> finalSequence_t;

	finalSequence_t m_params;

	class TypeSelector {
		finalSequence_t &m_refSequence;

		template<class T>
		class Base {
			typedef singleSeq_t< T > *ptr_SingleSequence_t;
		 protected:
			ptr_SingleSequence_t m_curSequence;
		 public:
			explicit Base(ptr_SingleSequence_t curSequence) 
				: m_curSequence( curSequence )
			{}
		};

		class MethodFloat : public Base< float > {
			using Base< float >::Base;
		 public:
			MethodFloat *float_(float value) {
				m_curSequence ->push_back( ParametersPack< float >( value ) );
				return this;
			}
			MethodFloat *float2_(std::array< float, 2 > value) {
				m_curSequence ->push_back( ParametersPack< float >( value ) );
				return this;
			}
		};

		class MethodInt : public Base< int > {
			using Base< int >::Base;
		public:
			MethodInt *int_(int value) {
				m_curSequence ->push_back( ParametersPack< int >( value ) );
				return this;
			}
		};

		class MethodUint : public Base< uint32_t > {
			using Base< uint32_t >::Base;
		public:
			MethodUint *uint_(uint32_t value) {
				m_curSequence ->push_back( ParametersPack< uint32_t >( value ) );
				return this;
			}
			MethodUint *uint2_(std::array< uint32_t, 2 > value) {
				m_curSequence ->push_back( ParametersPack< uint32_t >( value ) );
				return this;
			}
		};

		template<class T>
		singleSeq_t< T > *instance() {
			// check if last used type is T
			singleSeq_t< T > *singleSeq = nullptr;
			{
				auto itLastElem = m_refSequence.rbegin( );
				if ( m_refSequence.rend( ) != m_refSequence.rbegin( ) ) 
					if ( std::holds_alternative< singleSeq_t< T > >( *itLastElem ) ) 
						singleSeq = &std::get< singleSeq_t< T > >( *itLastElem );
			}
			// if last used type isnt T, create new
			if ( !singleSeq ) {
				m_refSequence.push_back( singleSeq_t< T >( ) );
				singleSeq = &std::get< singleSeq_t< T > >( m_refSequence.back( ) );
			}
			return singleSeq;
		}
	 public:
		explicit TypeSelector(finalSequence_t &sequence)
			: m_refSequence( sequence )
		{}

		auto likeFloat_() { 
			return std::make_shared< MethodFloat >( instance< float >( ) );
		}
		auto likeInt_() {
			return std::make_shared< MethodInt >( instance< int >( ) );
		}
		auto likeUint_() {
			return std::make_shared< MethodUint >( instance< uint32_t >( ) );
		}
	};

 public:
	Converter() 
		: wonnaPassHlslType( m_params )
	{}
	// Struct access to omit parentheses
	TypeSelector wonnaPassHlslType;

	bool send(const CPtr< ID3D12GraphicsCommandList > &pcCommandList) {
		if ( m_params.empty( ) || !pcCommandList )
			return false;
		UINT RootParameterIndex = 0;
		for ( const auto &curSeq : m_params ) {
			std::visit(
					[&pcCommandList, &RootParameterIndex](const auto& singleTypeSeq) {
						for ( const auto &parametersPack : singleTypeSeq ) {
							const UINT DestOffsetIn32BitValues = 0;
							pcCommandList ->SetGraphicsRoot32BitConstants( 
									RootParameterIndex++
									, parametersPack.m_Num32BitValuesToSet
									, parametersPack.m_pSrcData
									, DestOffsetIn32BitValues
								);
						}
					}
					, curSeq
				);
		}
		m_params.clear( );
		return true;
	}
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool::ConstantBuf::viaStruct
