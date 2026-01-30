#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Stage/StageGameMode.h" // 부모 클래스 헤더
#include "StageBossGameMode.generated.h"

class AStage1Boss;

/**
 * AStageBossGameMode
 * - 부모(StageGameMode)의 리스폰 로직 상속 (virtual RequestRespawn)
 * - 보스전 전용 QTE 및 팀 목숨 시스템 추가
 */
UCLASS()
class PROJECT_BANG_SQUAD_API AStageBossGameMode : public AStageGameMode
{
	GENERATED_BODY()

public:
	AStageBossGameMode();

	virtual void BeginPlay() override;

	// --- [1] QTE 시스템 (서버 전용) ---
	void TriggerSpearQTE(AStage1Boss* BossActor);
	void ProcessQTEInput(AController* PlayerController);

	// --- [2] 리스폰 규칙 재정의 ---
	// 부모인 StageGameMode.h에서 반드시 virtual을 붙여야 에러가 사라집니다.
	virtual void RequestRespawn(AController* Controller);
	// 기존 코드와의 호환성을 위해 추가 (내부에서 RequestRespawn 호출)
	UFUNCTION(BlueprintCallable, Category = "BS|BossRule")
	void OnPlayerDied(AController* DeadController);

	// 보스 처치 시 호출
	void OnBossKilled();

protected:
	// 내부 로직
	void EndSpearQTE(bool bSuccess);
	void OnQTETimeout();
	void CheckPartyWipe();
	void EndStage(bool bIsVictory);

protected:
	UPROPERTY()
	TObjectPtr<AStage1Boss> CurrentBoss;

	// QTE 변수
	int32 AccumulatedInputCount = 0;
	FTimerHandle QTETimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "BS|BossRule")
	int32 GoalQTECount = 40;

	UPROPERTY(EditDefaultsOnly, Category = "BS|BossRule")
	float QTEDuration = 10.0f;

	// 보스전 팀 공유 목숨
	UPROPERTY(EditDefaultsOnly, Category = "BS|BossRule")
	int32 MaxTeamLives = 10;
};