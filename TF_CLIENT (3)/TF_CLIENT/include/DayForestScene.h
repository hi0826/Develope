#pragma once
#include "Scene.h"
class DayForestScene :public CScene
{
	typedef  void(DayForestScene::*functionPointer)(char* Packet);
	typedef  std::map<char, functionPointer>  ProcessorMap;
	ProcessorMap        Processor;

private:
	  CMoveObject* STONEMONS[2];

public:
	DayForestScene();
	~DayForestScene();

public:
	virtual void ProcessInput(float fDeltaTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Update(float fDeltaTime);
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void ReleaseUploadBuffers();
	virtual void PacketProcess(char* packet) { (this->*Processor[packet[1]])(packet); }

public:
	void BuildLightsAndMaterials();
	
public:
	void PutNPC(char* packet);
	void MoveNPC(char* packet);
	void RemoveNPC(char* packet);

};

