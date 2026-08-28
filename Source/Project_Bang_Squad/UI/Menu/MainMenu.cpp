// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/UI/Menu/MainMenu.h"

#include "HostMenu.h"
#include "JoinMenu.h"
#include "ServerRow.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Online/BSSessionSubsystem.h"

bool UMainMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	if (HostButton) HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);
	if (JoinButton) JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	if (IsValid(HostMenu)) HostMenu->OnBackRequested.AddUObject(this, &UMainMenu::OpenMainMenu);
	if (IsValid(JoinMenu)) JoinMenu->OnJoinMenuBackRequested.AddUObject(this, &UMainMenu::OpenMainMenu);
	if (QuitButton) QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

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

void UMainMenu::OpenMainMenu()
{
	if (!IsValid(MenuSwitcher) || !IsValid(MainMenu)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenHostMenu()
{
	if (!IsValid(MenuSwitcher) || !IsValid(HostMenu)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::OpenJoinMenu()
{
	if (!IsValid(MenuSwitcher) || !IsValid(JoinMenu)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
	JoinMenu->RefreshSessions();
}

void UMainMenu::QuitGame()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	PC->ConsoleCommand("quit");
}
