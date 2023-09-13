// ClientApp/ABaseClientApp.h - client base class.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace prj_3d::HelloWinHlsl::ClientApp { 
#pragma warning( push )
#pragma warning( disable: 4324 )
template<class T>
class 
// This "declspec" is needed because there will probably be a member of type 
//	"XMMATRIX" inside the class that needs alignment, 
//	and there will be a "C4316" warning.
// @insp stackoverflow.com/questions/20104815/warning-c4316-object-allocated-on-the-heap-may-not-be-aligned-16 // NOLINT(whitespace/line_length)
A0S_HWH_ALIGN
ABaseClientApp {
 public:
    typedef uptr< ABaseClientApp > uptr_t;
    virtual bool init(
			typename Dx::Ctx::CtxDx<T>::cref_ptr_t
			, typename Dx::Ctx::CtxTool<T>::cref_ptr_t
			, typename Dx::AdjustAux<T>*
		) = 0;
    virtual void render_frame(
			typename Dx::Ctx::CtxDx<T>::cref_ptr_t
			, typename Dx::ClientDynamicData<T>::cref_ptr_t
		) = 0;
    virtual void present_to_screen(
			typename Dx::Ctx::CtxDx<T>::cref_ptr_t
			, typename Dx::ClientDynamicData<T>::cref_ptr_t
		) = 0;
    virtual ~ABaseClientApp() 
	 {}

	// So that the class by "new" is created aligned in memory, 
	//	and the memory is freed by "delete".
    void* operator new(size_t i) {
        return _aligned_malloc( i, 16 );
    }
    void operator delete(void* p) {
        _aligned_free( p );
    }
};
#pragma warning( pop )
} // namespace prj_3d::HelloWinHlsl::ClientApp
