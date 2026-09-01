// 

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Online/BSSessionTypes.h"
#include "Project_Bang_Squad/UI/Base/BaseMenu.h"
#include "InGameMenu.generated.h"

class UBSSessionSubsystem;
class UButton;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UInGameMenu : public UBaseMenu
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReturnToMainMenuButton;

	TWeakObjectPtr<UBSSessionSubsystem> SessionSubsystem;
	bool bWaitingForDestroy = false;

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandleContinueButtonClicked();
	UFUNCTION()
	void HandleReturnToMainMenuButtonClicked();
	UFUNCTION()
	void HandleSessionFailure(EBSSessionError Error);
};
