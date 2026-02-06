// Source/Project_Bang_Squad/AnimNotify/AnimNotify_Knockback.cpp

#include "AnimNotify_Knockback.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

// [�߰�] �÷��̾����� Ȯ���� �����ϱ� ���� ��� ����
#include "Project_Bang_Squad/Character/Base/BaseCharacter.h" 

UAnimNotify_Knockback::UAnimNotify_Knockback()
{
#if WITH_EDITOR
    NotifyColor = FColor(255, 100, 100, 255);
#endif
}

void UAnimNotify_Knockback::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp || !MeshComp->GetWorld()) return;

    AActor* OwnerActor = MeshComp->GetOwner();
    if (!OwnerActor) return;

    // 1. ���� ���� Ȯ��
    if (!OwnerActor->HasAuthority()) return;

    // 2. ���� ���� ����
    FVector Start = OwnerActor->GetActorLocation();
    FVector Forward = OwnerActor->GetActorForwardVector();
    FVector End = Start + (Forward * AttackRadius);

    // 3. �浹 �˻�
    TArray<FHitResult> HitResults;
    FCollisionShape Shape = FCollisionShape::MakeSphere(AttackRadius);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerActor);

    // Pawn(ĳ����) ä�θ� ����
    bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        Shape,
        Params
    );

    if (bHit)
    {
        TSet<AActor*> HitVictims;

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();

            if (HitActor && HitActor != OwnerActor && !HitVictims.Contains(HitActor))
            {
                HitVictims.Add(HitActor);

                // [�ٽ� ����] ACharacter ��� ABaseCharacter(�÷��̾�)���� Ȯ��!
                ABaseCharacter* Player = Cast<ABaseCharacter>(HitActor);

                // �÷��̾��� ��쿡�� ���� ����
                if (Player)
                {
                    // 1. ������ ����
                    UGameplayStatics::ApplyDamage(
                        HitActor,
                        DamageAmount,
                        OwnerActor->GetInstigatorController(),
                        OwnerActor,
                        UDamageType::StaticClass()
                    );

                    // 2. �˹� ����
                    // ���Ͱ� ���� ����(��)���� �о����
                    FVector PushDir = Forward;
                    FVector LaunchForce = (PushDir * KnockbackStrength) + FVector(0.f, 0.f, KnockbackUpForce);

                    Player->LaunchCharacter(LaunchForce, true, true);
                }
            }
        }
    }
}