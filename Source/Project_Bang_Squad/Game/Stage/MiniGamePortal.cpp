// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGamePortal.h"

#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Character/Base/BaseCharacter.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

AMiniGamePortal::AMiniGamePortal()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	RootComponent = TriggerSphere;
	TriggerSphere->SetSphereRadius(300.f);
	TriggerSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(RootComponent);
	PortalMesh->SetCollisionProfileName(TEXT("NoCollision"));

	CountdownText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("CountdownText"));
	CountdownText->SetupAttachment(RootComponent);
	CountdownText->SetText(FText::GetEmpty());
	CountdownText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	CountdownText->SetRelativeLocation(FVector(0.f, 0.f, 150.f));
}

void AMiniGamePortal::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
		if (GI && GI->GetbHasVisitedMiniGame())
		{
			Destroy();
			return;
		}
		
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AMiniGamePortal::OnOverlapBegin);
		TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AMiniGamePortal::OnOverlapEnd);
	}
}

void AMiniGamePortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	//BaseCharacter만 감지
	if (OtherActor && OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		OverlappingPlayers.Add(OtherActor);

		//GameState를 통해 플레이어 수 확인
		AGameStateBase* GS = GetWorld()->GetGameState();
		if (GS && OverlappingPlayers.Num() == GS->PlayerArray.Num())
		{
			StartCountdown();
		}
	}
}

void AMiniGamePortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	if (OtherActor && OverlappingPlayers.Contains(OtherActor))
	{
		OverlappingPlayers.Remove(OtherActor);
		CancelCountdown();
	}
}

void AMiniGamePortal::StartCountdown()
{
	RemainingTime = 5;

	//5초 후 이동
	GetWorldTimerManager().SetTimer(TravelTimerHandle, this, &AMiniGamePortal::ProcessLevelTransition, 5.f, false);

	//1초마다 텍스트 갱신
	GetWorldTimerManager().SetTimer(CountdownUpdateTimer, this, &AMiniGamePortal::UpdateCountdownText, 1.f, true);
	UpdateCountdownText();
}

void AMiniGamePortal::CancelCountdown()
{
	if (GetWorldTimerManager().IsTimerActive(TravelTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(TravelTimerHandle);
		GetWorldTimerManager().ClearTimer(CountdownUpdateTimer);

		//텍스트 초기화
		MulticastUpdateText(TEXT("Canceled"));
	}
}

void AMiniGamePortal::ProcessLevelTransition()
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetbHasVisitedMiniGame(true);
	}
	GetWorld()->ServerTravel("/Game/TeamShare/Level/Stage1_MiniGame?listen");
}

void AMiniGamePortal::UpdateCountdownText()
{
	if (RemainingTime > 0)
	{
		MulticastUpdateText(FString::FromInt(RemainingTime));
		RemainingTime--;
	}
}

void AMiniGamePortal::MulticastUpdateText_Implementation(const FString& NewText)
{
	if (CountdownText)
		CountdownText->SetText(FText::FromString(NewText));
}
