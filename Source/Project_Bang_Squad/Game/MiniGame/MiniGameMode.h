// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSGameMode.h"
#include "MiniGameMode.generated.h"

enum class EJobType : uint8;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AMiniGameMode : public ABSGameMode
{
	GENERATED_BODY()

public:
	AMiniGameMode();

	//접속 시 스폰 처리
	virtual void PostLogin(APlayerController* NewPlayer) override;

	//실제 소환 함수
	void SpawnPlayerCharacter(AController* Controller, EJobType JobType);

	//사망 처리
	void RequestRespawn(AController* DeadPlayer);

	//도착 처리
	void OnPlayerReachedGoal(AController* ReachedPlayer);
	

protected:
	//부활 함수(5초 뒤)
	void ExecuteRespawn(AController* Controller);

	//부활 위치 계산
	FTransform GetRespawnTransform(AController* Controller);

	//도착한 플레이어 목록
	UPROPERTY()
	TArray<AController*> FinishedPlayers;

	//모두 도착했는지 체크
	void CheckAllPlayersFinished();

	//UPROPERTY(EditAnywhere, Category = "BS|MiniGame")
	//float RespawnTime = 5.f;
};
