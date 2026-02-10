// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Core/BSGameTypes.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakenJobsChanged, const TArray<EJobType>&, TakenJobs);

UCLASS()
class PROJECT_BANG_SQUAD_API ALobbyGameState : public ABSGameState
{
	GENERATED_BODY()

public:
	ALobbyGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//현재 로비 단계
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhase)
	ELobbyPhase CurrentPhase = ELobbyPhase::PreviewJob;

	UPROPERTY(BlueprintAssignable)
	FOnLobbyPhaseChanged OnLobbyPhaseChanged;

	void SetLobbyPhase(ELobbyPhase NewPhase);

	//직업이 선택 가능한지 확인
	bool IsJobAvailable(EJobType JobType) const ;

	//직업을 목록에 추가
	void AddTakenJob(EJobType JobType);
	
	//직업을 목록에서 제거
	void RemoveTakenJob(EJobType JobType);
	TArray<EJobType> GetTakenJobs() const { return TakenJobs; }

	UPROPERTY(BlueprintAssignable)
	FOnTakenJobsChanged OnTakenJobsChanged;
protected:
	UFUNCTION()
	void OnRep_CurrentPhase();

	UFUNCTION()
	void OnRep_TakenJobs();
	
	//현재 선점된 직업 목록
	UPROPERTY(ReplicatedUsing = OnRep_TakenJobs)
	TArray<EJobType> TakenJobs;

	bool TryAddTakenJob(EJobType JobType);
};
