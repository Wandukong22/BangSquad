#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TitanThrowableActor.generated.h"

UCLASS()
class PROJECT_BANG_SQUAD_API ATitanThrowableActor : public AActor
{
	GENERATED_BODY()

public:
	ATitanThrowableActor();

protected:
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;
	
public:
	// ������ ��ü�� ���� (Static Mesh)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// �������� �� �߰� ȿ��(��: ���ư��� �Ҹ�, ��ƼŬ)�� ���� ����� �Լ�
	UFUNCTION(BlueprintNativeEvent, Category = "Titan|Throw")
	void OnThrown(FVector Direction);
};