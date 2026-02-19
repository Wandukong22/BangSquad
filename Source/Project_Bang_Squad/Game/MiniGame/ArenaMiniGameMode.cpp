// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaMiniGameMode.h"

#include "ArenaGameState.h"
#include "ArenaPlayerState.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

AArenaMiniGameMode::AArenaMiniGameMode()
{
	PlayerStateClass = AArenaPlayerState::StaticClass();
	GameStateClass = AArenaGameState::StaticClass();
}

void AArenaMiniGameMode::OnPlayerDied(AController* VictimController)
{
	if (!VictimController) return;

	AArenaPlayerState* PS = VictimController->GetPlayerState<AArenaPlayerState>();
	if (PS && PS->GetIsAlive())
	{
		PS->SetIsAlive(false);
		PS->SetArenaRank(AlivePlayerCount);
		AlivePlayerCount--;

		if (AlivePlayerCount <= 1)
		{
			GetWorldTimerManager().ClearTimer(ArenaTimerHandle);

			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				APlayerController* PC = Cast<APlayerController>(It->Get());
				if (PC)
				{
					AArenaPlayerState* ArenaPS = Cast<AArenaPlayerState>(PC->PlayerState);
					if (ArenaPS && ArenaPS->GetIsAlive() && PC->GetPawn())
					{
						PC->GetPawn()->SetCanBeDamaged(false);
						//TODO: UI 띄우기
						break;
					}
				}
			}

			//Stage3 Main으로 이동
			FTimerHandle ReturnTimerHandle;
			GetWorldTimerManager().SetTimer(ReturnTimerHandle, FTimerDelegate::CreateLambda([this]()
			{
				if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
				{
					GI->MoveToStage(EStageIndex::Stage3, EStageSection::Main);
				}
			}), 5.f, false);
		}
	}
}

void AArenaMiniGameMode::BeginPlay()
{
	Super::BeginPlay();

	AlivePlayerCount = GetNumPlayers();

	GetWorldTimerManager().SetTimer(ArenaTimerHandle, this, &AArenaMiniGameMode::UpdateArenaTimer, 1.f, true);
}

void AArenaMiniGameMode::UpdateArenaTimer()
{
	AArenaGameState* GS = GetGameState<AArenaGameState>();
	if (!GS) return;

	GS->SetRemainingTime(GS->GetRemainingTime() - 1);

	if (GS->GetRemainingTime() <= 0)
	{
		EArenaPhase CurrentPhase = GS->GetCurrentPhase();
		//페이즈 전환
		if (CurrentPhase == EArenaPhase::Waiting)
		{
			GS->SetCurrentPhase(EArenaPhase::Surviving);
			GS->SetRemainingTime(20);
		}
		else if (CurrentPhase == EArenaPhase::Surviving)
		{
			GS->SetCurrentPhase(EArenaPhase::FloorSinking);
			GS->SetRemainingTime(5);
			//TODO: 바닥 가라앉는 이벤트?
			int32 TargetFloorNum = GS->GetCurrentSinkingFloor();

			/*
			for (TActorIterator<AArenaFloor> It(GetWorld()); It; ++It)
			{
				AArenaFloor* Floor = *It;
				if (Floor && Floor->FloorNumber == TargetFloorNum)
				{
					// 멀티캐스트 RPC나 RepNotify를 통해 클라이언트 측에서도 가라앉는 연출 실행
					Floor->Multicast_StartSinking(); 
				}
			}
			*/
		}
		else if (CurrentPhase == EArenaPhase::FloorSinking)
		{
			GS->SetCurrentSinkingFloor(GS->GetCurrentSinkingFloor() + 1);
			if (GS->GetCurrentSinkingFloor() <= 2)
			{
				GS->SetCurrentPhase(EArenaPhase::Surviving);
				GS->SetRemainingTime(20);
			}
			else
			{
				GetWorldTimerManager().ClearTimer(ArenaTimerHandle);
			}
		}
	}
}
