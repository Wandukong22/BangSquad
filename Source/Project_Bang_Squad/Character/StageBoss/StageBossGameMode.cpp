#include "Project_Bang_Squad/Character/StageBoss/StageBossGameMode.h"

#include "StageBossPlayerState.h"
#include "Project_Bang_Squad/Character/StageBoss/Stage1Boss.h"
#include "Project_Bang_Squad/Core/TrueDamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

AStageBossGameMode::AStageBossGameMode()
{
	// �⺻�� ����
}

void AStageBossGameMode::BeginPlay()
{
	Super::BeginPlay();
}

// --- [QTE ����] ---

void AStageBossGameMode::TriggerSpearQTE(AStage1Boss* BossActor)
{
	if (!HasAuthority() || !IsValid(BossActor)) return;

	CurrentBoss = BossActor;
	AccumulatedInputCount = 0;

	// [�ű�] ������ �ʱ�ȭ
	PlayerTapCounts.Empty();

	CurrentBoss->PlayQTEVisuals(QTEDuration);

	GetWorldTimerManager().SetTimer(QTETimerHandle, this, &AStageBossGameMode::OnQTETimeout, QTEDuration, false);
	UE_LOG(LogTemp, Warning, TEXT("[BossGameMode] QTE Started!"));
}
void AStageBossGameMode::ProcessQTEInput(AController* PlayerController)
{
	if (!GetWorldTimerManager().IsTimerActive(QTETimerHandle)) return;

	// 1. ��ü ī��Ʈ ����
	AccumulatedInputCount++;

	// 2. [�ű�] ���� ��� ����
	if (PlayerController)
	{
		if (AStageBossPlayerState* PS = Cast<AStageBossPlayerState>(PlayerController->PlayerState))
		{
			PS->AddQTECount();
		}
		/*if (PlayerTapCounts.Contains(PlayerController))
		{
			PlayerTapCounts[PlayerController]++;
		}
		else
		{
			PlayerTapCounts.Add(PlayerController, 1);
		}*/
	}

	// ��ǥġ �޼�
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

	// --- [�ű�] ��� ������ ���� (TMap -> TArray) ---
	TArray<FPlayerQTEStat> FinalStats;
	
	// �ʿ� ��ϵ� �÷��̾�� �߰�
	for (auto& Pair : PlayerTapCounts)
	{
		FPlayerQTEStat Stat;
		// PlayerState�� ��ȿ�ϸ� �г��� ���, ������ �⺻��
		if (Pair.Key && Pair.Key->PlayerState)
		{
			Stat.PlayerName = Pair.Key->PlayerState->GetPlayerName();
		}
		else
		{
			Stat.PlayerName = TEXT("Unknown Player");
		}
		Stat.TapCount = Pair.Value;
		FinalStats.Add(Stat);
	}

	// --- ���� ó�� ---
	if (bSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("QTE Success! Stage Clear!"));
		
		// 1. �������� ���� ���� (��� ��Ÿ�� ���)
		if (IsValid(CurrentBoss))
		{
			CurrentBoss->HandleQTEResult(true);
		}

		// 2. Ŭ���� UI ȣ�� (BP���� ����)
		ShowGameClearUI(FinalStats);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("QTE Failed! Retry Required."));

		// 1. �������� ���� ���� (����ų� ������ ����)
		if (IsValid(CurrentBoss))
		{
			CurrentBoss->HandleQTEResult(false);
		}
		
		// 2. ��Ʈ���� UI ȣ�� (BP���� ����)
		ShowRetryUI(FinalStats);
	}
}

// --- [��Ȱ �� ���� �ý���] ---

void AStageBossGameMode::OnPlayerDied(AController* DeadController)
{
	// RequestRespawn���� ���� �Ͽ�ȭ
	RequestRespawn(DeadController);
}

void AStageBossGameMode::RequestRespawn(AController* Controller)
{
	if (!Controller) return;

	// �� ��� ���� ����
	if (MaxTeamLives > 0)
	{
		MaxTeamLives--;
		UE_LOG(LogTemp, Warning, TEXT("Team Life Used. Remaining: %d"), MaxTeamLives);

		// [�߿�] �θ� Ŭ����(AStageGameMode)�� ������ ����(��ġ ���, ���� ��)�� �״�� ���
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

	// ������ �ִ� ��� �÷��̾� ��Ʈ�ѷ� �˻�
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			// ���� ����ִٸ� ���� �ƴ�
			if (PC->GetPawn())
			{
				bAnyAlive = true;
				break;
			}
		}
	}

	if (!bAnyAlive)
	{
		EndStage(false); // �й�
	}
}

void AStageBossGameMode::OnBossKilled()
{
	EndStage(true); // �¸�
}

void AStageBossGameMode::DebugStartQTE()
{
	// 월드에서 보스 액터를 찾습니다.
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AStage1Boss::StaticClass());
	if (AStage1Boss* Boss = Cast<AStage1Boss>(FoundActor))
	{
		// 기존에 만들어진 QTE 실행 함수 호출
		TriggerSpearQTE(Boss);
	}
}

void AStageBossGameMode::EndStage(bool bIsVictory)
{
	if (bIsVictory)
	{
		UE_LOG(LogTemp, Warning, TEXT("Boss Cleared! Victory!"));
		// �¸� �� �θ��� �������� �̵� ���� Ȱ�� ����
		// ClearStageAndMove(EStageIndex::Stage2); 
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Mission Failed."));
		// �й� ó�� (���� ���� UI ��)
	}
}
