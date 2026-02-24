#include "BossPlatform.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ABossPlatform::ABossPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(MeshComp);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossPlatform::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABossPlatform::OnOverlapEnd);

	MoveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("MoveTimeline"));
}

void ABossPlatform::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();

	if (MeshComp) DynMaterial = MeshComp->CreateAndSetMaterialInstanceDynamic(0);

	if (MovementCurve)
	{
		FOnTimelineFloat ProgressUpdate;
		ProgressUpdate.BindUFunction(this, FName("HandleProgress"));

		FOnTimelineEvent FinishedEvent;
		FinishedEvent.BindUFunction(this, FName("OnMoveFinished"));

		MoveTimeline->AddInterpFloat(MovementCurve, ProgressUpdate);
		MoveTimeline->SetTimelineFinishedFunc(FinishedEvent);
	}
}

// [핵심 로직] 타임라인이 도는 동안 호출됨
void ABossPlatform::HandleProgress(float Value)
{
	// 1. 기본 이동 위치 계산 (Lerp)
	FVector TargetLoc = FMath::Lerp(InitialLocation, InitialLocation + FVector(0, 0, DownDepth), Value);

	// 2. [물리 진동] 랜덤한 떨림 추가
	// FMath::VRand()는 길이 1짜리 랜덤 벡터를 반환 -> Intensity를 곱해 강도 조절
	FVector ShakeOffset = FMath::VRand() * ShakeIntensity;

	// Z축 떨림은 제외하고 싶다면: ShakeOffset.Z = 0.0f; (선택사항)

	// 3. 최종 위치 적용
	SetActorLocation(TargetLoc + ShakeOffset);
}

// 이동이 끝나면 떨림을 멈추고 정확한 위치로 고정
void ABossPlatform::OnMoveFinished()
{
	FVector FinalLoc;
	if (bIsDown || bIsPermanentlyDown)
	{
		FinalLoc = InitialLocation + FVector(0, 0, DownDepth);
	}
	else
	{
		FinalLoc = InitialLocation;
	}
	SetActorLocation(FinalLoc);
}

void ABossPlatform::SetWarning(bool bActive)
{
	if (DynMaterial)
	{
		DynMaterial->SetScalarParameterValue(FName("WarningAlpha"), bActive ? 1.0f : 0.0f);
	}
}

void ABossPlatform::MoveDown(bool bPermanent)
{
	if (bIsPermanentlyDown) return;

	bIsDown = true;
	if (bPermanent) bIsPermanentlyDown = true;

	SetWarning(false);

	// [속도 제어] 빠르게 하강 (0.5초) -> PlayRate 2.0
	MoveTimeline->SetPlayRate(2.0f);
	MoveTimeline->Play();

	if (!bPermanent)
	{
		// 7초 후 자동 복귀
		FTimerHandle ReturnTimer;
		GetWorldTimerManager().SetTimer(ReturnTimer, this, &ABossPlatform::MoveUp, 7.0f, false);
	}
}

void ABossPlatform::MoveUp()
{
	if (bIsPermanentlyDown) return;

	bIsDown = false;

	// [속도 제어] 천천히 상승 (1.0초) -> PlayRate 1.0
	MoveTimeline->SetPlayRate(1.0f);
	MoveTimeline->Reverse();
}

void ABossPlatform::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
		OverlappingPlayers.Add(OtherActor);
}

void ABossPlatform::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
		OverlappingPlayers.Remove(OtherActor);
}