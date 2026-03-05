// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
#include "Project_Bang_Squad/Game/Base/BSGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API ALobbyGameMode : public ABSGameMode
{
	GENERATED_BODY()

public:
	ALobbyGameMode();

	/*//캐릭터 교체
	void ChangePlayerCharacter(AController* Controller, EJobType NewJob);*/

	//Ready 체크
	void CheckAllReady();

	//직업 확정했는지 체크
	void CheckConfirmedJob();
	
	UPROPERTY()
	TSet<EJobType> ConfirmedJobs;

	bool TryConfirmJob(EJobType Job, class ALobbyPlayerState* RequestingPS);

	// 클라이언트가 스킵을 눌렀을 때 호출할 함수
	void RegisterSkipVote();

protected:
	UPROPERTY(EditAnywhere, Category = "BS|Lobby")
	int32 PlayerCount = 4;

	// 영상 대기 타이머를 중간에 취소하기 위한 변수
	FTimerHandle VideoTravelTimerHandle;

	// 즉시 맵을 이동시키는 함수
	void ForceStartGame();
};
