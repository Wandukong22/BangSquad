// 

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/UI/Base/BaseMenu.h"
#include "HostMenu.generated.h"

class UTextBlock;
class UButton;
class USpinBox;
class UEditableTextBox;

DECLARE_MULTICAST_DELEGATE(FOnHostMenuBackRequested);
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UHostMenu : public UBaseMenu
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> RoomNameTextBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> NickNameTextBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxPlayersText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> IncreasePlayerButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DecreasePlayerButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> HostButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	int32 SelectedMaxPlayers = 4;
public:
	FOnHostMenuBackRequested OnBackRequested;

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void HandleHostButtonClicked();
	UFUNCTION()
	void HandleBackButtonClicked();
	UFUNCTION()
	void HandleIncreasePlayerButtonClicked();
	UFUNCTION()
	void HandleDecreasePlayerButtonClicked();
	UFUNCTION()
	void UpdateMaxPlayersText();
	UFUNCTION()
	void UpdateMaxPlayersUI();
	UFUNCTION()
	void HandleHostInputChanged(const FText& Text);
	UFUNCTION()
	void UpdateHostButtonState();
};
