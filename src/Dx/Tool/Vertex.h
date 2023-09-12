// Dx/Tool/Vertex.h - vertices type
#pragma once
namespace prj_3d::HelloWinHlsl::Dx::Tool {
template<class T> struct Vertex; // primary template

template<> struct Vertex<DxVer::v9> {
	D3DXVECTOR3 xyz;
	FLOAT rhw;
	DWORD color;
};

template<> struct Vertex<DxVer::v10> {
	D3DXVECTOR3 pos_;
	D3DXCOLOR color_;
};

template<> struct Vertex<DxVer::v11> {
	D3DXVECTOR3 pos_;
	D3DXCOLOR color_;
};

template<> struct Vertex<DxVer::v12> {
	DirectX::XMFLOAT3 pos_;
	DirectX::XMFLOAT4 color_;
};
} // namespace prj_3d::HelloWinHlsl::Dx::Tool
