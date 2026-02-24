#include "Stage3BossAIController.h"
#include "Stage3Boss.h"
#include "Kismet/GameplayStatics.h"

void AStage3BossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	Boss = Cast<AStage3Boss>(InPawn);
	GetWorldTimerManager().SetTimer(ActionTimer, this, &AStage3BossAIController::DecideNextAction, 10.0f, false);
}

void AStage3BossAIController::DecideNextAction()
{
	if (!Boss) return;
	TargetPlayer = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!TargetPlayer) return;

	float Dist = FVector::Dist(Boss->GetActorLocation(), TargetPlayer->GetActorLocation());
	float Time = GetWorld()->GetTimeSeconds();

	TMap<EBoss3Skill, float> Weights;

	if (Dist <= 300.0f) Weights.Add(EBoss3Skill::Basic, 100.0f);
	else Weights.Add(EBoss3Skill::Basic, 0.0f);

	if (Time - LastTime_Laser >= CD_Laser) Weights.Add(EBoss3Skill::Laser, 60.0f);
	if (Time - LastTime_Meteor >= CD_Meteor) Weights.Add(EBoss3Skill::Meteor, 80.0f);
	if (Time - LastTime_Break >= CD_Break) Weights.Add(EBoss3Skill::Break, 50.0f);

	float TotalWeight = 0.0f;
	for (auto& Pair : Weights) TotalWeight += Pair.Value;

	if (TotalWeight <= 0.0f)
	{
		Weights.Add(EBoss3Skill::Chase, 100.0f);
		TotalWeight = 100.0f;
	}

	float RandVal = FMath::FRandRange(0.0f, TotalWeight);
	EBoss3Skill Selected = EBoss3Skill::Chase;

	for (auto& Pair : Weights)
	{
		RandVal -= Pair.Value;
		if (RandVal <= 0.0f) { Selected = Pair.Key; break; }
	}

	float Duration = 1.0f;
	switch (Selected)
	{
	case EBoss3Skill::Basic: Duration = Boss->Execute_BasicAttack(); break;
	case EBoss3Skill::Laser: Duration = Boss->Execute_Laser(); LastTime_Laser = Time; break;
	case EBoss3Skill::Meteor: Duration = Boss->Execute_Meteor(); LastTime_Meteor = Time; break;
	case EBoss3Skill::Break: Duration = Boss->Execute_PlatformBreak(); LastTime_Break = Time; break;
	case EBoss3Skill::Chase: MoveToActor(TargetPlayer, 200.0f); Duration = 1.0f; break;
	}

	GetWorldTimerManager().SetTimer(ActionTimer, this, &AStage3BossAIController::DecideNextAction, Duration + 0.5f, false);
}