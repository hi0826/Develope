#pragma once

#include "stdafx.h"
#include "DDSTextureLoader12.h"
//#include "DDSTextureLoader.h"
#include "Mesh.h"

bool LoadMD5Model(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CMesh*& mesh, std::wstring filename,
	Model3D& MD5Model, std::vector<ID3D12Resource*>& shaderResourceViewArray, std::vector<std::wstring> texFileNameArray, float sx, float sy, float sz);

bool LoadIlluminatedMD5Model(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CMesh*& mesh, std::wstring filename,
	Model3D& MD5Model,
	std::vector<ID3D12Resource*>& shaderResourceViewArray,
	std::vector<std::wstring> texFileNameArray, float sx, float sy, float sz);

bool LoadMD5Anim(std::wstring filename, Model3D& MD5Model);

int UpdateMD5Model(CMesh*& p_Mesh, Model3D& MD5Model, float deltaTime, int animation);
