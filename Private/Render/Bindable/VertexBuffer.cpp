//#include "VertexBuffer.h"
//
//
//
////VertexBuffer<T>::VertexBuffer(const T& vertics)
////{
////	D3D11_BUFFER_DESC bd = {};
////	ZeroMemory(&bd, sizeof(bd));
////	bd.ByteWidth = sizeof(vertices);
////	bd.Usage = D3D11_BIND_VERTEX_BUFFER;
////	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
////	bd.CPUAccessFlags = 0;
////	D3D11_SUBRESOURCE_DATA InitData;
////	ZeroMemory(&InitData, sizeof(InitData));
////	InitData.pSysMem = &vertices.at(0);
////	pDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);
////}
//template<typename T, template<typename U> typename Container>
//VertexBuffer<T,Container>::VertexBuffer(const Container<T>& vertics)
//{
//	D3D11_BUFFER_DESC bd = {};
//	ZeroMemory(&bd, sizeof(bd));
//	bd.ByteWidth = sizeof(vertices);
//	bd.Usage = D3D11_BIND_VERTEX_BUFFER;
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	bd.CPUAccessFlags = 0;
//	D3D11_SUBRESOURCE_DATA InitData;
//	ZeroMemory(&InitData, sizeof(InitData));
//	InitData.pSysMem = &vertices.at(0);
//	pDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);
//}
//
//template<typename T, template<typename U> typename Container>
//void VertexBuffer<T, Container>::Bind(Graphics& gfx)
//{
//	UINT stride = sizeof(T);
//	UINT offset = 0u;
//	gfx.pDeviceContext->IASetVertexBuffers(0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset)
//}
//
//template<typename T, template<typename U> typename Container>
//void VertexBuffer<T, Container>::tBind(const ID3D11DeviceContext& pc)
//{
//	UINT stride = sizeof(T);
//	UINT offset = 0u;
//	pc.IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset)
//}
