// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	//캐릭터 교체
	void ChangePlayerCharacter(AController* Controller, EJobType NewJob);

	//Ready 체크
	void CheckAllReady();

	//직업 확정했는지 체크
	void CheckConfirmedJob();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	UPROPERTY()
	TSet<EJobType> ConfirmedJobs;

	bool TryConfirmJob(EJobType Job, class ALobbyPlayerState* RequestingPS);

protected:
	UPROPERTY(EditAnywhere, Category = "BS|Lobby")
	int32 PlayerCount = 4;
};
