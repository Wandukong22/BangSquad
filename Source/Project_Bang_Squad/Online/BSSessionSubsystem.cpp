// 


#include "BSSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

static const FName SETTING_ROOM_NAME(TEXT("ROOM_NAME"));
static const FName SETTING_HOST_NAME(TEXT("HOST_NAME"));

void UBSSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	//현재 World의 Online Session 인터페이스 획득
	UWorld* World = GetWorld();
	if (!World) return;

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(World);
	if (!OnlineSubsystem) return;

	SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	//Online Session 비동기 작업 완료 델리게이트 등록

	//각 완료 콜백을 델리게이트로 만들고 Online Session 인터페이스에 등록
	//FOnCreateSessionCompleteDelegate: Unreal이 정의한 세션 생성 완료용 델리게이트 타입
	//CreateUObject: 현재 Subsystem의 멤버 콜백 함수를 델리게이트에 바인딩
	//this: UBSSessionSubsystem 인스턴스

	//AddOn...: 완성된 델리게이트를 완료 이벤트에 등록하고 해제용 핸들을 반환
	CreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		FOnCreateSessionCompleteDelegate::CreateUObject(
			this, &UBSSessionSubsystem::OnCreateSessionComplete));
	FindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		FOnFindSessionsCompleteDelegate::CreateUObject(
			this, &UBSSessionSubsystem::OnFindSessionsComplete));
	JoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(
			this, &UBSSessionSubsystem::OnJoinSessionComplete));
	DestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
		FOnDestroySessionCompleteDelegate::CreateUObject(
			this, &UBSSessionSubsystem::OnDestroySessionComplete));
}

void UBSSessionSubsystem::Deinitialize()
{
	if (SessionInterface.IsValid())
	{
		//저장해 둔 핸들을 사용해 완료 델리게이트 등록 해제
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);

		//세션 인터페이스 공유 참조 해제
		SessionInterface.Reset();
	}
	Super::Deinitialize();
}

void UBSSessionSubsystem::CreateSession(const FBSCreateSessionRequest& Request)
{
	constexpr int32 MinPlayers = 1;
	constexpr int32 MaxPlayersLimit = 4;
	const FString RoomName = Request.RoomName.TrimStartAndEnd();
	const FString HostName = Request.HostName.TrimStartAndEnd();

	//세션을 만들 수 있는 상태인지 검사
	if (!CanStartOperation(EBSSessionState::Creating))
	{
		if (CurrentState == EBSSessionState::InSession)
		{
			HandleFailure(
				EBSSessionError::AlreadyInSession,
				TEXT("이미 세션에 참가중입니다."),
				CurrentState);
		}
		else
		{
			HandleFailure(
				EBSSessionError::OperationAlreadyInProgress,
				TEXT("다른 세션 작업이 진행중입니다."),
				CurrentState);
		}
		return;
	}

	//생성 요청값 검사
	//TrimStartAndEnd: 공백 무시
	if (RoomName.IsEmpty() ||
		HostName.IsEmpty() ||
		Request.MaxPlayers < MinPlayers ||
		Request.MaxPlayers > MaxPlayersLimit)
	{
		HandleFailure(
			EBSSessionError::InvalidRequest,
			TEXT("세션 생성 요청값이 올바르지 않습니다."),
			CurrentState);
		return;
	}

	//세션 인터페이스 유효성 검사
	if (!IsValidSessionInterface()) return;


	// Standalone은 기본 NetDriver가 없으므로 이 상태에서 세션을 만들면
	// OnlineSubsystemNull이 접속 포트를 0으로 광고한다.
	// 세션 정보를 만들기 전에 Listen 서버를 열어 실제 포트를 등록한다.
	UWorld* World = GetWorld();
	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(World) || !IsValid(GameInstance))
	{
		HandleFailure(
			EBSSessionError::CreateFailed,
			TEXT("호스트 월드를 찾을 수 없습니다."),
			EBSSessionState::Idle);
		return;
	}

	/*if (World->GetNetMode() == NM_Standalone && !GameInstance->EnableListenServer(true))
	{
		HandleFailure(
			EBSSessionError::CreateFailed,
			TEXT("Listen 서버를 시작할 수 없습니다."),
			EBSSessionState::Idle);
		return;
	}*/

	//NetDriver 확인
	UNetDriver* NetDriver = World->GetNetDriver();
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BSSession][CreateSession][Host] NetMode=%d, NetDriver=%s"),
		static_cast<int32>(World->GetNetMode()),
		NetDriver ? *NetDriver->GetName() : TEXT("NULL")
	);

	if (NetDriver)
	{
		const TSharedPtr<const FInternetAddr> LocalAddr = NetDriver->GetLocalAddr();

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BSSession][Host] LocalAddr=%s"),
			LocalAddr.IsValid()
			? *LocalAddr->ToString(true)
			: TEXT("INVALID")
		);
	}

	//기존 세션이 있는지 확인
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		//Idle상태에서 기존 세션이 있다면 Destroy
		DestroySession();
		return;
	}

	//상태 변경
	SetState(EBSSessionState::Creating);

	//이전 오류 초기화
	LastError = EBSSessionError::None;
	LastErrorMessage.Empty();

	//세션 설정 생성
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = Request.MaxPlayers; //최대 공개 인원
	SessionSettings.bShouldAdvertise = true; //검색 목록에 노출
	SessionSettings.bAllowJoinInProgress = true; //진행 중 참가 허용
	SessionSettings.bUsesPresence = false;
	SessionSettings.bAllowJoinViaPresence = false;
	SessionSettings.bUseLobbiesIfAvailable = false;

	//커스텀 정보(방 이름, 호스트 이름)
	//ViaOnlineServiceAndPing: 커스텀 값을 세션 검색 결과에서도 가져올 수 있도록 광고하겠다는 뜻
	SessionSettings.Set(
		SETTING_ROOM_NAME,
		RoomName,
		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(
		SETTING_HOST_NAME,
		HostName,
		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(World);
	if (OnlineSubsystem)
	{
		const int32 Port = GetPortFromNetDriver(OnlineSubsystem->GetInstanceName());

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BSSession][CreateSession][DEBUG] Instance=%s, GetPortFromNetDriver=%d"),
			*OnlineSubsystem->GetInstanceName().ToString(),
			Port
		);
	}

	//실제 생성 요청
	if (!SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings))
	{
		//실패 시 Idle 복구
		HandleFailure(
			EBSSessionError::RequestNotStarted,
			TEXT("세션 생성을 실패했습니다."),
			EBSSessionState::Idle);
	}
}

void UBSSessionSubsystem::FindSessions()
{
	//현재 작업을 시작할 수 있는지 확인
	if (!CanStartOperation(EBSSessionState::Finding))
	{
		HandleFailure(
			EBSSessionError::OperationAlreadyInProgress,
			TEXT("다른 세션 작업이 진행중입니다."),
			CurrentState);
		return;
	}
	//SessionInterface 유효성 확인
	if (!IsValidSessionInterface()) return;

	SetState(EBSSessionState::Finding);

	//이전 검색 결과 무효화
	ResetSearchResultAndData();

	//검색 결과 새로 생성
	SessionSearch = MakeShared<FOnlineSessionSearch>();
	SessionSearch->MaxSearchResults = 100; //최대 검색 개수
	SessionSearch->bIsLanQuery = true; //LAN 검색 활성화

	if (!SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()))
	{
		//요청 시작 실패
		ResetSearchResultAndData();
		HandleFailure(
			EBSSessionError::FindFailed,
			TEXT("Find 실패"),
			EBSSessionState::Idle);
	}
}

void UBSSessionSubsystem::JoinSession(const FGuid& ResultId)
{
	if (!CanStartOperation(EBSSessionState::Joining))
	{
		HandleFailure(
			EBSSessionError::OperationAlreadyInProgress,
			TEXT("다른 세션 작업이 진행중입니다."),
			CurrentState);
		return;
	}
	if (!IsValidSessionInterface()) return;

	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		DestroySession();
		return;
	}

	//ResultId가 Map에 존재하는지 확인
	const int32* FoundIndex = SearchResultIndexMap.Find(ResultId);
	if (!FoundIndex)
	{
		HandleFailure(
			EBSSessionError::SearchResultExpired,
			TEXT("검색 결과에 해당 ResultId가 없습니다."),
			EBSSessionState::Idle);
		return;
	}

	if (!SessionSearch.IsValid() || !SessionSearch->SearchResults.IsValidIndex(*FoundIndex))
	{
		HandleFailure(
			EBSSessionError::SearchResultExpired,
			TEXT("검색 결과가 만료되었거나 유효하지 않습니다."),
			EBSSessionState::Idle);
		return;
	}
	SetState(EBSSessionState::Joining);

	const FOnlineSessionSearchResult& SearchResult = SessionSearch->SearchResults[*FoundIndex];
	if (!SessionInterface->JoinSession(0, NAME_GameSession, SearchResult))
	{
		HandleFailure(
			EBSSessionError::JoinFailed,
			TEXT("Join 요청 실패"),
			EBSSessionState::Idle);
	}
}

void UBSSessionSubsystem::DestroySession()
{
	//작업 가능한 상태인지 확인
	if (!CanStartOperation(EBSSessionState::Destroying))
	{
		HandleFailure(
			EBSSessionError::OperationAlreadyInProgress,
			TEXT("다른 세션 작업이 진행중입니다."),
			CurrentState);
		return;
	}

	//세션 인터페이스 유효성 검사
	if (!IsValidSessionInterface()) return;

	//Named Session이 존재하는지 확인
	if (SessionInterface->GetNamedSession(NAME_GameSession))
	{
		//존재할 경우
		StateBeforeDestroy = CurrentState;
		SetState(EBSSessionState::Destroying);
		if (!SessionInterface->DestroySession(NAME_GameSession)) //요청이 실패하면
		{
			HandleFailure(
				EBSSessionError::LeaveFailed,
				TEXT("세션 정리 실패"),
				StateBeforeDestroy);
		}
	}
	else
	{
		//존재하지 않을 경우 => 이미 나가있음
		//검색 결과 / 내부 데이터 정리
		ResetSearchResultAndData();

		SetState(EBSSessionState::Idle);
		OnBSDestroySessionSucceeded.Broadcast();
	}
}

int32 UBSSessionSubsystem::GetMaxPlayerNum() const
{
	if (!SessionInterface.IsValid()) return 0;
	FNamedOnlineSession* Session = SessionInterface->GetNamedSession(NAME_GameSession);
	if (!Session) return 0;

	return Session->SessionSettings.NumPublicConnections;
}

void UBSSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (CurrentState != EBSSessionState::Finding) return;
	if (!bWasSuccessful || !SessionSearch.IsValid())
	{
		ResetSearchResultAndData();
		HandleFailure(
			EBSSessionError::FindFailed,
			TEXT("Find 실패"),
			EBSSessionState::Idle);
		return;
	}

	const TArray<FOnlineSessionSearchResult>& SearchResults = SessionSearch->SearchResults;
	SessionSummaries.Reserve(SearchResults.Num());

	for (int32 Index = 0; Index < SearchResults.Num(); ++Index)
	{
		const FOnlineSessionSearchResult& SearchResult = SearchResults[Index];

		FBSSessionSummary Summary;
		Summary.ResultId = FGuid::NewGuid();
		SearchResult.Session.SessionSettings.Get(SETTING_ROOM_NAME, Summary.RoomName);
		SearchResult.Session.SessionSettings.Get(SETTING_HOST_NAME, Summary.HostName);
		Summary.MaxPlayerCount = SearchResult.Session.SessionSettings.NumPublicConnections;
		Summary.CurrentPlayerCount = Summary.MaxPlayerCount - SearchResult.Session.NumOpenPublicConnections;
		Summary.PingInMs = SearchResult.PingInMs;

		SearchResultIndexMap.Add(Summary.ResultId, Index);
		SessionSummaries.Add(MoveTemp(Summary));
	}
	SetState(EBSSessionState::Idle);

	OnBSFindSessionSucceeded.Broadcast(SessionSummaries);
}

void UBSSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionName != NAME_GameSession || CurrentState != EBSSessionState::Joining) return;
	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success: break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		HandleFailure(
			EBSSessionError::SessionIsFull,
			TEXT("세션 인원이 가득 찼습니다."),
			EBSSessionState::Idle);
		return;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		HandleFailure(
			EBSSessionError::SessionDoesNotExist,
			TEXT("세션이 존재하지 않습니다."),
			EBSSessionState::Idle);
		return;

	case EOnJoinSessionCompleteResult::AlreadyInSession:
		HandleFailure(
			EBSSessionError::AlreadyJoinedSession,
			TEXT("이미 해당 세션에 참가했습니다."),
			EBSSessionState::Idle);
		return;

	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		HandleFailure(
			EBSSessionError::CouldNotResolveConnectString,
			TEXT("접속 주소를 가져올 수 없습니다."),
			EBSSessionState::Idle);
		return;

	default:
		HandleFailure(
			EBSSessionError::JoinFailed,
			TEXT("세션 참가에 실패했습니다."),
			EBSSessionState::Idle);
		return;
	}
	//GetResolvedConnectString()로 접속 주소 획득
	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		HandleFailure(
			EBSSessionError::CouldNotResolveConnectString,
			TEXT("접속 주소를 가져올 수 없습니다."),
			EBSSessionState::Idle);
		DestroySession();
		return;
	}
	//InSession 상태 변경
	SetState(EBSSessionState::InSession);

	UE_LOG(LogTemp, Log, TEXT("[BSSession] 세션 참가 성공, ConnectString: %s"), *ConnectString);
	//접속 주소를 성공 이벤트로 전달
	OnBSJoinSessionSucceeded.Broadcast(ConnectString);
}

void UBSSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionName != NAME_GameSession || CurrentState != EBSSessionState::Destroying) return;
	if (bWasSuccessful)
	{
		//검색 결과 정리 & 내부 세션 관련 데이터 정리
		ResetSearchResultAndData();
		SetState(EBSSessionState::Idle);
		OnBSDestroySessionSucceeded.Broadcast();
	}
	else
	{
		HandleFailure(
			EBSSessionError::LeaveFailed,
			TEXT("세션 정리 실패"),
			StateBeforeDestroy);
	}
}

void UBSSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionName != NAME_GameSession || CurrentState != EBSSessionState::Creating) return;
	if (bWasSuccessful)
	{
		// Null Online Subsystem은 세션 생성 시 호스트의 공개 슬롯을
		// 자동으로 차감하지 않으므로 호스트를 명시적으로 등록
		if (FNamedOnlineSession* Session = SessionInterface->GetNamedSession(SessionName))
		{
			if (Session->OwningUserId.IsValid())
			{
				const bool bHostAlreadyRegistered = Session->RegisteredPlayers.ContainsByPredicate(
					[&Session](const FUniqueNetIdRef& PlayerId)
					{
						return *PlayerId == *Session->OwningUserId;
					});

				if (!bHostAlreadyRegistered)
				{
					SessionInterface->RegisterPlayer(SessionName, *Session->OwningUserId, false);
				}
			}
		}

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[BSSession] 세션 생성 성공: %s"),
			*SessionName.ToString());

		SetState(EBSSessionState::InSession);
		OnBSCreateSessionSucceeded.Broadcast();
	}
	else
	{
		HandleFailure(
			EBSSessionError::CreateFailed,
			TEXT("세션 생성을 실패했습니다."),
			EBSSessionState::Idle);
	}
}

void UBSSessionSubsystem::SetState(EBSSessionState NewState)
{
	if (CurrentState == NewState) return;
	const EBSSessionState PreviousState = CurrentState;
	CurrentState = NewState;

	OnBSSessionStateChanged.Broadcast(PreviousState, CurrentState);
}

bool UBSSessionSubsystem::CanStartOperation(EBSSessionState RequestedOperation) const
{
	switch (RequestedOperation)
	{
	case EBSSessionState::Creating:
	case EBSSessionState::Finding:
	case EBSSessionState::Joining:
		return CurrentState == EBSSessionState::Idle;
	case EBSSessionState::Destroying:
		return CurrentState == EBSSessionState::InSession || CurrentState == EBSSessionState::Idle;

	default:
		return false;
	}
}

void UBSSessionSubsystem::HandleFailure(EBSSessionError Error, const FString& Message, EBSSessionState RecoveryState)
{
	LastError = Error;
	LastErrorMessage = Message;

	UE_LOG(LogTemp, Error, TEXT("[BSSession] Error: %d, Message: %s"), static_cast<int32>(Error), *Message);
	SetState(RecoveryState);

	OnBSSessionFailure.Broadcast(Error);
}

void UBSSessionSubsystem::ResetSearchResultAndData()
{
	SessionSearch.Reset();
	SessionSummaries.Reset();
	SearchResultIndexMap.Reset();
}

bool UBSSessionSubsystem::IsValidSessionInterface()
{
	if (SessionInterface.IsValid()) return true;

	HandleFailure(
		EBSSessionError::SessionInterfaceUnavailable,
		TEXT("세션 인터페이스를 사용할 수 없습니다."),
		CurrentState);
	return false;
}
