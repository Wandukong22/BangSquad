// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"

#include "Blueprint/UserWidget.h"

void ABSPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 레벨 이동이나 종료 시 관리하던 위젯들을 모두 제거
	for (UUserWidget* Widget : ManagedWidgets)
	{
		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}
	ManagedWidgets.Empty();
}

void ABSPlayerController::RegisterManagedWidget(UUserWidget* InWidget)
{
	if (InWidget)
		ManagedWidgets.Add(InWidget);
}
