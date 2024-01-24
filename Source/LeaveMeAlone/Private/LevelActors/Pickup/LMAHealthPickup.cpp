// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "LevelActors/Pickup/LMAHealthPickup.h"
#include "Components/LMAHealthComponent.h"
#include "Components/SphereComponent.h"
#include "Player/LMADefaultCharacter.h"

// Sets default values
ALMAHealthPickup::ALMAHealthPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Инициализация сферы как компонента пикапа здоровья
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Задаем условия перекрытия
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	SetRootComponent(SphereComponent);
}

// Called when the game starts or when spawned
void ALMAHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}
//Функция NotifyActorBeginOverlap отработает, при пересечении персонажем капсулы пикапа здоровья 
void ALMAHealthPickup::NotifyActorBeginOverlap(AActor* OtherActor) 
{
	Super::NotifyActorBeginOverlap(OtherActor);
	const auto Charcter = Cast<ALMADefaultCharacter>(OtherActor);
	if (GivePickup(Charcter))
	{
		PickupWasTaken();
	}
}

// Called every frame
void ALMAHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
//Метод добавляет в компонент здоровье кол-во единиц пикапа здоровья
bool ALMAHealthPickup::GivePickup(ALMADefaultCharacter* Character)
{
	const auto HealthComponent = Character->GetHealthComponent();
	if (!HealthComponent) return false;

	return HealthComponent->AddHealth(HealthFromPickup);
}

void ALMAHealthPickup::PickupWasTaken()//Метод обрабатывает взятие пикапа здоровья
{
	//Игнорируем все соприкосновенния с пикапом здоровья
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//Функция SetVisibility имеется два аргумента, первый – это будет ли объект виден на уровне,
	//второй – это то, относится ли данное условие ко всем дочерним компонентам.
	GetRootComponent()->SetVisibility(false, true);
	//Запускаем таймер, который вернет на сцену новый объект пикапа здровья по окончании работы таймера
	FTimerHandle RespawnTimerHandle;
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &ALMAHealthPickup::RespawnPickup, RespawnTime);
}

void ALMAHealthPickup::RespawnPickup() 
{
	//Возвращаем показатели пикапа в начальное состояние, можно соприкасаться и можно видеть. 
	SphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	GetRootComponent()->SetVisibility(true, true);
}

