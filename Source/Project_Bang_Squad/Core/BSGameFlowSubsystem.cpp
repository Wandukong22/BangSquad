// 


#include "BSGameFlowSubsystem.h"

#include "BSGameInstance.h"
#include "BSGameSettings.h"
#include "Project_Bang_Squad/Data/DataAsset/BSMapData.h"
#include "Project_Bang_Squad/Game/Base/BSPlayerController.h"
#include "Project_Bang_Squad/Online/BSSessionSubsystem.h"

void UBSGameFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	//의존성 명시
	Collection.InitializeDependency<UBSSessionSubsystem>();

	//MapData Settings에서 가져오기
	const UBSGameSettings* Settings = GetDefault<UBSGameSettings>();
	if (!Settings) return;
	MapData = Settings->MapData.LoadSynchronous();
	
	//세션 델리게이트 등록
	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)) return;
	
	SessionSubsystem = GameInstance->GetSubsystem<UBSSessionSubsystem>();
	if (!IsValid(SessionSubsystem)) return;

	//생성 성공 이벤트 구독
	SessionSubsystem->OnBSCreateSessionSucceeded.AddUObject(this, &UBSGameFlowSubsystem::HandleCreateSessionSucceeded);
	//Join 성공 이벤트 구독
	SessionSubsystem->OnBSJoinSessionSucceeded.AddUObject(this, &UBSGameFlowSubsystem::HandleJoinSessionSucceeded);
	//Leave 성공 이벤트 구독
	SessionSubsystem->OnBSLeaveSessionSucceeded.AddUObject(this, &UBSGameFlowSubsystem::HandleLeaveSessionSucceeded);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBSGameFlowSubsystem::HandlePostLoadMap);

	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UBSGameFlowSubsystem::HandleNetworkFailure);
		GEngine->OnTravelFailure().AddUObject(this, &UBSGameFlowSubsystem::HandleTravelFailure);
	}
}

void UBSGameFlowSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SessionSubsystem->OnBSCreateSessionSucceeded.RemoveAll(this);
	SessionSubsystem->OnBSJoinSessionSucceeded.RemoveAll(this);
	SessionSubsystem->OnBSLeaveSessionSucceeded.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

void UBSGameFlowSubsystem::HandleCreateSessionSucceeded()
{
	HostTravelToLobby();
}

void UBSGameFlowSubsystem::HandleJoinSessionSucceeded(const FString& Address)
{
	ClientTravelToAddress(Address);
}

void UBSGameFlowSubsystem::HandleLeaveSessionSucceeded()
{
	ClientTravelToMainMenu();
}

void UBSGameFlowSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	bIsTraveling = false;
}

void UBSGameFlowSubsystem::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	bIsTraveling = false;

	UE_LOG(LogTemp, Error,
		TEXT("Network Failure: %s"), *ErrorString);

	if (IsValid(SessionSubsystem))
	{
		SessionSubsystem->LeaveSession();
	}
	else
	{
		ClientTravelToMainMenu();
	}
}

void UBSGameFlowSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType,
	const FString& ErrorString)
{
	bIsTraveling = false;

	UE_LOG(LogTemp, Error,
		TEXT("Travel Failure: %s"), *ErrorString);

	if (IsValid(SessionSubsystem))
	{
		SessionSubsystem->LeaveSession();
	}
	else
	{
		ClientTravelToMainMenu();
	}
}

void UBSGameFlowSubsystem::ServerTravelToStage(EStageIndex StageIndex, EStageSection Section)
{
	if (bIsTraveling) return;
	
	//서버에서 호출됐는지 확인
	UWorld* World = GetWorld();
	if (!World || World->GetNetMode() == ENetMode::NM_Client) return;

	//MapData에서 StageIndex, Section에 맞는 맵 경로 조회
	if (!IsValid(MapData)) return;
	const FString MapPath = MapData->GetMapPath(StageIndex, Section);
	if (MapPath.IsEmpty()) return;

	//로딩 UI 표시
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ABSPlayerController* PC = Cast<ABSPlayerController>(It->Get()))
		{
			PC->Client_ShowLoadingScreen(StageIndex, Section);
		}
	}
	if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
	{
		if (GI->GetCurrentStage() != StageIndex)
		{
			GI->SetCurrentStage(StageIndex);
			GI->InitSavedCheckpointIndex();
		}
	}
	bIsTraveling = true;
	
	//맵 이동
	if (!World->ServerTravel(MapPath + "?listen")) bIsTraveling = false;
	
}

void UBSGameFlowSubsystem::HostTravelToLobby()
{
	//이동중인지 확인
	if (bIsTraveling) return;

	//월드 유효성 검사
	//클라이언트에서 호출된 거 아닌지 검사
	UWorld* World = GetWorld();
	if (!IsValid(World) || World->GetNetMode() == NM_Client) return;

	if (!IsValid(MapData)) return;
	//로비 경로가 비어있지 않은지 검사

	const FString LobbyPath = MapData->GetMapPath(EStageIndex::Lobby, EStageSection::Main);
	if (LobbyPath.IsEmpty()) return;

	bIsTraveling = true;

	const FString TravelURL = LobbyPath + TEXT("?listen");
	if (!World->ServerTravel(TravelURL)) bIsTraveling = false;
}

void UBSGameFlowSubsystem::ClientTravelToAddress(const FString& Address)
{
	if (bIsTraveling) return;
	if (Address.IsEmpty()) return;

	UGameInstance* GameInstance = GetGameInstance();
	if (!IsValid(GameInstance)) return;

	APlayerController* PlayerController = GameInstance->GetFirstLocalPlayerController();
	if (!IsValid(PlayerController)) return;

	bIsTraveling = true;

	PlayerController->ClientTravel(Address, TRAVEL_Absolute);
}

void UBSGameFlowSubsystem::ClientTravelToMainMenu()
{
	if (!IsValid(MapData)) return;

	const FString MenuPath =
		MapData->GetMapPath(EStageIndex::Lobby, EStageSection::Menu);

	if (MenuPath.IsEmpty()) return;

	if (APlayerController* PC =
		GetGameInstance()->GetFirstLocalPlayerController())
	{
		PC->ClientTravel(MenuPath, TRAVEL_Absolute);
	}
}
