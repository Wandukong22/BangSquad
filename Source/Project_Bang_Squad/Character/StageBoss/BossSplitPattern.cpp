// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Character/StageBoss/BossSplitPattern.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

ABossSplitPattern::ABossSplitPattern()
{
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	//A구역 설정
	ZoneA_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneA_Mesh"));
	ZoneA_Mesh->SetupAttachment(RootComponent);
	ZoneA_Mesh->SetRelativeLocation(FVector(0.f, -400.f, 0.f));

	ZoneA_Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneA_Trigger"));
	ZoneA_Trigger->SetupAttachment(ZoneA_Mesh);
	ZoneA_Trigger->SetBoxExtent(FVector(100, 100, 100));

	ZoneA_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ZoneA_Text"));
	ZoneA_Text->SetupAttachment(ZoneA_Mesh);
	ZoneA_Text->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

	//B구역 설정
	ZoneB_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneB_Mesh"));
	ZoneB_Mesh->SetupAttachment(RootComponent);
	ZoneB_Mesh->SetRelativeLocation(FVector(0.f, 400.f, 0.f));

	ZoneB_Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneB_Trigger"));
	ZoneB_Trigger->SetupAttachment(ZoneB_Mesh);
	ZoneB_Trigger->SetBoxExtent(FVector(100, 100, 100));

	ZoneB_Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ZoneB_Text"));
	ZoneB_Text->SetupAttachment(ZoneB_Mesh);
	ZoneB_Text->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
}

void ABossSplitPattern::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		//트리거 이벤트 바인딩
		ZoneA_Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABossSplitPattern::OnOverlapUpdate);
		ZoneA_Trigger->OnComponentEndOverlap.AddDynamic(this, &ABossSplitPattern::OnEndOverlapUpdate);

		ZoneB_Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABossSplitPattern::OnOverlapUpdate);
		ZoneB_Trigger->OnComponentEndOverlap.AddDynamic(this, &ABossSplitPattern::OnEndOverlapUpdate);
	}
}

void ABossSplitPattern::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABossSplitPattern, RequiredA);
	DOREPLIFETIME(ABossSplitPattern, RequiredB);
}

void ABossSplitPattern::ActivatePattern()
{
	if (!HasAuthority()) return;

	//랜덤 숫자 뽑기
	int32 ToTalPlayers = GetWorld()->GetNumPlayerControllers();
	RequiredA = FMath::RandRange(0, ToTalPlayers);
	RequiredB = ToTalPlayers - RequiredA;

	//텍스트 갱신
	OnRep_UpdateTexts();

	//타이머 시작
	//TODO: 임시로 5초로 해둠
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABossSplitPattern::CheckResult, 5.0f, false);
}

void ABossSplitPattern::OnRep_UpdateTexts()
{
	ZoneA_Text->SetText(FText::AsNumber(RequiredA));
	ZoneB_Text->SetText(FText::AsNumber(RequiredB));
}

void ABossSplitPattern::OnOverlapUpdate(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->IsA(ACharacter::StaticClass())) return;

	if (OverlappedComp == ZoneA_Trigger) CurrentA++;
	else if (OverlappedComp == ZoneB_Trigger) CurrentB++;
}

void ABossSplitPattern::OnEndOverlapUpdate(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->IsA(ACharacter::StaticClass())) return;

	if (OverlappedComp == ZoneA_Trigger) CurrentA--;
	else if (OverlappedComp == ZoneB_Trigger) CurrentB--;
}

void ABossSplitPattern::CheckResult()
{
	bool bSuccess = (CurrentA == RequiredA) && (CurrentB == RequiredB);
	OnSplitPatternFinished.Broadcast(bSuccess);

	//패턴끝나고 없앰
	Destroy();
}



