// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Lobby/JobSelectWidget.h"

#include "Components/Button.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerController.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerState.h"

void UJobSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_SelectTitan)
	{
		Btn_SelectTitan->OnJobSelected.AddDynamic(this, &UJobSelectWidget::OnJobButtonClicked);
		UE_LOG(LogTemp, Warning, TEXT("Titan AssignedJob: %d"), (int32)Btn_SelectTitan->AssignedJob);
	}
	if (Btn_SelectStriker)
	{
		Btn_SelectStriker->OnJobSelected.AddDynamic(this, &UJobSelectWidget::OnJobButtonClicked);
		UE_LOG(LogTemp, Warning, TEXT("Titan AssignedJob: %d"), (int32)Btn_SelectStriker->AssignedJob);
	}
	if (Btn_SelectMage)
	{
		Btn_SelectMage->OnJobSelected.AddDynamic(this, &UJobSelectWidget::OnJobButtonClicked);
		UE_LOG(LogTemp, Warning, TEXT("Titan AssignedJob: %d"), (int32)Btn_SelectMage->AssignedJob);

	}
	if (Btn_SelectPaladin)
	{
		Btn_SelectPaladin->OnJobSelected.AddDynamic(this, &UJobSelectWidget::OnJobButtonClicked);
		UE_LOG(LogTemp, Warning, TEXT("Titan AssignedJob: %d"), (int32)Btn_SelectPaladin->AssignedJob);

	}
	if (Btn_Confirm) Btn_Confirm->OnClicked.AddDynamic(this, &UJobSelectWidget::OnConfirmClicked);


		UpdateJobAvailability();
}

void UJobSelectWidget::UpdateJobAvailability()
{
	//LobbyGameState 가져오기
	UWorld* World = GetWorld();
	if (!World) return;
	ALobbyGameState* GS = World->GetGameState<ALobbyGameState>();
	if (!GS) return;
	//LobbyPlayerState 가져오기
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;
	ALobbyPlayerState* PS = PC->GetPlayerState<ALobbyPlayerState>();
	if (!PS) return;

	TSet<EJobType> TakenJobs;
	EJobType MyConfirmedJob = PS->GetJob();

	for (APlayerState* PlayerState : GS->PlayerArray)
	{
		ALobbyPlayerState* LobbyPlayerState = Cast<ALobbyPlayerState>(PlayerState);
		if (!LobbyPlayerState) continue;
		
		EJobType Job = LobbyPlayerState->GetJob();
		if (Job != EJobType::None)
			TakenJobs.Add(Job);
	}
	
	auto UpdateButtonState = [&](UJobButton* Btn, EJobType JobType)
	{
		if (!Btn) return;

		bool bIsTaken = TakenJobs.Contains(JobType);
		bool bIsMyJob = (MyConfirmedJob == JobType);
		
		Btn->SetIsEnabled(!bIsTaken);

		if (bIsTaken)
		{
			if (PendingJob == JobType)
			{
				PendingJob = EJobType::None;
			}
			Btn->SetSelectedState(bIsMyJob);
		}
		else
		{
			//하이라이트
			Btn->SetSelectedState(PendingJob == JobType);
		}
	};

	UpdateButtonState(Btn_SelectTitan, EJobType::Titan);
	UpdateButtonState(Btn_SelectStriker, EJobType::Striker);
	UpdateButtonState(Btn_SelectMage, EJobType::Mage);
	UpdateButtonState(Btn_SelectPaladin, EJobType::Paladin);

	if (Btn_Confirm)
	{
		bool bCanConfirm = (PendingJob != EJobType::None) && !TakenJobs.Contains(PendingJob);
		Btn_Confirm->SetIsEnabled(bCanConfirm);
	}
}

void UJobSelectWidget::OnJobButtonClicked(EJobType SelectedJob)
{
	PendingJob = SelectedJob;

	UpdateJobAvailability();
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
