#include "Project_Bang_Squad/Character/Player/Titan/TitanThrowableActor.h"

#include <ThirdParty/ShaderConductor/ShaderConductor/External/DirectXShaderCompiler/include/dxc/DXIL/DxilConstants.h>

ATitanThrowableActor::ATitanThrowableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. ��Ƽ�÷��� �̵� ����ȭ (�������� ������ Ŭ�� ���ư���)
	bReplicates = true;
	SetReplicateMovement(true);

	// 2. �޽� ������Ʈ ����
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// [�߿�] ��ü�� �����̷��� �ݵ�� Movable�̾�� �� (Static�̸� ���� �ξ���)
	MeshComp->SetMobility(EComponentMobility::Movable);

	// 3. ���� �� �浹 �⺻ ����
	// (������ ������ �������� ���� ������, �⺻������ �ѵδ� ����)
	
	MeshComp->SetEnableGravity(true);

	// PhysicsActor: ���� ����� ������ (������Ʈ ������ ���� �ٸ� �� ������ Ȯ�� �ʿ�)
	// ���� Ŀ���� �������� ���ٸ� 'PhysicsActor' ��� 'BlockAllDynamic' ���� ���
	MeshComp->SetCollisionProfileName(TEXT("PhysicsActor"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// ���� ���� (�ʹ� ������� ����ó�� ���ư���, �ʹ� ���ſ�� �� ���ư�)
}

void ATitanThrowableActor::BeginPlay()
{
	Super::BeginPlay();

	// [�ٽ�] ���� ���� �� ������ ������ �߷��� �ѹ����ϴ�.
	// �������Ʈ ������ �����־ �� �ڵ尡 �̱�ϴ�.
	if (MeshComp)
	{
		MeshComp->SetMobility(EComponentMobility::Movable);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetSimulatePhysics(true); // ���� �ѱ�
		MeshComp->SetEnableGravity(true); // �߷� �ѱ�

		// (����) �ʹ� ������� �� �������� ������ �� �� ������ ���� ����
		MeshComp->SetMassOverrideInKg(NAME_None, 50.0f);
	}
}

void ATitanThrowableActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (MeshComp)
	{
		MeshComp->SetSimulatePhysics(true);
		MeshComp->SetMassOverrideInKg(NAME_None, 50.0f);
	}
}

void ATitanThrowableActor::OnThrown_Implementation(FVector Direction)
{
}
