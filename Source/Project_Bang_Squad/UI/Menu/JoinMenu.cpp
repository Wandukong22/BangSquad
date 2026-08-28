// 


#include "JoinMenu.h"

#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Online/BSSessionSubsystem.h"

void UJoinMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (JoinButton) JoinButton->OnClicked.AddDynamic(this, &UJoinMenu::HandleJoinButtonClicked);
	if (BackButton) BackButton->OnClicked.AddDynamic(this, &UJoinMenu::HandleBackButtonClicked);
	if (RefreshButton) RefreshButton->OnClicked.AddDynamic(this, &UJoinMenu::HandleRefreshButtonClicked);
	if (NickNameTextBox) NickNameTextBox->OnTextChanged.AddDynamic(this, &UJoinMenu::HandleJoinInputChanged);

	UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
		SessionSubsystem = GameInstance->GetSubsystem<UBSSessionSubsystem>();

	if (IsValid(SessionSubsystem))
	{
		SessionSubsystem->OnBSFindSessionSucceeded.AddUObject(this, &UJoinMenu::HandleFindSessionsSucceeded);
		SessionSubsystem->OnBSSessionFailure.AddUObject(this, &UJoinMenu::HandleSessionFailure);
	}
	
	UpdateJoinButtonState();
}

void UJoinMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(SessionSubsystem))
	{
		SessionSubsystem->OnBSFindSessionSucceeded.RemoveAll(this);
		SessionSubsystem->OnBSSessionFailure.RemoveAll(this);
	}
}

void UJoinMenu::HandleJoinButtonClicked()
{
	if (!NickNameTextBox) return;
	const FString NickName = NickNameTextBox->GetText().ToString().TrimStartAndEnd();
	if (NickName.IsEmpty() || NickName.Contains(TEXT(" "))) return;

	if (!JoinButton || !JoinButton->GetIsEnabled()) return;
	if (!SelectedResultId.IsSet()) return;

	if (!IsValid(SessionSubsystem)) return;

	if (UBSGameInstance* GI = GetGameInstance<UBSGameInstance>())
	{
		GI->SetUserNickname(NickName);
	}
	
	SessionSubsystem->JoinSession(SelectedResultId.GetValue());
}

void UJoinMenu::HandleBackButtonClicked()
{
	SelectedResultId.Reset();

	if (ServerListScrollBox) ServerListScrollBox->ClearChildren();
	
	UpdateJoinButtonState();
	
	OnJoinMenuBackRequested.Broadcast();
}

void UJoinMenu::HandleRefreshButtonClicked()
{
	RefreshSessions();
}

void UJoinMenu::HandleJoinInputChanged(const FText& Text)
{
	UpdateJoinButtonState();
}

void UJoinMenu::HandleServerRowSelected(const FGuid& ResultId)
{
	//Guid 저장
	SelectedResultId = ResultId;
	//선택된 행 표시 갱신
	if (ServerListScrollBox)
	{
		const int32 RowCount = ServerListScrollBox->GetChildrenCount();
		for (int32 Index = 0; Index < RowCount; ++Index)
		{
			UServerRow* ServerRow = Cast<UServerRow>(ServerListScrollBox->GetChildAt(Index));
			if (!ServerRow) continue;

			const bool bIsSelected = ServerRow->GetResultId() == ResultId;
			ServerRow->SetSelected(bIsSelected);
		}
	}
	//Join 버튼 상태 갱신
	UpdateJoinButtonState();
}

void UJoinMenu::HandleFindSessionsSucceeded(const TArray<FBSSessionSummary>& SessionSummaries)
{
	if (!ServerListScrollBox || !ServerRowClass) return;
	
	ServerListScrollBox->ClearChildren(); //기존 ScrollBox 목록 초기화
	ServerListScrollBox->ScrollToStart(); //갱신한 뒤 스크롤 위치 초기화
	SelectedResultId.Reset(); //Guid 초기화

	//검색 결과마다 ServerRow 생성
	for (const FBSSessionSummary& Summary : SessionSummaries)
	{
		//검색 결과마다 ServerRow 생성
		UServerRow* ServerRow = CreateWidget<UServerRow>(GetWorld(), ServerRowClass);
		if (!ServerRow) continue;

		//행의 정보를 채움
		ServerRow->SetUp(Summary);

		//행 선택 이벤트 구독
		ServerRow->OnServerRowSelected.AddUObject(this, &UJoinMenu::HandleServerRowSelected);

		//ScrollBox에 행 추가
		ServerListScrollBox->AddChild(ServerRow);
	}
	UpdateJoinButtonState();
}

void UJoinMenu::HandleSessionFailure(EBSSessionError Error)
{
	UE_LOG(
	LogTemp,
	Error,
	TEXT("세션 작업 실패: %d"),
	static_cast<int32>(Error));

	if (RefreshButton) RefreshButton->SetIsEnabled(true);
}

void UJoinMenu::UpdateJoinButtonState()
{
	if (!JoinButton || !NickNameTextBox) return;

	const FString NickName = NickNameTextBox->GetText().ToString().TrimStartAndEnd();
	const bool bCanJoin = !NickName.IsEmpty() && !NickName.Contains(TEXT(" ")) && SelectedResultId.IsSet();
	JoinButton->SetIsEnabled(bCanJoin);
}

void UJoinMenu::RefreshSessions()
{
	if (!IsValid(SessionSubsystem)) return;

	SelectedResultId.Reset();
	if (ServerListScrollBox)
		ServerListScrollBox->ClearChildren();
	UpdateJoinButtonState();
	SessionSubsystem->FindSessions();
}
