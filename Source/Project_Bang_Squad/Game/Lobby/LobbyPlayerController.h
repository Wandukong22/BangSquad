// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyGameState.h"
#include "LobbyGameTypes.h"
#include "GameFramework/PlayerController.h"
#include "Project_Bang_Squad/Core/BSGameInstance.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"
#include "LobbyPlayerController.generated.h"

class ALobbyPlayerState;
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API ALobbyPlayerController : public ABSPlayerController
{
	GENERATED_BODY()

	TWeakObjectPtr<ALobbyGameState> CachedLobbyGameState;

public:
	//체험 직업 변경 요청
	UFUNCTION(BlueprintCallable)
	void RequestChangePreviewJob(EJobType NewJob);

	//준비
	UFUNCTION(BlueprintCallable)
	void RequestToggleReady();

	//직업 최종 확정
	UFUNCTION(BlueprintCallable)
	void RequestConfirmedJob(EJobType FinalJob);
	
	//UI 갱신 요청
	void RefreshLobbyUI();

	// 입력(ESC 등)이 들어왔을 때 호출할 함수
	UFUNCTION(BlueprintCallable)
	void RequestSkipVideo();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(Server, Reliable)
	void ServerPreviewJob(EJobType NewJob);

	UFUNCTION(Server, Reliable)
	void ServerToggleReady();

	UFUNCTION(Server, Reliable)
	void ServerConfirmedJob(EJobType FinalJob);

	UFUNCTION(Client, Reliable)
	void ClientJobClaimResult(EJobType RequestedJob, EJobClaimResult Result);
	
	UFUNCTION(Server, Reliable)
	void ServerRequestSkipVideo();

private:
	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UUserWidget> LobbyMainWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UUserWidget> JobSelectWidgetClass;
	UPROPERTY()
	class ULobbyMainWidget* LobbyMainWidget = nullptr;
	UPROPERTY()
	class UJobSelectWidget* JobSelectWidget = nullptr;

	//GameState 초기화 대기용
	void InitLobbyUI();
	FTimerHandle InitTimerHandle;

	UFUNCTION()
	void HandleLobbyPhaseChanged(ELobbyPhase NewPhase);

	// 영상 재생용 UI 위젯
	UPROPERTY(EditDefaultsOnly, Category = "BS|UI")
	TSubclassOf<UUserWidget> VideoWidgetClass;

	UPROPERTY()
	UUserWidget* VideoWidget = nullptr;

	// 한 명이 여러 번 투표하는 것 방지
	bool bHasVotedSkip = false;

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "BS|Input")
	UInputAction* IA_ToggleLobbyMenu;

private:
	bool bIsMenuVisible = true;

	UFUNCTION(BlueprintCallable)
	void ToggleLobbyMenu();
	void SetMenuState(bool bShow);

	//PlayerState Delegate 연결하는 전용 함수
	void RebindLobbyPlayerStateDelegates();
	UFUNCTION()
	void HandleLobbyRosterChanged();
	UFUNCTION()
	void HandleLobbyDataChanged();

	TArray<TWeakObjectPtr<ALobbyPlayerState>> BoundLobbyPlayerStates;

public:
	UFUNCTION(Exec)
	void DebugClaimJob(const FString& JobName);
};
