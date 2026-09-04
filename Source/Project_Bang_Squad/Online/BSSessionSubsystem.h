// 

#pragma once

#include "CoreMinimal.h"
#include "BSSessionTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSSessionSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBSSessionStateChanged, EBSSessionState, EBSSessionState);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBSSessionFailure,EBSSessionError);
DECLARE_MULTICAST_DELEGATE(FOnBSCreateSessionSucceeded);
DECLARE_MULTICAST_DELEGATE(FOnBSDestroySessionSucceeded);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBSFindSessionSuccedeed, const TArray<FBSSessionSummary>&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBSJoinSessionSucceeded, const FString&);
/**
 * 
 */
UCLASS()
class PROJECT_BANG_SQUAD_API UBSSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	//OnlineSubsystem이 제공하는 세션 인터페이스
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	TArray<FBSSessionSummary> SessionSummaries;
	
	//UI가 보낸 ResultId로 실제 SearchResult를 찾아 참가하기 위한 용도
	TMap<FGuid, int32> SearchResultIndexMap;

	//Online Session 완료 델리게이트 등록 해제용 핸들
	FDelegateHandle CreateSessionCompleteHandle;
	FDelegateHandle FindSessionsCompleteHandle;
	FDelegateHandle JoinSessionCompleteHandle;
	FDelegateHandle DestroySessionCompleteHandle;

	EBSSessionState CurrentState = EBSSessionState::Idle;
	EBSSessionState StateBeforeDestroy = EBSSessionState::Idle;
	EBSSessionError LastError = EBSSessionError::None;
	FString LastErrorMessage;

public:
	FOnBSSessionStateChanged OnBSSessionStateChanged;
	FOnBSSessionFailure OnBSSessionFailure;
	FOnBSCreateSessionSucceeded OnBSCreateSessionSucceeded;
	FOnBSDestroySessionSucceeded OnBSDestroySessionSucceeded;
	FOnBSFindSessionSuccedeed OnBSFindSessionSucceeded;
	FOnBSJoinSessionSucceeded OnBSJoinSessionSucceeded;
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	void CreateSession(const FBSCreateSessionRequest& Request);
	void FindSessions();
	void JoinSession(const FGuid& ResultId);
	void DestroySession();

	int32 GetMaxPlayerNum() const;

private:
	//Online Session 비동기 작업 완료 콜백
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(
		FName SessionName,
		EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	//현재 세션 작업 상태 변경
	void SetState(EBSSessionState NewState);
	//현재 상태에서 지정한 작업을 시작할 수 있는지 검사
	bool CanStartOperation(EBSSessionState RequestedOperation) const;
	//실패 정보 전달 및 안정 상태로 복구
	void HandleFailure(
		EBSSessionError Error,
		const FString& Message,
		EBSSessionState RecoveryState
		);

	//검색 결과 & 내부 데이터 정리 함수
	void ResetSearchResultAndData();
	//세션 인터페이스 유효성 검사 함수
	bool IsValidSessionInterface();
};
