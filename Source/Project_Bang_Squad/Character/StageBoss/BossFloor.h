#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossFloor.generated.h"

class UStaticMeshComponent;
class UNavModifierComponent;

UCLASS()
class PROJECT_BANG_SQUAD_API ABossFloor : public AActor
{
	GENERATED_BODY()

public:
	ABossFloor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> FloorMesh;

	// 충돌 설정이 까다로울 때 강제로 네비매쉬를 생성해주는 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UNavModifierComponent> NavModifier;
};