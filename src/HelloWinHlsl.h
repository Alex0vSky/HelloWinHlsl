// HelloWinHlsl.h - the main include file for the client to use this library.
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#include "_pch\stdafx.h" // NOLINT(build/include_subdir)


#pragma comment( lib, "dxgi" )
// toDrawFps
#pragma comment( lib, "d2d1" )
#pragma comment( lib, "dwrite" )
// DirectX 9
#pragma comment( lib, "d3d9" )
#pragma comment( lib, "d3dx9" )
// DirectX 10
#pragma comment( lib, "d3d10_1" )
#pragma comment( lib, "d3dx10" )
// DirectX 11
#pragma comment( lib, "d3d11" )
#pragma comment( lib, "d3dcompiler" )
// DirectX 12
#pragma comment( lib, "d3d12" )
#pragma comment( lib, "d3dcompiler" )


#include "..\resource\resource.h" // NOLINT(build/include_subdir)
#ifndef A0S_HWH_YOUROWN_ENTRY
// Avoid error LNK2019: unresolved external symbol _WinMain@16
//  due conflict entry point from static lib
//	if you include PCH (stdaxf.h), then the library cannot be transferred to 
//	the client via pragma, but must be directly in the options.
#pragma comment( linker, "/ENTRY:\"wWinMainCRTStartup\"" )
#endif // A0S_HWH_YOUROWN_ENTRY


template<class T> class CPtr : public Microsoft::WRL::ComPtr<T> {
	using Microsoft::WRL::ComPtr<T>::ComPtr;
	// Remove danger operator
    Microsoft::WRL::Details::ComPtrRef< Microsoft::WRL::ComPtr<T> > operator&() // NOLINT(runtime/operator)
#ifndef A0S_cppcheck__
		throw( ) 
#endif // A0S_cppcheck__
		= delete;
};
template<class T> using uptr = std::unique_ptr<T>;
template<class T> using sptr = std::shared_ptr<T>;

// Base dx version
struct DxVerBase {}; 
// Supported dx versions
struct DxVer { 
	struct v9 : public DxVerBase { static constexpr unsigned int m_version = 9; };
	struct v10 : public DxVerBase { static constexpr unsigned int m_version = 10; };
	struct v11 : public DxVerBase { static constexpr unsigned int m_version = 11; };
	struct v12 : public DxVerBase { static constexpr unsigned int m_version = 12; };
};

#include "Sys/AlignConst.h"
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif // __clang__
#include "Sys/AlignCode.h"
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__
#include "Sys/AlignMacro.h"
#include "Sys/Hr.h"
#include "Tpl/Trait.h"

#include "Dx/Ctx/CtxDx.h"
#include "Dx/Ctx/CtxTool.h"
#include "Dx/Ctxs.h"
// Alias
namespace prj_3d::HelloWinHlsl::Ty {
	template<class T>
	using StDxCtx_crefPtr = typename Dx::Ctx::CtxDx<T>::cref_ptr_t;
	template<class T>
	using StDxCtx_ptr = typename Dx::Ctx::CtxDx<T>::sptrc_t;

	template<class T>
	using StCtxs_crefPtr = typename Dx::Ctxs<T>::cref_ptr_t;
	template<class T>
	using StCtxs_ptr = typename Dx::Ctxs<T>::uptrc_t;
} // namespace prj_3d::HelloWinHlsl::Ty

#include "Dx/Tool/ConstantBuf/viaStruct/Converter.h"
#include "Dx/Tool/ConstantBuf/viaStruct/HolderMem.h"
#include "Dx/Tool/ConstantBuf/viaStruct/Accessor.h"
#include "Dx/Tool/ConstantBuf/viaStruct/FactoryStrict.h"
#include "Dx/Tool/ConstantBuf/viaStruct/RootSignatureCreator.h"
#include "Dx/Tool/Vertex.h"
#include "Dx/Tool/Shader/Locator.h"
#include "Dx/Tool/Shader/Loader/SourceCode/FromMem.h"
#include "Dx/Tool/Shader/Loader/ByteCode/FromFile.h"
#include "Dx/Tool/Shader/Loader/ArrayC/FromHeader.h"
#include "Dx/Tool/Shader/Loader/ChainSourceCode.h"
#include "Dx/Tool/Shader/Loader/ChainByteCode.h"
#include "Dx/Tool/Shader/Loader/ChainArrayC.h"
#include "Dx/Tool/Shader/Loader/Composite.h"
#include "Dx/Tool/Shader/Predef/PassThrough/Colored.h"
#include "Dx/Tool/Shader/Predef/ChainPassThrough.h"
#include "Dx/Tool/Shader/Predef/Composite.h"
#include "Dx/Tool/VertexBuf.h"
#include "Dx/Tool/IndexBuf.h"
#include "Dx/Tool/Shader/ConstantSetterDx9.h"
#include "Dx/Tool/Predef/Quad.h"
#include "LikeDxut/Timer.h"
#include "LikeDxut/Mouse.h"
#include "Dx/Spec/D12/Waiter.h"
#include "Dx/Spec/D12/Resizer.h"
#include "Dx/Spec/D12/SetterRootSignature.h"
#include "Sys/Wnd/IResizebleWindow.h"
#include "Sys/Wnd/StToken.h"
#include "Sys/Wnd/Proc.h"
#include "Sys/Wnd/AWndProcHolder.h"
#include "Sys/ErrorHandler.h"
#include "Dx/Spec/D12/WindowResizer.h"
#include "Dx/Spec/Any/WindowResizerStub.h"
#include "Dx/AdjustAux.h"

#include "Dx/DynamicData.h"
// Alias, to avoid using "typename" before type
namespace prj_3d::HelloWinHlsl::Ty {
	template<class T>
	using DynamicData_ptr = typename Dx::DynamicData<T>::sptr_t;
} // namespace prj_3d::HelloWinHlsl::Ty

#include "ClientApp/ABaseClientApp.h"
#include "ClientApp/Present/AOrdinary.h"
#include "Dx/Descr.h"

// Alias
namespace prj_3d::HelloWinHlsl::Ty {
	template<class T> using CurClientApp = ClientApp::Present::AOrdinary<T>;
} // namespace prj_3d::HelloWinHlsl::Ty

// TODO(Alex0vSky): make it for all includes?
namespace prj_3d::HelloWinHlsl { 
#include "Sys/HolderClientApp.h"
#include "Sys/HolderType.h"
#include "ClientApp/Configurator/Config.h"
#include "ClientApp/Configurator/Predef.h"
#include "ClientApp/Configurator/Builder.h"
} // namespace prj_3d::HelloWinHlsl

// Alias, only for client usage
namespace prj_3d::HelloWinHlsl::ClientTy {
	template<class T> using DxCtx = Dx::Ctx::CtxDx< T >;
	template<class T> using ToolCtx = Dx::Ctx::CtxTool< T >;
	template<class T> using Adjust = Dx::AdjustAux< T >;
	template<class T> using Dynamic = Dx::ClientDynamicData< T >;
	typedef ClientApp::Configurator::Config Config;
	template<class T> using CurClientApp = Ty::CurClientApp< T >;
	// Only declared, client code must define
	//	Point of entry.
	uptr<Sys::HolderClientApp> entryPoint();
	//	Configurator.
	ClientApp::Configurator::Config::uptrc_t configure();
} // namespace prj_3d::HelloWinHlsl::ClientTy

// For your own entryPoint
#include "DxtkFont/CompiledToBinary.h"
#include "DrawAux/Fps/Ordinary.h"
#include "FrameLimiter/ABase.h"
#include "FrameLimiter/ViaSleep.h"
#include "FrameLimiter/Unlimit.h"
#include "Sys/Rtti.h"
#include "Sys/Wnd/Creator.h"
#include "Sys/Wnd/Change.h"
#include "GameLoop/ABase.h"
#include "GameLoop/BaseInCurrentThread.h"
#include "GameLoop/BaseInSeparateThread.h"
#include "Dx/Spec/D12/GameLoopPart.h"
#include "GameLoop/InCurrentThread.h"
#include "GameLoop/InSeparateThread.h"
#include "LikeDxut/Camera/Default.h"
#include "Dx/Adapter/Dx9Adapter.h"
#include "Dx/Adapter/Dx10Adapter.h"
#include "Dx/Adapter/Dx11Adapter.h"
#include "Dx/Adapter/Dx12Adapter.h"
#include "Dx/Ctx/Impl/Dx9CtxImpl.h"
#include "Dx/Ctx/Impl/Dx10CtxImpl.h"
#include "Dx/Ctx/Impl/Dx11CtxImpl.h"
#include "Dx/Ctx/Impl/Dx12CtxImpl.h"
#include "Dx/Ctx/FactoryCtxs.h"
#include "Main.h"
