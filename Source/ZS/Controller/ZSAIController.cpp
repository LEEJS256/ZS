// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ZSAIController.h"

#include "Character/Monster/ZSMonsterBase.h"

AZSAIController::AZSAIController()
{
}

void AZSAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
 {
 	Super::OnMoveCompleted(RequestID, Result);
	
	AZSMonsterBase* Monster = Cast<AZSMonsterBase>(GetPawn());
	if (!Monster) return;

	Monster->SwitchTarget();
 }
