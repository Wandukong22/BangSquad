#pragma once

#include "CoreMinimal.h"
#include "Project_Bang_Squad/Game/Stage/StageGameState.h"
#include "StageBossGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossHealthChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTEStateChanged, bool, bIsActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQTECountUpdated, int32, Current, int32, Target);
// [�ű�] ��� ���� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeamLivesChanged, int32, NewLives);

UCLASS()
class PROJECT_BANG_SQUAD_API AStageBossGameState : public AStageGameState
{
	GENERATED_BODY()

public:
	AStageBossGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- [1] ���� ü�� ---
	UPROPERTY(ReplicatedUsing = OnRep_BossHealth, BlueprintReadOnly, Category = "Boss|State")
	float BossCurrentHealth;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Boss|State")
	float BossMaxHealth;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnBossHealthChanged OnBossHealthChanged;

	// --- [2] QTE ��� ---
	UPROPERTY(ReplicatedUsing = OnRep_IsQTEActive, BlueprintReadOnly, Category = "Boss|QTE")
	bool bIsQTEActive;

	UPROPERTY(ReplicatedUsing = OnRep_QTECounts, BlueprintReadOnly, Category = "Boss|QTE")
	int32 CurrentQTECount;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Boss|QTE")
	int32 TargetQTECount;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnQTEStateChanged OnQTEStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnQTECountUpdated OnQTECountUpdated;

	// --- [3] �� ���� ��� (�ű� �߰�) ---
	UPROPERTY(ReplicatedUsing = OnRep_TeamLives, BlueprintReadOnly, Category = "Boss|Rules")
	int32 TeamLives;

	UPROPERTY(BlueprintAssignable, Category = "Boss|Events")
	FOnTeamLivesChanged OnTeamLivesChanged;

	UFUNCTION()
	void SetQTEActive(bool bActive);

	// --- [���� �Լ�] ---
	void UpdateBossHealth(float NewCurrent, float NewMax);
	void SetQTEStatus(bool bActive, int32 InTarget);
	void UpdateQTECount(int32 NewCount);
	void SetTeamLives(int32 NewLives); // GameMode�� ȣ��

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EndQTE(bool bSuccess);

protected:
	UFUNCTION() void OnRep_BossHealth();
	UFUNCTION() void OnRep_IsQTEActive();
	UFUNCTION() void OnRep_QTECounts();
	UFUNCTION() void OnRep_TeamLives(); // UI ���ſ�

private:
	UPROPERTY(Replicated)
	int32 TeamDeathCount = 0;
};