// 


#include "InGameMenu.h"

#include "Components/Button.h"
#include "Project_Bang_Squad/Online/BSSessionSubsystem.h"

bool UInGameMenu::Initialize()
{
	if (!Super::Initialize()) return false;

	if (ContinueButton) ContinueButton->OnClicked.AddDynamic(this, &UInGameMenu::HandleContinueButtonClicked);
	if (ReturnToMainMenuButton) ReturnToMainMenuButton->OnClicked.AddDynamic(this, &UInGameMenu::HandleReturnToMainMenuButtonClicked);
	
	return true;
}

void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)) return;
	SessionSubsystem = GameInstance->GetSubsystem<UBSSessionSubsystem>();
	if (!SessionSubsystem.IsValid()) return;

	SessionSubsystem->OnBSSessionFailure.RemoveAll(this);
	SessionSubsystem->OnBSSessionFailure.AddUObject(this, &UInGameMenu::HandleSessionFailure);
}

void UInGameMenu::NativeDestruct()
{
	if (SessionSubsystem.IsValid())
		SessionSubsystem->OnBSSessionFailure.RemoveAll(this);
	
	Super::NativeDestruct();
}

void UInGameMenu::HandleContinueButtonClicked()
{
	HideWidget();
}

void UInGameMenu::HandleReturnToMainMenuButtonClicked()
{
	if (!SessionSubsystem.IsValid() || bWaitingForDestroy) return;
	bWaitingForDestroy = true;

	if (ReturnToMainMenuButton)
		ReturnToMainMenuButton->SetIsEnabled(false);
	
	SessionSubsystem->DestroySession();
}

void UInGameMenu::HandleSessionFailure(EBSSessionError Error)
{
	if (!bWaitingForDestroy) return;

	bWaitingForDestroy = false;
	if (ReturnToMainMenuButton)
		ReturnToMainMenuButton->SetIsEnabled(true);
}
