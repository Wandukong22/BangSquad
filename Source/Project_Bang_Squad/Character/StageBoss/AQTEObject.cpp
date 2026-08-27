// [AQTEObject.cpp]
#include "AQTEObject.h"

#include "StageBossGameState.h"
#include "Project_Bang_Squad/Character/StageBoss/StageBossBase.h"
#include "Project_Bang_Squad/Character/Base/BaseCharacter.h"
#include "Project_Bang_Squad/Character/Damage/TrueDamageType.h"
#include "Project_Bang_Squad/Character/StageBoss/Stage1Boss.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AQTEObject::AQTEObject()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
    SetReplicateMovement(true); // ������ ����ȭ

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
}

// [���� ���� ���]
void AQTEObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // CurrentTapCount�� ��� Ŭ���̾�Ʈ���� ����
    DOREPLIFETIME(AQTEObject, CurrentTapCount);
}

void AQTEObject::BeginPlay()
{
    Super::BeginPlay();
}

void AQTEObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // [����] ���� ����
    if (HasAuthority() && bIsFalling)
    {
        ElapsedTime += DeltaTime;
        float Alpha = FMath::Clamp(ElapsedTime / FallDuration, 0.0f, 1.0f);
        SetActorLocation(FMath::Lerp(StartLocation, TargetLocation, Alpha));
    }
}

// [���� �ʱ�ȭ]
void AQTEObject::InitializeFalling(AActor* Target, float Duration)
{
    if (!HasAuthority() || !Target) return;

    TargetLocation = Target->GetActorLocation();
    TargetLocation.Z -= 100.0f; // �ٴ� ����

    StartLocation = TargetLocation + FVector(0.0f, 0.0f, DropHeight);
    SetActorLocation(StartLocation);

    FallDuration = Duration;
    ElapsedTime = 0.0f;
    bIsFalling = true;
}

// [�� ���]
void AQTEObject::RegisterTap()
{
    if (!HasAuthority()) return;

    CurrentTapCount++;
    UE_LOG(LogTemp, Warning, TEXT("QTE Tapped! Current Count: %d"), CurrentTapCount);

    if (CurrentTapCount >= TargetTapCount)
    {
        TriggerSuccess();
    }
}

// [���� ó��: ���� ó��]
void AQTEObject::TriggerSuccess()
{
    if (HasAuthority())
    {
        Multicast_PlayExplosion(true);

        // 1. [�ٽ�] ���忡 �ִ� Stage1Boss�� ���� ã���ϴ�. (GetOwner ��� X)
        AActor* BossActor = UGameplayStatics::GetActorOfClass(GetWorld(), AStage1Boss::StaticClass());

        // ������ġ: �� ã������ �θ� Ŭ������ �˻�
        if (!BossActor)
        {
            BossActor = UGameplayStatics::GetActorOfClass(GetWorld(), AStageBossBase::StaticClass());
        }

        // 2. ���� �߰� �� ó�� ������ �߼�
        if (BossActor)
        {
            UE_LOG(LogTemp, Warning, TEXT(">>> TARGET FOUND: %s. Executing True Damage! <<<"), *BossActor->GetName());

            UGameplayStatics::ApplyDamage(
                BossActor,
                999999.0f,                      // ��� ������
                nullptr,
                this,
                UTrueDamageType::StaticClass()  // [����] ���� ���� (TrueDamageType)
            );
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT(">>> ERROR: Boss Not Found! Damage Failed. <<<"));
        }

        //QTE 끝났다고 Broadcast
        if (auto* GS = GetWorld()->GetGameState<AStageBossGameState>())
        {
            GS->Multicast_EndQTE(true);
        }

        SetLifeSpan(1.0f);
    }
}

// [���� ó��: ����Ʈ��]
// (�÷��̾� ������ ������ HandleQTEResult���� ó���Ѵٰ� �ϼ����Ƿ� ���⼱ ����Ʈ�� ��)
void AQTEObject::TriggerFailure()
{
    if (HasAuthority())
    {
        Multicast_PlayExplosion(false);

        if (auto* GS = GetWorld()->GetGameState<AStageBossGameState>())
        {
            GS->Multicast_EndQTE(false);
        }
        
        SetLifeSpan(0.5f);
    }
}

void AQTEObject::OnRep_CurrentTapCount()
{
    // ���־� ���� �ʿ� �� ����
}

void AQTEObject::Multicast_PlayExplosion_Implementation(bool bIsSuccess)
{
    if (ExplosionFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionFX, GetActorLocation(), GetActorRotation(), FVector(3.0f));
    }
}