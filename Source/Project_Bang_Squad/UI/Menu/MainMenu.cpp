// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Menu/MainMenu.h"

#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Project_Bang_Squad/UI/Menu/ServerRow.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"

UMainMenu::UMainMenu()
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowClass_Asset(TEXT("/Game/TeamShare/UI/WBP_ServerRow"));
	if (ServerRowClass_Asset.Succeeded())
		ServerRowClass = ServerRowClass_Asset.Class;
}

bool UMainMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	// --- 디버깅 로그 시작 ---
	if (ConfirmHostButton == nullptr)
	{
		// 🔴 빨간색 경고: 버튼 연결 실패
		UE_LOG(LogTemp, Error, TEXT("CRITICAL: 'ConfirmHostButton' 찾을 수 없음! 위젯 블루프린트 이름 확인 필수!"));
	}
	else
	{
		// 🟢 초록색 알림: 버튼 연결 성공
		UE_LOG(LogTemp, Warning, TEXT("SUCCESS: 'ConfirmHostButton' 연결 완료. 클릭 이벤트 등록 중..."));
		ConfirmHostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	}

	if (HostButton) HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
	if (JoinButton) JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	if (CancelHostButton) CancelHostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	if (CancelJoinButton) CancelJoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);
	if (ConfirmJoinButton) ConfirmJoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
	
	if (ConfirmJoinButton)
	{
		ConfirmJoinButton->SetIsEnabled(false);
	}
	if (GuestNameInput)
	{
		GuestNameInput->OnTextChanged.AddDynamic(this, &UMainMenu::OnGuestNameChanged);
	}

	return true;
}

void UMainMenu::SetButtonColorState(class UButton* InButton, bool bIsSelected)
{
	if (!InButton) return;

	// 색상은 여기서 한 번만 정의하면 됩니다! (하늘색 / 회색)
	const FLinearColor SelectedColor = FLinearColor(0.0f, 0.5f, 1.0f, 1.0f);
	const FLinearColor UnselectedColor = FLinearColor(0.3f, 0.3f, 0.3f, 1.0f);

	// 선택 여부에 따라 색상 적용
	InButton->SetBackgroundColor(bIsSelected ? SelectedColor : UnselectedColor);
}

void UMainMenu::SetServerList(TArray<FServerData> InServerData)
{
	UWorld* World = GetWorld();
	if (!World) return;

	if (!ServerList) return;
	ServerList->ClearChildren();

	// 1. 데이터 0개면 종료
	if (InServerData.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 0개 발견. 목록 갱신 종료."));
		return;
	}

	uint32 i = 0;
	for (const FServerData& ServerData : InServerData)
	{
		UServerRow* ServerRow = CreateWidget<UServerRow>(World, ServerRowClass);

		if (!ServerRow) continue;
		
		if (ServerRow->ServerName == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("ServerRow's ServerName is null."));
			return;
		}

		ServerRow->ServerName->SetText(FText::FromString(ServerData.Name));
		ServerRow->HostUser->SetText(FText::FromString(ServerData.HostUserName));

		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);
		ServerRow->ConnectionFraction->SetText(FText::FromString(FractionText));

		ServerRow->SetUp(this, i++);
		ServerList->AddChild(ServerRow);
	}
}

void UMainMenu::OpenMainMenu()
{
	if (!MenuSwitcher) return;
	if (!MainMenu) return;

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenHostMenu()
{
	if (!MenuSwitcher) return;
	if (!HostMenu) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::OpenJoinMenu()
{
	if (!MenuSwitcher) return;
	if (!JoinMenu) return;

	SelectedIndex.Reset();
	UpdateJoinButtonState();

	if (ConfirmJoinButton)
	{
		ConfirmJoinButton->SetIsEnabled(false);
	}

	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (OwningInstance)
		OwningInstance->RefreshServerList();
}

void UMainMenu::HostServer()
{
	// 1. 클릭 로그
	UE_LOG(LogTemp, Warning, TEXT("BUTTON CLICKED: HostServer 함수 진입 성공!"));

	// 2. 안전장치 (인스턴스 및 방 제목 입력칸 확인)
	if (OwningInstance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: OwningInstance is NULL"));
		return;
	}

	if (ServerHostName == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: ServerHostName is NULL"));
		return;
	}

	// 3. 방 이름 가져오기
	FString ServerName = ServerHostName->GetText().ToString();
	FString HostName = OwnerNameInput->GetText().ToString();
	if (HostName.IsEmpty()) HostName = TEXT("Unknown Host");

	if (OwningInstance)
	{
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(OwningInstance))
		{
			GI->UserNickname = HostName;
		}
	}
	
	OwningInstance->Host(ServerName, MaxPlayers, HostName);
}

void UMainMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	PC->ConsoleCommand("quit");
}

void UMainMenu::JoinServer()
{
	// 1. 화면 체크 (기존 코드)
	if (MenuSwitcher && MenuSwitcher->GetActiveWidget() != JoinMenu)
	{
		return;
	}

	// ✨✨ [추가] 최종 방어: "입장 버튼이 잠겨있으면(회색) 절대 실행하지 마!"
	// (이게 있으면 그 어떤 귀신이 와도 강제로 입장을 못 시킵니다)
	if (ConfirmJoinButton && !ConfirmJoinButton->GetIsEnabled())
	{
		UE_LOG(LogTemp, Warning, TEXT("⛔ [차단] 입장 버튼이 비활성화 상태입니다. 입장을 거부합니다."));
		return;
	}

	// 2. 선택된 방 없으면 무시
	if (!SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Warning, TEXT("선택된 방이 없습니다."));
		return;
	}

	if (OwningInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index is %d"), SelectedIndex.GetValue());
		if (UBSGameInstance* GI = Cast<UBSGameInstance>(OwningInstance))
		{
			FString GuestName = GuestNameInput->GetText().ToString();
			if (GuestName.IsEmpty()) GuestName = TEXT("Guest");

			GI->UserNickname = GuestName;
		}
		
		OwningInstance->Join(SelectedIndex.GetValue());
	}
}

void UMainMenu::SetSelectedIndex(uint32 InIndex)
{
	SelectedIndex = InIndex;
	UpdateJoinButtonState();

	//UpdateJoinButtonState();

	// (기존 색상 변경 코드 유지)
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		auto serverRow = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (serverRow)
		{
			serverRow->bSelected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
			serverRow->UpdateColor();
		}
	}
}

void UMainMenu::OnGuestNameChanged(const FText& Text)
{
	UpdateJoinButtonState();
}

void UMainMenu::UpdateJoinButtonState()
{
	if (!ConfirmJoinButton) return;

	// 1. 방을 선택했는가?
	bool bRoomSelected = SelectedIndex.IsSet();

	// 2. 닉네임이 비어있지 않은가?
	bool bNameEntered = (GuestNameInput && !GuestNameInput->GetText().IsEmpty());

	// 둘 다 OK여야 버튼 활성화
	ConfirmJoinButton->SetIsEnabled(bRoomSelected && bNameEntered);
}
