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

	InputComponent->BindKey(EKeys::F1, IE_Pressed, this, &ABSPlayerController::ServerDebugMoveToStage1Boss);
}

void ABSPlayerController::ServerDebugMoveToStage1Boss_Implementation()
{
	if (UBSGameInstance* GI = GetGameInstance<UBSGameInstance>())
	{
		GI->MoveToStage(EStageIndex::Stage1, EStageSection::Boss);
	}
}
