#include "Project_Bang_Squad/Character/Player/Titan/TitanRock.h" // ��ΰ� �´��� Ȯ��!
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" // SphereOverlapActors��
#include "GameFramework/Character.h"    // LaunchCharacter��

ATitanRock::ATitanRock()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. �浹ü ����
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(40.0f);
	
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComp->SetNotifyRigidBodyCollision(true); // Hit �̺�Ʈ �ʼ�

	RootComponent = CollisionComp;

	// 2. ���� �޽� ����
	RockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RockMesh"));
	RockMesh->SetupAttachment(RootComponent);
	RockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// �浹 �̺�Ʈ ���ε�
	CollisionComp->OnComponentHit.AddDynamic(this, &ATitanRock::OnHit);
}

void ATitanRock::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(5.0f); // 5�� �� �ڵ� ����
}

void ATitanRock::InitializeRock(float InDamage, AActor* InOwner)
{
	Damage = InDamage;
	OwnerCharacter = InOwner;
	SetOwner(InOwner); // ���� ����
}

void ATitanRock::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ���� ���� ���ΰ� �ٷ� �ε����� �� ����
	if (OtherActor == OwnerCharacter) return;

	// �����̵� �ε����� ����
	Explode();
}

void ATitanRock::Explode()
{
	FVector ExplosionLocation = GetActorLocation();

	// [�α�] ���� Ȯ�ο�
	// UE_LOG(LogTemp, Warning, TEXT("TitanRock Exploded!"));

	// 1. ���� �� ���� ã��
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this); // �� �ڽ� ����

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		ExplosionLocation,
		ExplosionRadius,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		OverlappedActors
	);

	// 2. ������ �� �˹� ó��
	for (AActor* Victim : OverlappedActors)
	{
		if (!Victim || !Victim->IsValidLowLevel()) continue;

		// �Ʊ� Ȯ�� ("Player" �±�)
		bool bIsAlly = Victim->ActorHasTag("Player");

		// �������Ը� ������
		if (!bIsAlly)
		{
			UGameplayStatics::ApplyDamage(
				Victim,
				Damage,
				OwnerCharacter ? OwnerCharacter->GetInstigatorController() : nullptr,
				OwnerCharacter,
				UDamageType::StaticClass()
			);
		}

		// �˹� (��� ����)
		FVector LaunchDir = (Victim->GetActorLocation() - ExplosionLocation).GetSafeNormal();
		LaunchDir.Z = 0.6f; // ���� ����
		LaunchDir.Normalize();

		if (ACharacter* VictimChar = Cast<ACharacter>(Victim))
		{
			VictimChar->LaunchCharacter(LaunchDir * KnockbackForce, true, true);
		}
		else if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Victim->GetRootComponent()))
		{
			if (RootComp->IsSimulatingPhysics())
			{
				RootComp->AddImpulse(LaunchDir * KnockbackForce * 100.0f);
			}
		}
	}

	// 3. �ı�
	Destroy();
}