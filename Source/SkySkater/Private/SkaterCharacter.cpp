// Fill out your copyright notice in the Description page of Project Settings.


#include "SkaterCharacter.h"

// Sets default values
ASkaterCharacter::ASkaterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASkaterCharacter::BeginPlay()
{
	Super::BeginPlay();
	movComponent = GetCharacterMovement();
	skateboard = FindComponentByTag<UStaticMeshComponent>("Skateboard");
}

// Called every frame
void ASkaterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	double speed = movComponent->Velocity.Length();

	if (speed > 0)
	{
		currRot = GetActorRotation();
		targetRot = movComponent->Velocity.Rotation();
		SetActorRotation(UKismetMathLibrary::RInterpTo(currRot, targetRot, DeltaTime, 10));
	}

	lastTickSpeed = currentTickSpeed;
	currentTickSpeed = speed;

	if (!movComponent->IsFalling())
	{
		TryAlignSkateboard();
	}
}

void ASkaterCharacter::TryAlignSkateboard()
{
	// Skateboard alignment with the ground
	FVector frontSocketLocation = skateboard->GetSocketLocation("Front");
	FVector backSocketLocation = skateboard->GetSocketLocation("Back");

	FVector frontStartPoint = frontSocketLocation + FVector(0, 0, 30);
	FVector frontEndPoint = frontSocketLocation + FVector(0, 0, -30);

	FVector backStartPoint = backSocketLocation + FVector(0, 0, 30);
	FVector backEndPoint = backSocketLocation + FVector(0, 0, -30);

	FHitResult frontHit;
	FHitResult backHit;

	if (GetWorld()->LineTraceSingleByChannel(frontHit, frontStartPoint, frontEndPoint, ECollisionChannel::ECC_Visibility))
	{
		frontSocketLocation = frontHit.Location;
	}

	if (GetWorld()->LineTraceSingleByChannel(backHit, backStartPoint, backEndPoint, ECollisionChannel::ECC_Visibility))
	{
		backSocketLocation = backHit.Location;
	}

	FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(backSocketLocation, frontSocketLocation);

	skateboard->SetWorldRotation(newRotation);
}

// Called to bind functionality to input
void ASkaterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASkaterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASkaterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &ASkaterCharacter::LookVertical);
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &ASkaterCharacter::TurnHorizontal);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ASkaterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ASkaterCharacter::StopJumping);
	PlayerInputComponent->BindAction("PowerPush", EInputEvent::IE_Pressed, this, &ASkaterCharacter::PowerPush);
	PlayerInputComponent->BindAction("Brake", EInputEvent::IE_Repeat, this, &ASkaterCharacter::Brake);
}

void ASkaterCharacter::MoveForward(float value)
{
	FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0, Controller->GetControlRotation().Yaw, 0));
	AddMovementInput(Direction * value);
}

void ASkaterCharacter::MoveRight(float value)
{
	FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0, Controller->GetControlRotation().Yaw, Controller->GetControlRotation().Roll));
	AddMovementInput(Direction * value);
}

void ASkaterCharacter::LookVertical(float value)
{
	AddControllerPitchInput(value);
}

void ASkaterCharacter::TurnHorizontal(float value)
{
	AddControllerYawInput(value);
}

bool ASkaterCharacter::IsAccelerating()
{
	return currentTickSpeed > lastTickSpeed + accelerationThreshold;
}

FVector ASkaterCharacter::GetFrontLegSocket()
{
	return skateboard->GetSocketLocation("FrontLeg") + FVector(0, 0, 15);
}

FVector ASkaterCharacter::GetBackLegSocket()
{
	return skateboard->GetSocketLocation("BackLeg") + FVector(0, 0, 15);
}

void ASkaterCharacter::PowerPush()
{
	if (powerPushTimerHandle.IsValid())
		return;

	movComponent->MaxWalkSpeed = movComponent->MaxWalkSpeed + powerPushExtraSpeed;
	GetWorld()->GetTimerManager().SetTimer(
		powerPushTimerHandle, // handle to cancel timer at a later time
		this, // the owning object
		&ASkaterCharacter::OnPowerPushEnd, // function to call on elapsed
		powerPushExtraSpeedTime, // float delay until elapsed
		false); // looping?
}

void ASkaterCharacter::Brake()
{
	if (movComponent->Velocity.Length() < 1)
	{
		return;
	}
	FVector Direction = GetActorForwardVector();
	movComponent->AddImpulse(-Direction * brakingValue);
}

void ASkaterCharacter::OnPowerPushEnd()
{
	movComponent->MaxWalkSpeed = movComponent->MaxWalkSpeed - powerPushExtraSpeed;
	powerPushTimerHandle.Invalidate();
}

