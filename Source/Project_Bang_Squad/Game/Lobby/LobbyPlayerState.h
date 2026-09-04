// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerState.h"
#include "LobbyPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyDataChanged);

UCLASS()
class PROJECT_BANG_SQUAD_API ALobbyPlayerState : public ABSPlayerState
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_LobbyData)
	EJobType PreviewJob = EJobType::None;

	//준비 완료 여부
	UPROPERTY(ReplicatedUsing = OnRep_LobbyData)
	bool bIsReady = false;
	
public:
	ALobbyPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	EJobType GetPreviewJob() const { return PreviewJob; }
	void SetPreviewJob(EJobType NewJob);
	
	UPROPERTY(BlueprintAssignable)
	FOnLobbyDataChanged OnLobbyDataChanged;

	virtual void SetJob(EJobType NewJob) override;

	void SetIsReady(bool NewIsReady);
	bool GetIsReady() const { return bIsReady; }

protected:
	UFUNCTION()
	void OnRep_LobbyData();

	//PlayerName이 변경될 때 호출되는 함수
	//닉네임이 서버로부터 복제되어 내 컴퓨터에 도착했을 때 실행
	virtual void OnRep_PlayerName() override;

	virtual void OnRep_JobType() override;

private:
	void NotifyLobbyDataChanged();
};
