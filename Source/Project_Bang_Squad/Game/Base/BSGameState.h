// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "BSGameState.generated.h"

class ABSPlayerState;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API ABSGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:
	TArray<ABSPlayerState*> GetAllBSPlayerStates() const;

	//살아있는 플레이어 수 확인
	UFUNCTION()
	int32 GetAlivePlayerCount() const;

	//특정 직업을 가진 플레이어 찾기
	ABSPlayerState* FindPlayerByJob(EJobType Job) const;
};
