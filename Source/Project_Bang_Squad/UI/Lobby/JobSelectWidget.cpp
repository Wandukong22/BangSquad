// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Lobby/JobSelectWidget.h"

#include "Components/Button.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerController.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerState.h"

void UJobSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("=== JobSelectWidget 초기화 ==="));
	UE_LOG(LogTemp, Warning, TEXT("Btn_SelectTitan: %s"), Btn_SelectTitan ? TEXT("✅") : TEXT("❌"));
	UE_LOG(LogTemp, Warning, TEXT("Btn_SelectStriker: %s"), Btn_SelectStriker ? TEXT("✅") : TEXT("❌"));
	UE_LOG(LogTemp, Warning, TEXT("Btn_SelectMage: %s"), Btn_SelectMage ? TEXT("✅") : TEXT("❌"));
	UE_LOG(LogTemp, Warning, TEXT("Btn_SelectPaladin: %s"), Btn_SelectPaladin ? TEXT("✅") : TEXT("❌"));


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

	UpdateJobAvailAbility();
}

void UJobSelectWidget::UpdateJobAvailAbility()
{
	ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>();
	if (!GS) return;

	TSet<EJobType> TakenJobs;
	for (APlayerState* PS : GS->PlayerArray)
	{
		ALobbyPlayerState* LobbyPS = Cast<ALobbyPlayerState>(PS);
		if (LobbyPS && LobbyPS->GetIsConfirmedJob() && PS != GetOwningPlayerState())
		{
			TakenJobs.Add(LobbyPS->GetJob());
		}
	}

	auto UpdateButtonState = [&](UJobButton* Btn, EJobType JobType)
	{
		if (!Btn) return;

		bool bIsTaken = TakenJobs.Contains(JobType);
		Btn->SetIsEnabled(!bIsTaken);

		if (bIsTaken)
		{
			Btn->SetSelectedState(false);
			if (PendingJob == JobType)
			{
				PendingJob = EJobType::None;
			}
		}
		else
		{
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
	UpdateJobAvailAbility();
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
