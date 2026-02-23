#include "BossFloor.h"
#include "Components/StaticMeshComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Default.h" 

ABossFloor::ABossFloor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. 메쉬 설정
	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	RootComponent = FloorMesh;

	// 눈에 안 보이게 설정 (게임 시작 시)
	FloorMesh->SetHiddenInGame(true);

	// 2. 콜리전 기본 설정
	FloorMesh->SetCollisionProfileName(TEXT("Custom"));
	FloorMesh->SetCollisionObjectType(ECC_WorldStatic);

	// 기본적으로 다 무시 (플레이어 추락)
	FloorMesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	// ★중요: 네비게이션 시스템이 이 메쉬를 인식하도록 강제함
	FloorMesh->SetCanEverAffectNavigation(true);

	// 3. 네비게이션 모디파이어 설정
	// SetupAttachment 삭제함 (ActorComponent라서 필요 없음)
	NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));

	// 이 영역은 '일반 땅(Default)'으로 인식해라
	NavModifier->SetAreaClass(UNavArea_Default::StaticClass());

}