// Fill out your copyright notice in the Description page of Project Settings.


#include "MapPortal.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Evaluation/IMovieSceneEvaluationHook.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Bang_Squad/Character/Base/BaseCharacter.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/UI/Stage/PortalMainWidget.h"

AMapPortal::AMapPortal()
{
	bReplicates = true;
	bAlwaysRelevant = true;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
	
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->SetupAttachment(RootComponent);
	TriggerSphere->SetSphereRadius(300.f);
	TriggerSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetBoxExtent(FVector(300.f, 300.f, 100.f));
	TriggerBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
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

void AMapPortal::SaveAllPuzzles()
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (!GI) return;

	// 맵에 있는 ISaveInterface를 가진 모든 액터 찾기
	TArray<AActor*> PuzzleActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), PuzzleActors);

	for (AActor* Actor : PuzzleActors)
	{
		ISaveInterface* SaveInterface = Cast<ISaveInterface>(Actor);
		if (SaveInterface)
		{
			FActorSaveData TempData;
			SaveInterface->SaveActorData(TempData); // 가방에 데이터 채우라고 명령
            
			// 다 채워진 가방을 GameInstance에 전달
			GI->SaveDataToInstance(SaveInterface->GetSaveID(), TempData); 
		}
	}
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
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMapPortal::OnOverlapBegin);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AMapPortal::OnOverlapEnd);
		
		FTimerHandle InitTimer;
		GetWorld()->GetTimerManager().SetTimer(InitTimer, [this]()
		{
			if (IsValid(this))
			{
				int32 MaxPlayers = 0;
				if (GetWorld()->GetGameState())
				{
					MaxPlayers = GetWorld()->GetGameState()->PlayerArray.Num();
				}
				MulticastUpdateUI(0, MaxPlayers, -1);
			}
		}, 0.5f, false);
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
		PortalWidgetComp->SetCullDistance(MaxDrawDistance);

		//UPortalMainWidget* PortalUI = Cast<UPortalMainWidget>(PortalWidgetComp->GetUserWidgetObject());
		//if (PortalUI && GetWorld()->GetGameState())
		//{
		//	PortalUI->InitializePortal(GetWorld()->GetGameState()->PlayerArray.Num());
		//	PortalUI->UpdatePlayerCount(0, GetWorld()->GetGameState()->PlayerArray.Num());
		//}
	}
	GetWorld()->GetTimerManager().SetTimer(CheckDistanceTimerHandle, this, &AMapPortal::CheckWidgetDistance, 0.1f, true);
}

void AMapPortal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (PortalShape == EPortalShape::Sphere)
	{
		TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 박스 끄기
		TriggerBox->SetHiddenInGame(true); // 디버그 선 숨기기
	}
	else
	{
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TriggerSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 원 끄기
		TriggerSphere->SetHiddenInGame(true);
	}
}

void AMapPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	//BaseCharacter만 감지
	if (OtherActor && OtherActor->IsA(ABaseCharacter::StaticClass()))
	{
		OverlappingPlayers.Add(OtherActor);

		int32 MaxPlayers = 0;
		AGameStateBase* GS = GetWorld()->GetGameState();
		if (GS)
		{
			MaxPlayers = GS->PlayerArray.Num();
		}

		MulticastUpdateUI(OverlappingPlayers.Num(), MaxPlayers, RemainingTime);
		//MulticastUpdateUI(OverlappingPlayers.Num(), RemainingTime);

		//GameState를 통해 플레이어 수 확인
		//AGameStateBase* GS = GetWorld()->GetGameState();
		if (GS && OverlappingPlayers.Num() >= MaxPlayers && MaxPlayers > 0)
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

		int32 MaxPlayers = 0;
		if (GetWorld()->GetGameState())
		{
			MaxPlayers = GetWorld()->GetGameState()->PlayerArray.Num();
		}
		MulticastUpdateUI(OverlappingPlayers.Num(), MaxPlayers, -1);
	}
}

void AMapPortal::CheckWidgetDistance()
{
	if (!PortalWidgetComp || !PortalWidgetComp->GetUserWidgetObject()) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager) return;

	FVector CamLoc = PC->PlayerCameraManager->GetCameraLocation();
	float Distance = FVector::Dist(GetActorLocation(), CamLoc);

	// Fade 로직 (방법 2 활용)
	float FadeStart = MaxDrawDistance * 0.8f;
	float FadeEnd = MaxDrawDistance;

	if (Distance > FadeEnd)
	{
		// 완전히 멀어지면 렌더링 자체를 꺼버림 (성능 절약)
		if (PortalWidgetComp->IsVisible()) 
			PortalWidgetComp->SetVisibility(false);
	}
	else
	{
		if (!PortalWidgetComp->IsVisible()) 
			PortalWidgetComp->SetVisibility(true);

		float Alpha = 1.0f;
		if (Distance > FadeStart)
		{
			Alpha = 1.0f - ((Distance - FadeStart) / (FadeEnd - FadeStart));
		}

		// Cast 비용을 줄이기 위해 멤버변수로 캐싱해두는 것을 추천
		if (UUserWidget* Widget = PortalWidgetComp->GetUserWidgetObject())
		{
			Widget->SetRenderOpacity(Alpha);
		}
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
		RemainingTime = -1;
		//텍스트 초기화
		//MulticastUpdateUI(OverlappingPlayers.Num(), -1);
	}
}

void AMapPortal::ProcessLevelTransition()
{
	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (GI)
	{
		SaveAllPuzzles();
		GI->MarkStageAsVisited(TargetStageIndex, TargetSection);
		GI->MoveToStage(TargetStageIndex, TargetSection);
	}
}

void AMapPortal::UpdateCountdownText()
{
	if (RemainingTime >= 0)
	{
		int32 MaxPlayers = 0;
		if (GetWorld()->GetGameState())
		{
			MaxPlayers = GetWorld()->GetGameState()->PlayerArray.Num();
		}
		
		MulticastUpdateUI(OverlappingPlayers.Num(), MaxPlayers, RemainingTime);
		RemainingTime--;
	}
}

void AMapPortal::MulticastUpdateUI_Implementation(int32 CurrentPlayerCount, int32 MaxPlayers, int32 CurrentTime)
{
	if (!PortalWidgetComp) return;

	UPortalMainWidget* PortalUI = Cast<UPortalMainWidget>(PortalWidgetComp->GetUserWidgetObject());
	if (PortalUI)
	{
		//PortalUI->UpdatePlayerCount(CurrentPlayerCount, GetWorld()->GetGameState()->PlayerArray.Num());
		PortalUI->UpdatePortalState(CurrentPlayerCount, MaxPlayers, CurrentTime);
	}
}
