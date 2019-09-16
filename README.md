# Trinity Forest Readme
해당 프로젝트는 4인팀으로 구성된 프로젝트 입니다.
2017년 8월 기획단계부터 2018년 10월 전시까지의 프로젝트 입니다.
그래픽 : 진선영
클라이언트 : 차현오, 최홍일
서버, 프레임워크 설계 : 강진구
로 구성된 팀입니다.

# Client 최홍일
개발 내용은 인스턴싱과 그림자가 주요 개발 내용입니다.

# Instancing
DayForestScene::Initialize() 에서
인스턴싱에 필요한 모든 정적 객체들의 모델을 먼저 읽습니다.

그리고 DayForestScene::CreateStaticObjectFromFile() 에서
각각 렌더링될 정적 객체들의 Shader 객체들을 만들어줍니다.

Shader를 만들어줄 때 Create~VertexShader / Create~PixelShader 함수들로
hlsl에 있는 함수들에 쉐이더를 연결시켜줍니다.

Shader 객체 안에는 GameObject의 배열이 있습니다.
하나의 모델과 텍스쳐를 공유하기 때문에,
Shader 객체 내부의 게임 오브젝트는 위치정보만 가지고 있으면 되기 때문입니다.

CInstancingShader::Render() 
Shader가 가지고 있는 Object에 대한 Render를 객체의 수를 매개변수로 호출합니다.

CGameObject::Render() 
해당 오브젝트에 연결된 Mesh에 객체의 수를 매개변수로 넘겨주면서 Render를 호출시킵니다.

CMesh::Render()
Index버퍼가 있는지 확인하고, 
있으면 DrawIndexedInstanced() 함수를 호출하여 그리기 명령을 실행시키고
없으면 DrawInstanced() 함수를 호출하여 그리기 명령을 실행시킵니다.

# Shadow
DayForestScene::CreateStaticObjectFromFile() 에서
기존 정적 객체들과 함께 ShadowShader를 함께 만들어 Shader 배열에 넣어줍니다.
ShadowShader를 만들면서 CInstancingShader::InitializeShadow() 함수를 호출합니다.

CInstancingShader::InitializeShadow()
그림자의 대상이 될 Shader 와 함께 선언하면서 동시에 그림자 벡터를 만들어서 넣어줍니다.
그림자 벡터는 DirectX SDK 의 XMMatrixShadow(그림자가 생길 평면 벡터, 빛으로 향하는 방향벡터)를 매개인자로 넣습니다.
XMMatrixShadow() 함수로 나온 그림자 벡터를 해당 객체의 WorldMatrix에 곱해주면 WorldMatrix가 그림자로 바뀝니다.

CInstancingShader::Render() , CGameObject::Render(), CMesh::Render() 를 거쳐서
쉐이더로 넘어갑니다.

# Shaders.hlsl
쉐이더파일에서는 오브젝트들이 Render() 함수의 그리기 명령으로 넘어올 때마다 이전에 쉐이더를 생성해줄때
함께 컴파일한 쉐이더 함수를 찾아와 쉐이더 단계를 실행하고 다음 그래픽스 파이프라인으로 진행합니다.
픽셀쉐이더를 거치면 색을 가지며, 화면에 렌더링 됩니다.
