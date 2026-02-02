#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WebPad.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECT_BANG_SQUAD_API AWebPad : public AActor
{
    GENERATED_BODY()

public:
    AWebPad();

protected:
    UPROPERTY(EditAnywhere, Category = "BS|Web")
    bool bDestroyAvailable = false;
    
    /** 영역 감지용 박스 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* CollisionBox;

    /** 바닥 거미줄 데칼 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* WebMeshComp;

    /** 감속 비율 (0.3f면 원래 속도의 30%로 감소) */
    UPROPERTY(EditAnywhere, Category = "BS|Web", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float SpeedMultiplier = 0.2f;
    
    UPROPERTY(EditAnywhere, Category = "BS|Web")
    float WebGravityScale = 0.1f;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};