// 


#include "HostMenu.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Online/BSSessionSubsystem.h"

bool UHostMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (HostButton) HostButton->OnClicked.AddDynamic(this, &UHostMenu::HandleHostButtonClicked);
	if (BackButton) BackButton->OnClicked.AddDynamic(this, &UHostMenu::HandleBackButtonClicked);
	if (IncreasePlayerButton) IncreasePlayerButton->OnClicked.AddDynamic(this, &UHostMenu::HandleIncreasePlayerButtonClicked);
	if (DecreasePlayerButton) DecreasePlayerButton->OnClicked.AddDynamic(this, &UHostMenu::HandleDecreasePlayerButtonClicked);
	if (RoomNameTextBox) RoomNameTextBox->OnTextChanged.AddDynamic(this, &UHostMenu::HandleHostInputChanged);
	if (NickNameTextBox) NickNameTextBox->OnTextChanged.AddDynamic(this, &UHostMenu::HandleHostInputChanged);

	UpdateMaxPlayersUI();
	UpdateHostButtonState();
	
	return true;
}

void UHostMenu::HandleHostButtonClicked()
{
	if (!RoomNameTextBox || !NickNameTextBox) return;
	const FString RoomName = RoomNameTextBox->GetText().ToString().TrimStartAndEnd();
	const FString NickName = NickNameTextBox->GetText().ToString().TrimStartAndEnd();
	if (RoomName.IsEmpty() || NickName.IsEmpty() || NickName.Contains(TEXT(" "))) return;
	
	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)) return;
	UBSSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UBSSessionSubsystem>();
	if (!IsValid(SessionSubsystem)) return;
	
	FBSCreateSessionRequest SessionRequest;
	SessionRequest.RoomName = RoomName;
	SessionRequest.HostName = NickName;
	SessionRequest.MaxPlayers = SelectedMaxPlayers;

	if (UBSGameInstance* GI = GetGameInstance<UBSGameInstance>())
	{
		GI->SetUserNickname(NickName);
	}
	
	SessionSubsystem->CreateSession(SessionRequest);
}

void UHostMenu::HandleBackButtonClicked()
{
	OnBackRequested.Broadcast();
}

void UHostMenu::HandleIncreasePlayerButtonClicked()
{
	SelectedMaxPlayers = FMath::Clamp(SelectedMaxPlayers + 1, 1, 4);
	UpdateMaxPlayersUI();
}

void UHostMenu::HandleDecreasePlayerButtonClicked()
{
	SelectedMaxPlayers = FMath::Clamp(SelectedMaxPlayers - 1, 1, 4);
	UpdateMaxPlayersUI();
}

void UHostMenu::UpdateMaxPlayersText()
{
	if (MaxPlayersText)
	{
		MaxPlayersText->SetText(FText::AsNumber(SelectedMaxPlayers));
	}
}

void UHostMenu::UpdateMaxPlayersUI()
{
	if (MaxPlayersText)
	{
		MaxPlayersText->SetText(FText::AsNumber(SelectedMaxPlayers));
	}

	if (DecreasePlayerButton)
	{
		DecreasePlayerButton->SetIsEnabled(SelectedMaxPlayers > 1);
	}

	if (IncreasePlayerButton)
	{
		IncreasePlayerButton->SetIsEnabled(SelectedMaxPlayers < 4);
	}
}

void UHostMenu::HandleHostInputChanged(const FText& Text)
{
	UpdateHostButtonState();
}

void UHostMenu::UpdateHostButtonState()
{
	if (!HostButton || !RoomNameTextBox || !NickNameTextBox) return;

	const FString RoomName = RoomNameTextBox->GetText().ToString().TrimStartAndEnd();
	const FString NickName = NickNameTextBox->GetText().ToString().TrimStartAndEnd();

	const bool bCanHost =
		!RoomName.IsEmpty() &&
		!NickName.IsEmpty() &&
		!NickName.Contains(TEXT(" "));

	HostButton->SetIsEnabled(bCanHost);
}
