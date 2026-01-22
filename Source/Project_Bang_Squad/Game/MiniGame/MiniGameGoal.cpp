// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_Bang_Squad/Game/MiniGame/MiniGameGoal.h"

#include "MiniGameMode.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AMiniGameGoal::AMiniGameGoal()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(RootComponent);
	DetectSphere->SetSphereRadius(150.f);
	DetectSphere->SetCollisionProfileName(TEXT("Trigger"));

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(RootComponent);
	InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidget->SetDrawAtDesiredSize(true);
	InteractWidget->SetVisibility(false);
	InteractWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	
	bReplicates = true;
}

void AMiniGameGoal::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("Goal::Interact 진입 성공!"));
	
	if (!InstigatorPawn) return;

	//게임 모드 가져와서 보고
	AMiniGameMode* GM = Cast<AMiniGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->OnPlayerReachedGoal(InstigatorPawn->GetController());
	}
}

void AMiniGameGoal::BeginPlay()
{
	Super::BeginPlay();

	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &AMiniGameGoal::OnOverlapBegin);
	DetectSphere->OnComponentEndOverlap.AddDynamic(this, &AMiniGameGoal::OnOverlapEnd);
}

void AMiniGameGoal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (PlayerPawn && PlayerPawn->IsLocallyControlled())
	{
		InteractWidget->SetVisibility(true);
	}
}

void AMiniGameGoal::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* PlayerPawn = Cast<APawn>(OtherActor);
	if (PlayerPawn && PlayerPawn->IsLocallyControlled())
	{
		InteractWidget->SetVisibility(false);
	}
}

