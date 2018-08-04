#pragma once
#include "Scene.h"
class DayForestScene :public CScene
{
	typedef  void(DayForestScene::*functionPointer)(char* Packet);
	typedef  std::map<char, functionPointer>  ProcessorMap;
	ProcessorMap        Processor;

private:
	std::unordered_map<string,INSTANCEOB> m_mProtoType; 
<<<<<<< HEAD
=======
	std::unordered_map<string, vector<ModelAnimation>> m_mAniProtoType;
>>>>>>> 1b444bfd5c9d5d477b5c55d7bd8f583a66b25add
	int					    m_MapWidth, m_MapHeight;
	int**				    m_MapFile;
	CPlaneMap*			    m_pMap;
	XMFLOAT3*			    m_PosTree;
	XMFLOAT3*			    m_PosMonster;
	vector<CStaticObj*>     m_StaticObjects;
	vector<CMoveObject*>    m_vMonsters;
<<<<<<< HEAD
	CMoveObject**			m_pMonsters;

=======
	std::vector<ModelAnimation> Warrior_animations;
>>>>>>> 1b444bfd5c9d5d477b5c55d7bd8f583a66b25add
public:
	DayForestScene();
	virtual ~DayForestScene();

public:
	virtual void ProcessInput(float fDeltaTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Update(float fDeltaTime);
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void ReleaseUploadBuffers();
	virtual void PacketProcess(char* packet) { (this->*Processor[packet[1]])(packet); }

public:
	void BuildLightsAndMaterials();
	void ReadMap();
	void CreateStaticObjectFromMapFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void CreateMovableObjectFromMapFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandLIst);

<<<<<<< HEAD
	void CreateStaticObProtoType(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const std::wstring filePath, const std::string strTag, const XMFLOAT3 scale);

	INSTANCEOB FindStaticObProtoType(const std::string& strKey);
=======
	void CreateStaticObProtoType(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const std::wstring filePath, const std::string strTag);
	void CreateAniProtoType(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const std::string strTag);
	INSTANCEOB FindStaticObProtoType(const std::string& strKey);
	vector<ModelAnimation> FindAniProtoType(const std::string& strKey);

>>>>>>> 1b444bfd5c9d5d477b5c55d7bd8f583a66b25add

public: /*Packet 처리함수*/
	void PutNPC(char* packet);
	void MoveNPC(char* packet);
	void HeatedNPC(char* packet);
	void RemoveNPC(char* packet);

};

