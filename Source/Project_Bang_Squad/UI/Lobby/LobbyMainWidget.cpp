// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyMainWidget.h"

#include "Components/Button.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerController.h"
#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerState.h"
#include "Project_Bang_Squad/UI/Stage/PlayerRow.h"

void ULobbyMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_SelectTitan) Btn_SelectTitan->OnJobSelected.AddDynamic(this, &ULobbyMainWidget::OnJobButtonClicked);
	if (Btn_SelectStriker) Btn_SelectStriker->OnJobSelected.AddDynamic(this, &ULobbyMainWidget::OnJobButtonClicked);
	if (Btn_SelectMage) Btn_SelectMage->OnJobSelected.AddDynamic(this, &ULobbyMainWidget::OnJobButtonClicked);
	if (Btn_SelectPaladin) Btn_SelectPaladin->OnJobSelected.AddDynamic(this, &ULobbyMainWidget::OnJobButtonClicked);
	if (Btn_Ready) Btn_Ready->OnClicked.AddDynamic(this, &ULobbyMainWidget::OnBtn_ReadyClicked);
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
	UpdateJobButtonStates();
}

void ULobbyMainWidget::SetMenuVisibility(bool bVisible)
{
	if (MenuControlArea)
	{
		MenuControlArea->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void ULobbyMainWidget::OnBtn_ReadyClicked()
{
	if (auto PC = GetLobbyPC())
	{
		PC->RequestToggleReady();
	}
}

void ULobbyMainWidget::OnJobButtonClicked(EJobType SelectedJob)
{
	if (auto PC = GetLobbyPC())
		PC->RequestChangePreviewJob(SelectedJob);}

void ULobbyMainWidget::UpdateJobButtonStates()
{
	// 1. 내 플레이어 컨트롤러 & 스테이트 가져오기
	ALobbyPlayerController* PC = GetLobbyPC();
	if (!PC) return;
    
	ALobbyPlayerState* MyPS = PC->GetPlayerState<ALobbyPlayerState>();
	if (!MyPS) return;

	EJobType MyCurrentJob = MyPS->CurrentJob;

	// 2. 람다 함수로 중복 제거 (버튼이 유효한지 체크 후 상태 설정)
	auto SetButtonState = [&](UJobButton* Btn, EJobType TargetJob)
	{
		if (Btn)
		{
			// 내 직업과 버튼의 직업이 같으면 True(켜짐), 다르면 False(꺼짐)
			Btn->SetSelectedState(MyCurrentJob == TargetJob);
		}
	};

	// 3. 각 버튼에 적용
	SetButtonState(Btn_SelectTitan, EJobType::Titan);
	SetButtonState(Btn_SelectStriker, EJobType::Striker);
	SetButtonState(Btn_SelectMage, EJobType::Mage);
	SetButtonState(Btn_SelectPaladin, EJobType::Paladin);
}

class ALobbyPlayerController* ULobbyMainWidget::GetLobbyPC()
{
	return Cast<ALobbyPlayerController>(GetOwningPlayer());
}
