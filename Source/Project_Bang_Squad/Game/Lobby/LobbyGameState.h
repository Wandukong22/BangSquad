// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSGameState.h"
#include "LobbyGameState.generated.h"

UENUM(BlueprintType)
enum class ELobbyPhase : uint8
{
	PreviewJob,
	SelectJob,
	GameStarting //맵 이동 때 UI 프리즈 하기 위해
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyPhaseChanged, ELobbyPhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkipVoteChanged, int32, CurrentVotes, int32, TotalVotes);

UCLASS()
class PROJECT_BANG_SQUAD_API ALobbyGameState : public ABSGameState
{
	GENERATED_BODY()
	
	//현재 로비 단계
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase)
	ELobbyPhase CurrentLobbyPhase = ELobbyPhase::PreviewJob;

public:
	ALobbyGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ELobbyPhase GetCurrentLobbyPhase() const {return CurrentLobbyPhase;}
	void SetCurrentLobbyPhase(ELobbyPhase NewPhase);

	UPROPERTY(BlueprintAssignable)
	FOnLobbyPhaseChanged OnLobbyPhaseChanged;

	// 현재 스킵을 누른 인원 수
	UPROPERTY(ReplicatedUsing = OnRep_SkipVoteCount, BlueprintReadOnly)
	int32 SkipVoteCount = 0;

	UPROPERTY(BlueprintAssignable)
	FOnSkipVoteChanged OnSkipVoteChanged;

	UFUNCTION()
	void OnRep_SkipVoteCount();

protected:
	UFUNCTION()
	void OnRep_CurrentPhase();
};
