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

	if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
	{
		GS->OnTakenJobsChanged.RemoveDynamic(this, &UJobSelectWidget::HandleTakenJobsChanged);
		GS->OnTakenJobsChanged.AddDynamic(this, &UJobSelectWidget::HandleTakenJobsChanged);

		UpdateJobAvailability(GS->GetTakenJobs());
	}
}

void UJobSelectWidget::UpdateJobAvailability(const TArray<EJobType>& TakenJobs)
{
	EJobType MyConfirmedJob = EJobType::None;
	if (ALobbyPlayerState* PS = GetOwningPlayer()->GetPlayerState<ALobbyPlayerState>())
	{
		if (PS->GetIsConfirmedJob())
		{
			MyConfirmedJob = PS->GetJob();
		}
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

	if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
		UpdateJobAvailability(GS->GetTakenJobs());
}

void UJobSelectWidget::OnConfirmClicked()
{
	if (PendingJob == EJobType::None) return;

	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		ConfirmJob = PendingJob;
		PC->RequestConfirmedJob(PendingJob);
	}
}

void UJobSelectWidget::HandleTakenJobsChanged(const TArray<EJobType>& NewTakenJobs)
{
	UpdateJobAvailability(NewTakenJobs);
}
