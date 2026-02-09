// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"

#include "BSPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

void ABSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (UBSGameInstance* GI = GetGameInstance<UBSGameInstance>())
		{
			EJobType MyJob = GI->GetPlayerJob();

			if (ABSPlayerState* PS = GetPlayerState<ABSPlayerState>())
			{
				PS->Server_SetJob(MyJob);
			}
		}
	}
}

void ABSPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	// 레벨 이동이나 종료 시 관리하던 위젯들을 모두 제거
	for (UUserWidget* Widget : ManagedWidgets)
	{
		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}
	ManagedWidgets.Empty();

	
}

void ABSPlayerController::ServerSetNickName_Implementation(const FString& NewName)
{
	if (ABSPlayerState* PS = GetPlayerState<ABSPlayerState>())
	{
		PS->SetPlayerName(NewName);
	}
}

void ABSPlayerController::RegisterManagedWidget(UUserWidget* InWidget)
{
	if (InWidget)
		ManagedWidgets.Add(InWidget);
}

void ABSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &ABSPlayerController::MoveToStage1Map);
	InputComponent->BindKey(EKeys::F2, IE_Pressed, this, &ABSPlayerController::MoveToStage1MiniGameMap);
	InputComponent->BindKey(EKeys::F3, IE_Pressed, this, &ABSPlayerController::MoveToStage1BossMap);
	InputComponent->BindKey(EKeys::F4, IE_Pressed, this, &ABSPlayerController::MoveToStage2Map);
	InputComponent->BindKey(EKeys::F5, IE_Pressed, this, &ABSPlayerController::MoveToStage2MiniGameMap);
	InputComponent->BindKey(EKeys::F6, IE_Pressed, this, &ABSPlayerController::MoveToStage2BossMap);
	InputComponent->BindKey(EKeys::F7, IE_Pressed, this, &ABSPlayerController::MoveToStage3Map);
	InputComponent->BindKey(EKeys::F8, IE_Pressed, this, &ABSPlayerController::MoveToStage3MiniGameMap);
	InputComponent->BindKey(EKeys::F9, IE_Pressed, this, &ABSPlayerController::MoveToStage3BossMap);
}

void ABSPlayerController::MoveToStage1Map()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage1, EStageSection::Main);
}

void ABSPlayerController::MoveToStage1MiniGameMap()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage1, EStageSection::MiniGame);
}

void ABSPlayerController::MoveToStage1BossMap()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage1, EStageSection::Boss);
}

void ABSPlayerController::MoveToStage2Map()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage2, EStageSection::Main);
}

void ABSPlayerController::MoveToStage2MiniGameMap()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage2, EStageSection::MiniGame);
}

void ABSPlayerController::MoveToStage2BossMap()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage2, EStageSection::Boss);
}

void ABSPlayerController::MoveToStage3Map()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage3, EStageSection::Main);
}

void ABSPlayerController::MoveToStage3MiniGameMap()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage3, EStageSection::MiniGame);
}

void ABSPlayerController::MoveToStage3BossMap()
{
	ServerDebugMoveToMapHandle(EStageIndex::Stage3, EStageSection::Boss);
}

void ABSPlayerController::ServerDebugMoveToMapHandle_Implementation(EStageIndex Stage, EStageSection Section)
{
	if (UBSGameInstance* GI = GetGameInstance<UBSGameInstance>())
	{
		GI->MoveToStage(Stage, Section);
	}
}

