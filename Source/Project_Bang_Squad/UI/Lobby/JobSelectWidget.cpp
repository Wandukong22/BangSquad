// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Lobby/JobSelectWidget.h"

#include "Components/Button.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerController.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerState.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyGameState.h"
#include "Project_Bang_Squad/UI/PlayerRow.h"

void UJobSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_SelectTitan) Btn_SelectTitan->OnClicked.AddDynamic(this, &UJobSelectWidget::OnPickTitan);
	if (Btn_SelectStriker) Btn_SelectStriker->OnClicked.AddDynamic(this, &UJobSelectWidget::OnPickStriker);
	if (Btn_SelectMage) Btn_SelectMage->OnClicked.AddDynamic(this, &UJobSelectWidget::OnPickMage);
	if (Btn_SelectPaladin) Btn_SelectPaladin->OnClicked.AddDynamic(this, &UJobSelectWidget::OnPickPaladin);
	if (Btn_Confirm) Btn_Confirm->OnClicked.AddDynamic(this, &UJobSelectWidget::OnConfirmClicked);
}

void UJobSelectWidget::UpdateJobAvailAbility()
{
	ALobbyPlayerController* MyPC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (!MyPC) return;

	ALobbyPlayerState* MyPS = MyPC->GetPlayerState<ALobbyPlayerState>();

	ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>();
	if (!GS) return;

	TSet<EJobType> TakenJobs;

	for (APlayerState* PS : GS->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS && LobbyPS->bIsConfirmedJob)
		{
			TakenJobs.Add(LobbyPS->CurrentJob);
		}
	}

	if (Btn_SelectTitan) Btn_SelectTitan->SetIsEnabled(!TakenJobs.Contains(EJobType::Titan));
	if (Btn_SelectStriker) Btn_SelectStriker->SetIsEnabled(!TakenJobs.Contains(EJobType::Striker));
	if (Btn_SelectMage) Btn_SelectMage->SetIsEnabled(!TakenJobs.Contains(EJobType::Mage));
	if (Btn_SelectPaladin) Btn_SelectPaladin->SetIsEnabled(!TakenJobs.Contains(EJobType::Paladin));
}

void UJobSelectWidget::UpdatePlayerList()
{
	if (!PlayerListContainer || !PlayerRowClass) return;

	PlayerListContainer->ClearChildren();

	ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>();
	if (!GS) return;

#pragma region PlayerList Sorting
	//플레이어 순서가 네트워크 데이터 도착 순서대로 하고 있어서 뒤죽박죽 나옴
	//-> Sort 후 나타내기
	TArray<APlayerState*> SortedPlayers = GS->PlayerArray;

	SortedPlayers.Sort([](const APlayerState& A, const APlayerState& B)
	{
		return A.GetPlayerId() < B.GetPlayerId();
	});
	
#pragma endregion 
	//List 갱신
	for (APlayerState* PS : SortedPlayers)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS)
		{
			//위젯 생성
			UPlayerRow* Row = CreateWidget<UPlayerRow>(PlayerListContainer, PlayerRowClass);
			if (Row)
			{
				Row->SetWidgetMode(ERowMode::Lobby);
				Row->SetTargetPlayerState(LobbyPS);
				Row->UpdateLobbyInfo(LobbyPS->bIsReady, LobbyPS->CurrentJob);
				PlayerListContainer->AddChild(Row);
			}
		}
	}
}

void UJobSelectWidget::OnPickTitan()
{
	PendingJob = EJobType::Titan;
}

void UJobSelectWidget::OnPickStriker()
{
	PendingJob = EJobType::Striker;
}

void UJobSelectWidget::OnPickMage()
{
	PendingJob = EJobType::Mage;
}

void UJobSelectWidget::OnPickPaladin()
{
	PendingJob = EJobType::Paladin;
}

void UJobSelectWidget::OnConfirmClicked()
{
	if (PendingJob == EJobType::None) return;

	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->RequestConfirmedJob(PendingJob);
	}
}
