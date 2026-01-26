// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGamePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/UI/MiniGame/MiniGameWidget.h"

void AMiniGamePlayerController::CreateGameWidget()
{
	if (MiniGameWidgetClass)
	{
		UMiniGameWidget* MiniGameWidget = CreateWidget<UMiniGameWidget>(this, MiniGameWidgetClass);
		if (MiniGameWidget)
		{
			MiniGameWidget->AddToViewport();
		}
	}
}
