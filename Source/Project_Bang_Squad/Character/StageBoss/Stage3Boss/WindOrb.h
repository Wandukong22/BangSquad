#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindOrb.generated.h"

UCLASS()
class PROJECT_BANG_SQUAD_API AWindOrb : public AActor
{
	GENERATED_BODY()

public:
	AWindOrb();

protected:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* EffectComp;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};