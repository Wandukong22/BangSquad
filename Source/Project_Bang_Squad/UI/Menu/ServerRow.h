// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/Online/BSSessionTypes.h"
#include "ServerRow.generated.h"

class UBorder;
class UJoinMenu;
class UTextBlock;
class UButton;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnServerRowSelected, const FGuid&);

UCLASS()
class PROJECT_BANG_SQUAD_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RoomNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> HostNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PingText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RowButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SelectionBorder;

	FGuid ResultId;

	bool bSelected;

public:
	FOnServerRowSelected OnServerRowSelected;
	
protected:
	virtual void NativeConstruct() override;

public:
	void SetUp(const FBSSessionSummary& Summary);
	const FGuid& GetResultId() const { return ResultId; }
	void SetSelected(bool bInSelected);
private:
	UFUNCTION()
	void HandleRowButtonClicked();
};
