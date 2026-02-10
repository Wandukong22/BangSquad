// Fill out your copyright notice in the Description page of Project Settings.


#include "MapPortal.h"

#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Character/Base/BaseCharacter.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/UI/Stage/PortalMainWidget.h"

AMapPortal::AMapPortal()
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

	PortalWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("PortalWidgetComp"));
	PortalWidgetComp->SetupAttachment(RootComponent);
	PortalWidgetComp->SetRelativeLocation(FVector(0.f, 0.f, 200.f)); // 포탈보다 약간 위에
	PortalWidgetComp->SetWidgetSpace(EWidgetSpace::Screen); // Screen으로 하면 항상 정면을 바라봄 (World로 하면 3D 물체처럼 보임)
	PortalWidgetComp->SetDrawSize(FVector2D(500.f, 200.f)); // 위젯 크기 적절히 조절
}

void AMapPortal::ActivatePortal()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	if (TriggerSphere) TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMapPortal::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
		if (GI && GI->HasVisitedStage(TargetStageIndex, TargetSection))
		{
			Destroy();
			return;
		}
		
		TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AMapPortal::OnOverlapBegin);
		TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AMapPortal::OnOverlapEnd);

	}

	if (!bIsStartActive)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		if (TriggerSphere) TriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (PortalWidgetComp)
	{
		PortalWidgetComp->InitWidget();
		
		UPortalMainWidget* PortalUI = Cast<UPortalMainWidget>(PortalWidgetComp->GetUserWidgetObject());
		if (PortalUI && GetWorld()->GetGameState())
		{
			PortalUI->InitializePortal(GetWorld()->GetGameState()->PlayerArray.Num());
			PortalUI->UpdatePlayerCount(0, GetWorld()->GetGameState()->PlayerArray.Num());
		}
	}
}

void AMapPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	//BaseCharacter만 감지
	if (OtherActor && OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		OverlappingPlayers.Add(OtherActor);

		MulticastUpdateUI(OverlappingPlayers.Num(), RemainingTime);

		//GameState를 통해 플레이어 수 확인
		AGameStateBase* GS = GetWorld()->GetGameState();
		if (GS && OverlappingPlayers.Num() >= GS->PlayerArray.Num())
		{
			StartCountdown();
		}
	}
}

void AMapPortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) return;
	if (OtherActor && OverlappingPlayers.Contains(OtherActor))
	{
		OverlappingPlayers.Remove(OtherActor);
		CancelCountdown();

		MulticastUpdateUI(OverlappingPlayers.Num(), -1);
	}
}

void AMapPortal::StartCountdown()
{
	RemainingTime = 5;

	//5초 후 이동
	GetWorldTimerManager().SetTimer(TravelTimerHandle, this, &AMapPortal::ProcessLevelTransition, 5.f, false);
	GetWorldTimerManager().SetTimer(CountdownUpdateTimer, this, &AMapPortal::UpdateCountdownText, 1.f, true);

	UpdateCountdownText();
}

void AMapPortal::CancelCountdown()
{
	if (GetWorldTimerManager().IsTimerActive(TravelTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(TravelTimerHandle);
		GetWorldTimerManager().ClearTimer(CountdownUpdateTimer);

		//텍스트 초기화
		MulticastUpdateUI(OverlappingPlayers.Num(), -1);
	}
}

void AMapPortal::ProcessLevelTransition()
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->MarkStageAsVisited(TargetStageIndex, TargetSection);
		GI->MoveToStage(TargetStageIndex, TargetSection);
	}
}

void AMapPortal::UpdateCountdownText()
{
	if (RemainingTime > 0)
	{
		MulticastUpdateUI(OverlappingPlayers.Num(), RemainingTime);
		RemainingTime--;
	}
}

void AMapPortal::MulticastUpdateUI_Implementation(int32 CurrentPlayerCount, int32 CurrentTime)
{
	if (!PortalWidgetComp) return;

	UPortalMainWidget* PortalUI = Cast<UPortalMainWidget>(PortalWidgetComp->GetUserWidgetObject());
	if (PortalUI && GetWorld()->GetGameState())
	{
		PortalUI->UpdatePlayerCount(CurrentPlayerCount, GetWorld()->GetGameState()->PlayerArray.Num());
	}
}
