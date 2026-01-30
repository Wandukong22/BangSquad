// EnemyMidBoss.cpp

#include "EnemyMidBoss.h"
#include "Project_Bang_Squad/Character/MonsterBase/MidBossAIController.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Project_Bang_Squad/Character/Base/BaseCharacter.h"
#include "Project_Bang_Squad/Projectile/SlashProjectile.h"

AEnemyMidBoss::AEnemyMidBoss()
{
	// CDO 체크 추가
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	}
	
	CurrentPhase = EMidBossPhase::Normal;
	bReplicates = true;

	// �ｺ ������Ʈ
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// ���� �浹 �ڽ� ���� �� ����
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	// �޽� ������ ���� (���� �̸� ����)
	if (GetMesh())
	{
		WeaponCollisionBox->SetupAttachment(GetMesh(), TEXT("swordSocket"));
	}
	else
	{
		WeaponCollisionBox->SetupAttachment(RootComponent);
	}

	// ��ҿ��� ����
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // �÷��̾ ����

	WeaponCollisionBox->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyMidBoss::OnWeaponOverlap);
}

void AEnemyMidBoss::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnemyMidBoss, CurrentPhase);
}

void AEnemyMidBoss::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (BossData)
	{
		if (GetMesh() && BossData->Mesh)
		{
			GetMesh()->SetSkeletalMesh(BossData->Mesh);
		}
		if (GetMesh() && BossData->AnimClass)
		{
			GetMesh()->SetAnimInstanceClass(BossData->AnimClass);
		}
		// CDO 체크 추가
		if (!HasAnyFlags(RF_ClassDefaultObject))
		{
			if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
			{
				MoveComp->MaxWalkSpeed = BossData->WalkSpeed;
			}
		}
	}
}

void AEnemyMidBoss::BeginPlay()
{
	Super::BeginPlay();

	// ���� ���� �� ������ ���� Ȯ��
	if (HasAuthority() && BossData && HealthComponent)
	{
		HealthComponent->SetMaxHealth(BossData->MaxHealth);
		UE_LOG(LogTemp, Log, TEXT("[MidBoss] Initialized Health: %f"), BossData->MaxHealth);
	}

	// ��� �̺�Ʈ ����
	if (HealthComponent)
	{
		HealthComponent->OnDead.AddDynamic(this, &AEnemyMidBoss::OnDeath);
	}

	// ������ ������ AttackRange�� AI���� ����!
	if (GetController())
	{
		auto* MyAI = Cast<AMidBossAIController>(GetController());
		if (MyAI && BossData)
		{
			MyAI->SetAttackRange(BossData->AttackRange);
			UE_LOG(LogTemp, Warning, TEXT("[MidBoss] Attack Range Updated from DataAsset: %f"), BossData->AttackRange);
		}
	}
}

float AEnemyMidBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage <= 0.0f) return 0.0f;
	if (HealthComponent && HealthComponent->IsDead()) return 0.0f;

	

	if (GetController())
	{
		auto* MyAI = Cast<AMidBossAIController>(GetController());
		if (MyAI)
		{
			// [������] ����ü(DamageCauser)�� ������� �۶����� �ʰ�, ������ ���(Instigator Pawn)�� ã�Ƽ� �Ѱ��ݴϴ�.
			AActor* RealAttacker = DamageCauser;
			if (EventInstigator && EventInstigator->GetPawn())
			{
				RealAttacker = EventInstigator->GetPawn();
			}

			MyAI->OnDamaged(RealAttacker);
		}
	}

	return ActualDamage;
}

// --- [Combat: Weapon Collision Logic] ---

void AEnemyMidBoss::EnableWeaponCollision()
{
	if (WeaponCollisionBox)
	{
		WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AEnemyMidBoss::DisableWeaponCollision()
{
	if (WeaponCollisionBox)
	{
		WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}


void AEnemyMidBoss::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()) return;
    if (OtherActor == nullptr || OtherActor == this || OtherActor == GetOwner()) return;

    if (OtherActor->IsA(ABaseCharacter::StaticClass()))
    {
        // 1. [최적화] 칼이 직접 방패 컴포넌트를 때렸다면?
        // LineTrace고 뭐고 필요 없음. 바로 데미지 줘서 방패 깎게 함.
        if (OtherComp && OtherComp->GetName().Contains(TEXT("Shield")))
        {
             UGameplayStatics::ApplyDamage(OtherActor, AttackDamage, GetController(), this, UDamageType::StaticClass());
             // UE_LOG(LogTemp, Warning, TEXT("BOSS: Direct Shield Hit!"));
             return; 
        }

        // 2. 몸통을 때렸을 때 -> 레이저 검사
        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        FVector Start = GetActorLocation(); 
        FVector End = OtherActor->GetActorLocation(); // 골반(Root) 위치

        // [디버그] 레이저가 어디로 나가는지 눈으로 확인! (빨간선)
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 2.0f);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            Start,
            End,
            ECC_Visibility,
            Params
        );

        if (bHit)
        {
            // 방패 감지
            if (HitResult.GetComponent() && HitResult.GetComponent()->GetName().Contains(TEXT("Shield")))
            {
                AActor* ShieldOwner = HitResult.GetActor();

                // [핵심] "방패 주인이 지금 맞은 놈인가?"
                if (ShieldOwner == OtherActor || ShieldOwner->GetOwner() == OtherActor)
                {
                    // PASS! (팔라딘 본인이 막음 -> 데미지 줘서 방패 깎아야 함)
                    // 여기에 아무것도 안 쓰고 통과시키면 됨
                    // UE_LOG(LogTemp, Log, TEXT("BOSS: Shield Owner Hit -> Apply Damage to Shield"));
                }
                else
                {
                    // BLOCKED! (뒤에 있는 친구 보호 -> 데미지 무시)
                    return; 
                }
            }
        }
        
        // 3. 데미지 적용 (방패 주인이면 여기서 데미지가 들어가서 Paladin::TakeDamage가 호출됨)
        UGameplayStatics::ApplyDamage(
           OtherActor,
           AttackDamage,
           GetController(),
           this,
           UDamageType::StaticClass()
        );
    }
}

// --- [Animation Helpers] ---

float AEnemyMidBoss::PlayAggroAnim()
{
	if (!HasAuthority()) return 0.0f;
	if (BossData && BossData->AggroMontage)
	{
		// [����] ��Ƽĳ��Ʈ�� "��� �����!" ���
		Multicast_PlayAttackMontage(BossData->AggroMontage);

		// AI���Դ� �ִϸ��̼� ���� ���� (Ÿ�̸ӿ�)
		return BossData->AggroMontage->GetPlayLength();
	}
	return 0.0f;
}

// [������] ������ ��Ÿ�ָ� ���� -> ��ο��� ��� -> ���̸� ����
float AEnemyMidBoss::PlayRandomAttack()
{
	if (!HasAuthority()) return 0.0f;

	if (BossData && BossData->AttackMontages.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, BossData->AttackMontages.Num() - 1);
		UAnimMontage* SelectedMontage = BossData->AttackMontages[RandomIndex];

		if (SelectedMontage)
		{
			// ��Ƽĳ��Ʈ ȣ�� (Ŭ���̾�Ʈ�鵵 ���)
			Multicast_PlayAttackMontage(SelectedMontage);

			// AI Ÿ�̸ӿ� ���� ����
			return SelectedMontage->GetPlayLength();
		}
	}
	return 0.0f;
}

// [NEW] ��Ƽĳ��Ʈ ������
void AEnemyMidBoss::Multicast_PlayAttackMontage_Implementation(UAnimMontage* MontageToPlay)
{
	if (MontageToPlay)
	{
		PlayAnimMontage(MontageToPlay);
	}
}

// EnemyMidBoss.cpp

float AEnemyMidBoss::PlayHitReactAnim()
{
	// 1. ���� üũ (������ ����)
	if (!HasAuthority()) return 0.0f;

	if (BossData && BossData->HitReactMontage)
	{
		// 2. [����] ��Ƽĳ��Ʈ�� "��� ���� ��� �����!" ���
		Multicast_PlayAttackMontage(BossData->HitReactMontage);

		// 3. AI���Դ� �ִϸ��̼� ���� ���� (���� �ð� ����)
		return BossData->HitReactMontage->GetPlayLength();
	}
	return 0.0f;
}

float AEnemyMidBoss::PlaySlashAttack()
{
	// 1. ���� ���� üũ
	if (!HasAuthority()) return 0.0f;

	// 2. ������ ���¿� ��Ÿ�ְ� �ִ��� Ȯ��
	// (����: BossData ����ü�� SlashAttackMontage ������ �־�� �մϴ�!)
	if (BossData && BossData->SlashAttackMontage)
	{
		// 3. ��Ƽĳ��Ʈ�� ��� (����+Ŭ�� ��� ���)
		Multicast_PlayAttackMontage(BossData->SlashAttackMontage);

		// 4. �ִϸ��̼� ���� ���� (AI Ÿ�̸ӿ�)
		return BossData->SlashAttackMontage->GetPlayLength();
	}

	return 0.0f;
}
// --- [Phase Control] ---

void AEnemyMidBoss::SetPhase(EMidBossPhase NewPhase)
{
	if (HasAuthority())
	{
		CurrentPhase = NewPhase;
		OnRep_CurrentPhase();
	}
}

void AEnemyMidBoss::OnRep_CurrentPhase()
{
	switch (CurrentPhase)
	{
	case EMidBossPhase::Normal:
		break;
	case EMidBossPhase::Gimmick:
		UE_LOG(LogTemp, Warning, TEXT("[MidBoss] Phase GIMMICK Start! (FX)"));
		break;
	case EMidBossPhase::Dead:
		// ��� �ִϸ��̼�
		if (BossData && BossData->DeathMontage)
		{
			PlayAnimMontage(BossData->DeathMontage);
		}

		// �浹 ����
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// ���� ������ Ȯ���� ����
		DisableWeaponCollision();
		break;
	}
}

void AEnemyMidBoss::OnDeath()
{
	SetPhase(EMidBossPhase::Dead);

	auto* MyAI = Cast<AMidBossAIController>(GetController());
	if (MyAI)
	{
		MyAI->SetDeadState();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisableWeaponCollision();

	if (HasAuthority())
	{
		SetLifeSpan(5.0f);
	}

	UE_LOG(LogTemp, Warning, TEXT("BOSS IS DEAD. Cleanup started."));
}

// 1. ���� ���� (AI�� ȣ��)
void AEnemyMidBoss::PlaySlashPattern()
{
	if (BossData && BossData->SlashAttackMontage)
	{
		PlayAnimMontage(BossData->SlashAttackMontage);
	}
}

// 2. �߻� Ʈ���� (AnimNotify���� ȣ��)
void AEnemyMidBoss::FireSlashProjectile()
{
	if (!BossData || !BossData->SlashProjectileClass) return;

	FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100.f;
	FRotator SpawnRot = GetActorRotation();

	if (GetMesh()->DoesSocketExist(TEXT("swordSocket")))
	{
		SpawnLoc = GetMesh()->GetSocketLocation(TEXT("swordSocket"));
	}

	// �������� ��û
	Server_SpawnSlashProjectile(SpawnLoc, SpawnRot);
}

// 3. ���� ������
void AEnemyMidBoss::Server_SpawnSlashProjectile_Implementation(FVector SpawnLoc, FRotator SpawnRot)
{
	if (!BossData || !BossData->SlashProjectileClass) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	GetWorld()->SpawnActor<AActor>(BossData->SlashProjectileClass, SpawnLoc, SpawnRot, Params);
}