#include "Project_Bang_Squad/Character/StageBoss/StageBossGameMode.h"
#include "Project_Bang_Squad/Character/StageBoss/Stage1Boss.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AStageBossGameMode::AStageBossGameMode()
{
	// 기본값 설정
}

void AStageBossGameMode::BeginPlay()
{
	Super::BeginPlay();
}

// --- [QTE 로직] ---

void AStageBossGameMode::TriggerSpearQTE(AStage1Boss* BossActor)
{
	// 권한 확인 및 유효성 검사
	if (!HasAuthority() || !IsValid(BossActor)) return;

	CurrentBoss = BossActor;
	AccumulatedInputCount = 0;

	// 타이머 시작 (Tick 사용 안함)
	GetWorldTimerManager().SetTimer(QTETimerHandle, this, &AStageBossGameMode::OnQTETimeout, QTEDuration, false);
	UE_LOG(LogTemp, Warning, TEXT("[BossGameMode] QTE Started!"));
}

void AStageBossGameMode::ProcessQTEInput(AController* PlayerController)
{
	if (!GetWorldTimerManager().IsTimerActive(QTETimerHandle)) return;

	AccumulatedInputCount++;

	// 목표치 달성 시 성공 처리
	if (AccumulatedInputCount >= GoalQTECount)
	{
		EndSpearQTE(true);
	}
}

void AStageBossGameMode::OnQTETimeout()
{
	EndSpearQTE(false);
}

void AStageBossGameMode::EndSpearQTE(bool bSuccess)
{
	GetWorldTimerManager().ClearTimer(QTETimerHandle);

	if (IsValid(CurrentBoss))
	{
		// 보스에게 결과 전달 (보스 클래스에 해당 함수가 없다면 주석 처리하거나 추가 구현 필요)
		// CurrentBoss->OnQTEFinished(bSuccess);
	}

	UE_LOG(LogTemp, Warning, TEXT("QTE Finished. Success: %d"), bSuccess);
}

// --- [부활 및 전멸 시스템] ---

void AStageBossGameMode::OnPlayerDied(AController* DeadController)
{
	// RequestRespawn으로 로직 일원화
	RequestRespawn(DeadController);
}

void AStageBossGameMode::RequestRespawn(AController* Controller)
{
	if (!Controller) return;

	// 팀 목숨 차감 로직
	if (MaxTeamLives > 0)
	{
		MaxTeamLives--;
		UE_LOG(LogTemp, Warning, TEXT("Team Life Used. Remaining: %d"), MaxTeamLives);

		// [중요] 부모 클래스(AStageGameMode)의 리스폰 로직(위치 계산, 스폰 등)을 그대로 사용
		Super::RequestRespawn(Controller);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Lives Left. Checking Wipe..."));
		CheckPartyWipe();
	}
}

void AStageBossGameMode::CheckPartyWipe()
{
	bool bAnyAlive = false;

	// 서버에 있는 모든 플레이어 컨트롤러 검사
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			// 폰이 살아있다면 전멸 아님
			if (PC->GetPawn())
			{
				bAnyAlive = true;
				break;
			}
		}
	}

	if (!bAnyAlive)
	{
		EndStage(false); // 패배
	}
}

void AStageBossGameMode::OnBossKilled()
{
	EndStage(true); // 승리
}

void AStageBossGameMode::EndStage(bool bIsVictory)
{
	if (bIsVictory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss Cleared! Victory!"));
		// 승리 시 부모의 스테이지 이동 로직 활용 가능
		// ClearStageAndMove(EStageIndex::Stage2); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Mission Failed."));
		// 패배 처리 (게임 오버 UI 등)
	}
}