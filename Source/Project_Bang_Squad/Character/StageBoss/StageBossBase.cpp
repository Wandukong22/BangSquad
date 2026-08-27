#include "StageBossBase.h"
#include "Net/UnrealNetwork.h"
#include "Project_Bang_Squad/Character/Damage/TrueDamageType.h"
#include "Engine/DamageEvents.h"

AStageBossBase::AStageBossBase()
{
    // ��Ƽ�÷��̾� ����
    bReplicates = true;
    SetNetUpdateFrequency(100.0f); // ������ �����Ӱ� ���� ��ȭ�� �߿��ϹǷ� �󵵸� ���� ����
}

void AStageBossBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // [�ߺ� ���� �� ����ȭ] ��� ���� ������ �� ������ ����մϴ�.
    DOREPLIFETIME(AStageBossBase, CurrentPhase);
    DOREPLIFETIME(AStageBossBase, bIsInvincible);
    DOREPLIFETIME(AStageBossBase, bIsActionInProgress);
}

float AStageBossBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // [����] �θ� Ŭ������ Ʈ�� �������� �˾ƺ��� �մϴ�!
    bool bIsTrueDamage = (DamageEvent.DamageTypeClass == UTrueDamageType::StaticClass());

    // Ʈ�� �������� '�ƴ� ����' ���� üũ
    if (!bIsTrueDamage && bIsInvincible)
    {
        return 0.0f;
    }

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
void AStageBossBase::SetPhase(EBossPhase NewPhase)
{
    if (!HasAuthority()) return;
    if (CurrentPhase == NewPhase) return;

    CurrentPhase = NewPhase;

    // ���������� ���� ����
    OnPhaseChanged(CurrentPhase);

    // ������ OnRep�� �ڵ����� ȣ����� �����Ƿ� ���� ȣ���Ͽ� ����ȭ ���� ����
    OnRep_CurrentPhase();
}

void AStageBossBase::OnRep_CurrentPhase()
{
    // [���־� ����ȭ] Ŭ���̾�Ʈ���� ������ ���濡 ���� UI ������Ʈ�� ����Ʈ ó���� ����
}

void AStageBossBase::OnPhaseChanged(EBossPhase NewPhase)
{
    // �ڽ� Ŭ����(Stage1Boss ��)���� �� ������ ���� ���� ����
}

void AStageBossBase::OnGimmickResolved(int32 GimmickID)
{
    // �ڽ� Ŭ�������� ��� Ŭ���� ���� ����
}

void AStageBossBase::ServerSetActionInProgress_Implementation(bool bInProgress)
{
    // ���������� ���¸� �����ϰ� �����ϵ��� ����
    if (HasAuthority())
    {
        bIsActionInProgress = bInProgress;

        // ����� �α�: ���� ��ȭ ������
        // UE_LOG(LogTemp, Log, TEXT("Boss Action State Changed: %s"), bInProgress ? TEXT("TRUE") : TEXT("FALSE"));
    }
}