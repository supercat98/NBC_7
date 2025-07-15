#include "Assignment_7_Pawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AAssignment_7_Pawn::AAssignment_7_Pawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->SetCapsuleHalfHeight(88.f);
	CapsuleComp->SetCapsuleRadius(34.f);
	CapsuleComp->SetSimulatePhysics(false);

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetSimulatePhysics(false);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm Component"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 350.f;
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComp->SetupAttachment(SpringArmComp);
}

void AAssignment_7_Pawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AAssignment_7_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAssignment_7_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAssignment_7_Pawn::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAssignment_7_Pawn::Look);
	}
}

void AAssignment_7_Pawn::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();

		const FVector ForwardDirection = GetActorForwardVector();
		AddActorWorldOffset(ForwardDirection * MovementVector.Y * MoveSpeed * DeltaTime, true);

		const FVector RightDirection = GetActorRightVector();
		AddActorWorldOffset(RightDirection * MovementVector.X * MoveSpeed * DeltaTime, true);
	}
}

void AAssignment_7_Pawn::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();


		AddActorLocalRotation(FRotator(0.f, LookAxisVector.X * LookSpeed * DeltaTime, 0.f), true);


		const FRotator CurrentRotation = SpringArmComp->GetRelativeRotation();
		const float NewPitch = FMath::Clamp(CurrentRotation.Pitch + (LookAxisVector.Y * LookSpeed * DeltaTime), -60.f, 60.f);
		SpringArmComp->SetRelativeRotation(FRotator(NewPitch, 0.f, 0.f));
	}
}


