#include "WindOrb.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AWindOrb::AWindOrb()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetSphereRadius(50.0f);
	SphereComp->SetCollisionProfileName(TEXT("Trigger"));

	EffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EffectComp"));
	EffectComp->SetupAttachment(RootComponent);
}

void AWindOrb::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* Player = Cast<ACharacter>(OtherActor))
	{
		if (UCharacterMovementComponent* CMC = Player->GetCharacterMovement())
		{
			// 이동 속도 40% 증가 (기본 600 -> 840)
			float OriginalSpeed = CMC->MaxWalkSpeed;
			CMC->MaxWalkSpeed *= 1.4f;

			// 10초 뒤 복구 타이머 (람다 캡처로 처리)
			FTimerHandle Handle;
			GetWorldTimerManager().SetTimer(Handle, [CMC, OriginalSpeed]()
				{
					if (CMC) CMC->MaxWalkSpeed = OriginalSpeed;
				}, 10.0f, false);
		}

		Destroy(); // 구슬 삭제
	}
}