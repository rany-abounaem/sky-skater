// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SkaterCharacter.generated.h"

UCLASS(Blueprintable)
class SKYSKATER_API ASkaterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASkaterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float value);
	void MoveRight(float value);
	void LookVertical(float value);
	void TurnHorizontal(float value);
	void PowerPush();
	void Brake();
	void OnPowerPushEnd();

	UFUNCTION(BlueprintCallable)
	bool IsAccelerating();

	UFUNCTION(BlueprintCallable)
	FVector GetFrontLegSocket();

	UFUNCTION(BlueprintCallable)
	FVector GetBackLegSocket();

	UPROPERTY(EditDefaultsOnly)
	float accelerationThreshold = 10;

	UPROPERTY(EditDefaultsOnly)
	float powerPushExtraSpeed = 500;

	UPROPERTY(EditDefaultsOnly)
	float powerPushExtraSpeedTime = 1;

	UPROPERTY(EditDefaultsOnly)
	float brakingValue = 0.5;

	UStaticMeshComponent* skateboard;

	void AddPlayerScore(int value)
	{
		playerScore += value;
	}

private:
	FRotator currRot;
	FRotator targetRot;
	float rotSpeed;
	UCharacterMovementComponent* movComponent;
	double lastTickSpeed;
	double currentTickSpeed;
	FTimerHandle powerPushTimerHandle;
	int playerScore;

	void TryAlignSkateboard();
};
