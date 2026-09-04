// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/Lobby/LobbyPlayerController.h"

#include "EnhancedInputComponent.h"
#include "InputTriggers.h"
#include "LobbyGameMode.h"
#include "LobbyPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Project_Bang_Squad/UI/Lobby/JobSelectWidget.h"
#include "Project_Bang_Squad/UI/Lobby/LobbyMainWidget.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		if (LobbyMainWidgetClass)
		{
			LobbyMainWidget = CreateWidget<ULobbyMainWidget>(this, LobbyMainWidgetClass);
			if (LobbyMainWidget)
			{
				LobbyMainWidget->AddToViewport();
				RegisterManagedWidget(LobbyMainWidget);

				SetMenuState(true);
			}
		}
		GetWorld()->GetTimerManager().SetTimer(InitTimerHandle, this, &ALobbyPlayerController::InitLobbyUI, 0.2f, true);

		InitLobbyUI();

		if (UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance()))
		{
			if (!GI->GetUserNickname().IsEmpty())
			{
				ServerSetNickName(GI->GetUserNickname());
			}
		}
	}
}

void ALobbyPlayerController::RequestChangePreviewJob(EJobType NewJob)
{
	ServerPreviewJob(NewJob);
}

void ALobbyPlayerController::RequestToggleReady()
{
	ServerToggleReady();
}

void ALobbyPlayerController::RequestConfirmedJob(EJobType FinalJob)
{
	ServerConfirmedJob(FinalJob);
}

void ALobbyPlayerController::RefreshLobbyUI()
{
	if (!GetWorld() || GetWorld()->IsInSeamlessTravel())
	{
		return;
	}

	ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>();
	if (GS && GS->GetCurrentLobbyPhase() == ELobbyPhase::GameStarting)
		return;

	if (IsValid(LobbyMainWidget) && LobbyMainWidget->IsInViewport())
	{
		LobbyMainWidget->UpdatePlayerList();
	}

	if (IsValid(JobSelectWidget) && JobSelectWidget->IsInViewport())
	{
		JobSelectWidget->UpdateJobAvailability();
	}
}

void ALobbyPlayerController::ClientJobClaimResult_Implementation(EJobType RequestedJob, EJobClaimResult Result)
{
	UE_LOG(LogTemp, Log, TEXT("직업: %s, 원인: %s"), *UEnum::GetValueAsString(RequestedJob), *UEnum::GetValueAsString(Result));
	if (JobSelectWidget)
		JobSelectWidget->UpdateJobAvailability();
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//Timer 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InitTimerHandle);
	}
}

// void ALobbyPlayerController::ServerSetNickName_Implementation(const FString& NewName) 
// {
// 	// [수정] this->를 붙여서 명확하게 호출하거나, 상속받은 변수 PlayerState를 사용합니다.
// 	if (APlayerState* PS = this->GetPlayerState<APlayerState>())
// 	{
// 		PS->SetPlayerName(NewName);
// 		UE_LOG(LogTemp, Warning, TEXT("[Server] 닉네임 변경 완료: %s"), *NewName);
// 	}
// }

void ALobbyPlayerController::InitLobbyUI()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	ALobbyGameState* GS = World->GetGameState<ALobbyGameState>();
	if (!GS) return;

	ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>();
	if (!PS) return;

	World->GetTimerManager().ClearTimer(InitTimerHandle);
	GS->OnLobbyPhaseChanged.RemoveDynamic(this, &ALobbyPlayerController::HandleLobbyPhaseChanged);
	GS->OnLobbyPhaseChanged.AddDynamic(this, &ALobbyPlayerController::HandleLobbyPhaseChanged);
	HandleLobbyPhaseChanged(GS->GetCurrentLobbyPhase());
	RefreshLobbyUI();
}

void ALobbyPlayerController::HandleLobbyPhaseChanged(ELobbyPhase NewPhase)
{
	if (NewPhase == ELobbyPhase::PreviewJob)
	{
		if (JobSelectWidget)
			JobSelectWidget->SetVisibility(ESlateVisibility::Hidden);

		SetMenuState(true);
	}
	else if (NewPhase == ELobbyPhase::SelectJob)
	{
		if (LobbyMainWidget)
			LobbyMainWidget->SetVisibility(ESlateVisibility::Hidden);

		if (!JobSelectWidget && JobSelectWidgetClass)
		{
			JobSelectWidget = CreateWidget<UJobSelectWidget>(this, JobSelectWidgetClass);
		}

		if (JobSelectWidget)
		{
			JobSelectWidget->SetVisibility(ESlateVisibility::Visible);
			JobSelectWidget->StartUp();

			if (ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>())
				JobSelectWidget->UpdateJobAvailability();
		}
	}
	else if (NewPhase == ELobbyPhase::GameStarting)
	{
		// 기존 UI 숨기기
		if (JobSelectWidget) JobSelectWidget->SetVisibility(ESlateVisibility::Hidden);
		if (LobbyMainWidget) LobbyMainWidget->SetVisibility(ESlateVisibility::Hidden);

		SetMenuState(false);

		// 영상 위젯 생성 및 띄우기
		if (!VideoWidget && VideoWidgetClass)
		{
			VideoWidget = CreateWidget<UUserWidget>(this, VideoWidgetClass);
		}

		if (VideoWidget)
		{
			VideoWidget->AddToViewport(100);
			bHasVotedSkip = false; // 투표 상태 초기화
		}
	}
}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_ToggleLobbyMenu)
		{
			EIC->BindAction(IA_ToggleLobbyMenu, ETriggerEvent::Started, this, &ALobbyPlayerController::ToggleLobbyMenu);
		}
	}
}

void ALobbyPlayerController::ToggleLobbyMenu()
{
	SetMenuState(!bIsMenuVisible);
}

void ALobbyPlayerController::SetMenuState(bool bShow)
{
	bIsMenuVisible = bShow;

	//UI보이기 / 숨기기
	if (LobbyMainWidget)
	{
		LobbyMainWidget->SetVisibility(ESlateVisibility::Visible);
		LobbyMainWidget->SetMenuVisibility(bShow);
	}

	//입력 모드 전환
	if (bShow)
	{
		FInputModeGameAndUI InputMode;
		if (LobbyMainWidget)
			InputMode.SetWidgetToFocus(LobbyMainWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
		bShowMouseCursor = true;

		if (LobbyMainWidget)
			LobbyMainWidget->UpdatePlayerList();
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
}

void ALobbyPlayerController::DebugClaimJob(const FString& JobName)
{
#if UE_BUILD_SHIPPING
	return;
#endif
	EJobType RequestedJob = EJobType::None;

	if (JobName.Equals(TEXT("Titan"), ESearchCase::IgnoreCase))
	{
		RequestedJob = EJobType::Titan;
	}
	else if (JobName.Equals(TEXT("Striker"), ESearchCase::IgnoreCase))
	{
		RequestedJob = EJobType::Striker;
	}
	else if (JobName.Equals(TEXT("Mage"), ESearchCase::IgnoreCase))
	{
		RequestedJob = EJobType::Mage;
	}
	else if (JobName.Equals(TEXT("Paladin"), ESearchCase::IgnoreCase))
	{
		RequestedJob = EJobType::Paladin;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DebugClaim] Unknown job: %s"), *JobName);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[DebugClaim] Requesting job: %s"), *JobName);

	// UI 입력만 우회하고, 동일한 서버 직업 확정 경로를 호출한다.
	// 서버 승인 성공 시 ClientJobClaimResult를 통해 GameInstance도 갱신된다.
	ServerConfirmedJob(RequestedJob);
}

void ALobbyPlayerController::ServerPreviewJob_Implementation(EJobType NewJob)
{
	if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
	{
		GM->TryPreviewJob(this, NewJob);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "[LobbyAction] Action=PreviewJob Player=Unknown Phase=Unknown Result=Rejected Reason=MissingGameMode"
		       ));
	}
}

void ALobbyPlayerController::ServerToggleReady_Implementation()
{
	if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
	{
		GM->TryToggleReady(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "[LobbyAction] Action=ToggleReady Player=Unknown Phase=Unknown Result=Rejected Reason=MissingGameMode"
		       ));
	}
}

void ALobbyPlayerController::ServerConfirmedJob_Implementation(EJobType FinalJob)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		ClientJobClaimResult(FinalJob, EJobClaimResult::InternalError);
		return;
	}
	ALobbyGameMode* GM = World->GetAuthGameMode<ALobbyGameMode>();
	if (!GM)
	{
		ClientJobClaimResult(FinalJob, EJobClaimResult::InternalError);
		return;
	}
	ALobbyPlayerState* PS = GetPlayerState<ALobbyPlayerState>();

	EJobClaimResult JobClaimResult = GM->TryClaimJob(FinalJob, PS);
	ClientJobClaimResult(FinalJob, JobClaimResult);
}

void ALobbyPlayerController::RequestSkipVideo()
{
	ALobbyGameState* GS = GetWorld()->GetGameState<ALobbyGameState>();
	// 영상 재생 중일 때만 스킵 가능하게 방어
	if (GS && GS->GetCurrentLobbyPhase() == ELobbyPhase::GameStarting)
	{
		ServerRequestSkipVideo();
	}
}

void ALobbyPlayerController::ServerRequestSkipVideo_Implementation()
{
	if (bHasVotedSkip)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[LobbyAction] Action=SkipVideo Player=Unknown Phase=Unknown Result=Rejected Reason=AlreadyVoted"));
		return;
	}

	ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
	if (!GM)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[LobbyAction] Action=SkipVideo Player=Unknown Phase=Unknown Result=Rejected Reason=MissingGameMode"
		       ));
		return;
	}

	if (GM->TryRegisterSkipVote(this))
		bHasVotedSkip = true;
}
