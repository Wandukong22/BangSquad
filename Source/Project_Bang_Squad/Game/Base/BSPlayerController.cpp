// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"

#include "Blueprint/UserWidget.h"

void ABSPlayerController::RegisterManagedWidget(UUserWidget* InWidget)
{
	if (InWidget)
		ManagedWidgets.Add(InWidget);
}

void ABSPlayerController::SeamlessTravelFrom(class APlayerController* OldPC)
{
	Super::SeamlessTravelFrom(OldPC);

	for (UUserWidget* Widget : ManagedWidgets)
	{
		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}
	ManagedWidgets.Empty();
}
