// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true); 
	
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;


	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Определяем персонажу компонент здоровья
	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");


}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	OnHealthChanged(HealthComponent->GetHealth());//Явно инициализируем переменную Health
	//Подписываемся на делегат, который сообщает о смерти персонажа с помощью функции AddUObject()
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	//Подписываемся на делегат, который обрабатывает изменения в здоровье персонажа с помощью функции AddUObject()
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
	
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!(HealthComponent->IsDead()))//Будет вызываться только если персонаж жив. 
	{
		RotationPlayerOnCursor();//Метод для управления курсором мышки
	}
	//Настройки спринта и выносливости
	if (IsSprint == true && MaxStamina > 0.f)
	{
		DecreaseStamina();
	}
	else if (IsSprint == false && MaxStamina < 100.f)
	{
		IncreaseStamina();
	}
	if (FMath::IsNearlyZero(MaxStamina))
	{
		SprintStop();
	}

}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);

	PlayerInputComponent->BindAxis("CameraZoomInOut", this, &ALMADefaultCharacter::CameraZoomInOut);

	PlayerInputComponent->BindAction("SprintRun", IE_Pressed, this, &ALMADefaultCharacter::SprintRun);
	PlayerInputComponent->BindAction("SprintRun", IE_Pressed, this, &ALMADefaultCharacter::SprintStop);


}

void ALMADefaultCharacter::OnDeath() 
{
	CurrentCursor->DestroyRenderState_Concurrent();//Предоставляет управление курсором стороннему наблюдателю. 

	PlayAnimMontage(DeathMontage);//Проигрываение анимации смерти персонажа
	GetCharacterMovement()->DisableMovement();//Отключение движение у персонажа
	SetLifeSpan(5.0f);//Событие, которое уничтожает объект через заданное кол-во времени

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth) 
{
	//Выводим на экран размер здоровья при изменении параметров здоровья
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor() 
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC))
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

void ALMADefaultCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value) 
{
	AddMovementInput(GetActorRightVector(), Value);
} 

void ALMADefaultCharacter::CameraZoomInOut(float Value)
{

	if (Value > 0 && newZoomF < MaxZoom)
	{
		newZoomF = newZoomF + ZoomSpeed; 
	}
	else if (Value < 0 && newZoomF > MinZoom)
	{
		newZoomF = newZoomF - ZoomSpeed;
	}
	SpringArmComponent->TargetArmLength = newZoomF;

}
//----------- Методы работы Спринта ----------------
void ALMADefaultCharacter::SprintRun() 
{
	IsSprint = true;
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 700.0f;
	if (MaxStamina > 0)
	{
		DecreaseStamina();
	}
}

void ALMADefaultCharacter::SprintStop() 
{
	IsSprint = false;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f;
	IncreaseStamina();
}

void ALMADefaultCharacter::DecreaseStamina() 
{
	MaxStamina = MaxStamina - WasteStamina;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::Printf(TEXT("Stamina = %f"), MaxStamina));
}

void ALMADefaultCharacter::IncreaseStamina() 
{
	if (IsSprint == false)
	{
		MaxStamina = MaxStamina + AccumulationStamina;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Stamina = %f"), MaxStamina));
	}	
}

