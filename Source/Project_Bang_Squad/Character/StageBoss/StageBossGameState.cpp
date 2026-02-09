// Source/Project_Bang_Squad/Character/StageBoss/StageBossGameState.cpp

#include "StageBossGameState.h"
#include "Net/UnrealNetwork.h"

AStageBossGameState::AStageBossGameState()
{
	BossCurrentHealth = 100.f;
	BossMaxHealth = 100.f;
	bIsQTEActive = false;
	CurrentQTECount = 0;
	TargetQTECount = 50;
	TeamLives = 10; // �⺻ ���
}

void AStageBossGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ���� ���� ���
	DOREPLIFETIME(AStageBossGameState, BossCurrentHealth);
	DOREPLIFETIME(AStageBossGameState, BossMaxHealth);
	DOREPLIFETIME(AStageBossGameState, bIsQTEActive);
	DOREPLIFETIME(AStageBossGameState, CurrentQTECount);
	DOREPLIFETIME(AStageBossGameState, TargetQTECount);
	DOREPLIFETIME(AStageBossGameState, TeamLives);
}

void AStageBossGameState::SetQTEActive(bool bActive)
{
	SetQTEStatus(bActive, TargetQTECount);
}

// [1] ���� ü�� ������Ʈ (���� -> Ŭ���̾�Ʈ ����)
void AStageBossGameState::UpdateBossHealth(float NewCurrent, float NewMax)
{
	if (HasAuthority())
	{
		BossCurrentHealth = NewCurrent;
		BossMaxHealth = NewMax;
		OnRep_BossHealth(); // ����(Host)������ ��� �ݿ�
	}
}

// [2] QTE ���� ���� (���� -> Ŭ���̾�Ʈ ����)
void AStageBossGameState::SetQTEStatus(bool bActive, int32 InTarget)
{
	if (HasAuthority())
	{
		bIsQTEActive = bActive;
		TargetQTECount = InTarget;
		CurrentQTECount = 0; // ���� �� �ʱ�ȭ
		OnRep_IsQTEActive();
	}
}

// [3] QTE ī��Ʈ ������Ʈ (���� -> Ŭ���̾�Ʈ ����)
void AStageBossGameState::UpdateQTECount(int32 NewCount)
{
	if (HasAuthority())
	{
		CurrentQTECount = NewCount;
		OnRep_QTECounts();
	}
}

// [4] �� ��� ���� (���� -> Ŭ���̾�Ʈ ����)
void AStageBossGameState::SetTeamLives(int32 NewLives)
{
	if (HasAuthority())
	{
		TeamLives = NewLives;
		OnRep_TeamLives();
	}
}

void AStageBossGameState::Multicast_EndQTE_Implementation(bool bSuccess)
{
	bIsQTEActive = false;
	CurrentQTECount = 0;

	OnQTEStateChanged.Broadcast(false);

	if (bSuccess)
	{
		//TODO: QTE 성공
	}
}

// --- [OnRep �Լ���: Ŭ���̾�Ʈ UI ���ſ�] ---

void AStageBossGameState::OnRep_BossHealth()
{
	OnBossHealthChanged.Broadcast(BossCurrentHealth, BossMaxHealth);
}

void AStageBossGameState::OnRep_IsQTEActive()
{
	OnQTEStateChanged.Broadcast(bIsQTEActive);
}

void AStageBossGameState::OnRep_QTECounts()
{
	OnQTECountUpdated.Broadcast(CurrentQTECount, TargetQTECount);
}

void AStageBossGameState::OnRep_TeamLives()
{
	OnTeamLivesChanged.Broadcast(TeamLives);
}