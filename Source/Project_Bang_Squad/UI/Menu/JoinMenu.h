// 

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Online/BSSessionTypes.h"
#include "Project_Bang_Squad/UI/Base/BaseMenu.h"
#include "JoinMenu.generated.h"

class UServerRow;
class UBSSessionSubsystem;
class UScrollBox;
class UButton;
class UEditableTextBox;

DECLARE_MULTICAST_DELEGATE(FOnJoinMenuBackRequested);

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UJoinMenu : public UBaseMenu
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> NickNameTextBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RefreshButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JoinButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ServerListScrollBox;

	UPROPERTY()
	TObjectPtr<UBSSessionSubsystem> SessionSubsystem;

	TOptional<FGuid> SelectedResultId;

protected:
	UPROPERTY(EditAnywhere, Category = "BS|Session")
	TSubclassOf<UServerRow> ServerRowClass;

public:
	FOnJoinMenuBackRequested OnJoinMenuBackRequested;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UFUNCTION()
	void HandleJoinButtonClicked();
	UFUNCTION()
	void HandleBackButtonClicked();
	UFUNCTION()
	void HandleRefreshButtonClicked();
	UFUNCTION()
	void HandleJoinInputChanged(const FText& Text);
	UFUNCTION()
	void HandleServerRowSelected(const FGuid& ResultId);
	void HandleFindSessionsSucceeded(const TArray<FBSSessionSummary>& SessionSummaries);
	void HandleSessionFailure(EBSSessionError Error);
	
	
	void UpdateJoinButtonState();

public:
	void RefreshSessions();
};
