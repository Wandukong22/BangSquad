// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/MenuPlayerController.h"

#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/UI/Menu/MainMenu.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalPlayerController() || !IsValid(MainMenuWidgetClass)) return;
	
	MainMenuWidget = CreateWidget<UMainMenu>(this, MainMenuWidgetClass);
	if (!IsValid(MainMenuWidget)) return;

	MainMenuWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}
