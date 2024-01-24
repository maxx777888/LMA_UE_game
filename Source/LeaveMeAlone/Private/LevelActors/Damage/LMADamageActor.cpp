// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "LevelActors/Damage/LMADamageActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


// Sets default values
ALMADamageActor::ALMADamageActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Инициализируем сферу которая будет имитировать огонь, в котором происходит урон
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetSphereRadius(SphereRadius);
	SetRootComponent(SphereComponent);
	//Инициализируем компонент меш, который визуально будет показывать область получения урона
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(SphereComponent);
	StaticMesh->SetRelativeScale3D(FVector(2.0f, 2.0f, 0.05f));
	StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -4.0f));


}

// Called when the game starts or when spawned
void ALMADamageActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALMADamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*Функция которая воспроизводит урон:
		? Указатель на мир.
		? Количественная характеристика урона.
		? Центр сферы урона.
		? Радиус сферы.
		? Тип урона.
		? Массив Акторов, которым мы не хотим наносить урон.
		? Актор который непосредственно наносит ущерб
		? Контроллер Актора, который наносит ущерб
		? Как будет наноситься ущерб.Если правда – то урон во всей сфере одинаковый,
			если ложь то урон будет интерполироваться относительно центра сферы.
		? Какие коллизии блокируют урон.*/

	UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), 
		SphereRadius, nullptr, {}, this, nullptr, false);


}

