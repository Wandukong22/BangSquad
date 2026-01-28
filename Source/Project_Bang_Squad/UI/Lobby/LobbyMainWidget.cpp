// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyMainWidget.h"

#include "Components/Button.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerController.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerState.h"
#include "Project_Bang_Squad/UI/Stage/PlayerRow.h"

void ULobbyMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_SelectTitan) Btn_SelectTitan->OnClicked.AddDynamic(this, &ULobbyMainWidget::OnBtn_SelectTitanClicked);
	if (Btn_SelectStriker) Btn_SelectStriker->OnClicked.AddDynamic(this, &ULobbyMainWidget::OnBtn_SelectStrikerClicked);
	if (Btn_SelectMage) Btn_SelectMage->OnClicked.AddDynamic(this, &ULobbyMainWidget::OnBtn_SelectMageClicked);
	if (Btn_SelectPaladin) Btn_SelectPaladin->OnClicked.AddDynamic(this, &ULobbyMainWidget::OnBtn_SelectPaladinClicked);
	if (Btn_Ready) Btn_Ready->OnClicked.AddDynamic(this, &ULobbyMainWidget::OnBtn_ReadyClicked);

	SetJobButtonIcon(Btn_SelectTitan, EJobType::Titan);
	SetJobButtonIcon(Btn_SelectStriker, EJobType::Striker);
	SetJobButtonIcon(Btn_SelectMage, EJobType::Mage);
	SetJobButtonIcon(Btn_SelectPaladin, EJobType::Paladin);
}

void ULobbyMainWidget::UpdatePlayerList()
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

	if (ALobbyPlayerController* PC = GetLobbyPC())
	{
		if (ALobbyPlayerState* LobbyPS = PC->GetPlayerState<ALobbyPlayerState>())
		{
			if (Txt_ReadyState)
			{
				if (LobbyPS->bIsReady)
				{
					Txt_ReadyState->SetText(FText::FromString("READY"));
					Txt_ReadyState->SetColorAndOpacity(FLinearColor::Green);
				}
				else
				{
					Txt_ReadyState->SetText(FText::FromString(TEXT("NOT READY")));
					Txt_ReadyState->SetColorAndOpacity(FLinearColor::Red);
				}
			}
		}
	}
}

void ULobbyMainWidget::SetMenuVisibility(bool bVisible)
{
	if (MenuControlArea)
	{
		MenuControlArea->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void ULobbyMainWidget::OnBtn_SelectTitanClicked()
{
	SelectJobClickedHelper(EJobType::Titan);
}

void ULobbyMainWidget::OnBtn_SelectStrikerClicked()
{
	SelectJobClickedHelper(EJobType::Striker);
}

void ULobbyMainWidget::OnBtn_SelectMageClicked()
{
	SelectJobClickedHelper(EJobType::Mage);
}

void ULobbyMainWidget::OnBtn_SelectPaladinClicked()
{
	SelectJobClickedHelper(EJobType::Paladin);
}

void ULobbyMainWidget::OnBtn_ReadyClicked()
{
	if (auto PC = GetLobbyPC())
	{
		PC->RequestToggleReady();
	}
}

void ULobbyMainWidget::SelectJobClickedHelper(EJobType Job)
{
	if (auto PC = GetLobbyPC())
		PC->RequestChangePreviewJob(Job);
}

void ULobbyMainWidget::SetJobButtonIcon(UButton* Btn, EJobType Job)
{
	if (!Btn) return;

	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	if (!GI) return;

	UTexture2D* Icon = GI->GetJobIcon(Job);
	if (!Icon) return;

	FButtonStyle NewStyle = Btn->WidgetStyle;

	FSlateBrush Brush;
	Brush.SetResourceObject(Icon);
	Brush.DrawAs = ESlateBrushDrawType::Image;

	NewStyle.SetNormal(Brush);
	NewStyle.SetHovered(Brush);
	NewStyle.SetPressed(Brush);

	Btn->SetStyle(NewStyle);
}

class ALobbyPlayerController* ULobbyMainWidget::GetLobbyPC()
{
	return Cast<ALobbyPlayerController>(GetOwningPlayer());
}
