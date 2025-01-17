#include "FPSTemplate.h"
#include "BasePlayerController.h"
#include "UnrealNetwork.h"
#include "HUD/TeamSelectionHUD.h"
#include "HUD/SpawnHUD.h"
#include "HUD/PlayerHUD.h"
#include "HUD/DeathHUD.h"
#include "HUD/SpectatorHUD.h"
#include "HUD/MatchEndHUD.h"
#include "HUD/MenuHUD.h"
#include "Game/FPSGameMode.h"
#include "BaseWorldSettings.h"
#include "Damage/DamageType_Suicide.h"
#include "Game/FPSGameMode.h"
#include "StaticFunctionLibrary.h"
#include "Items/Firearm.h"

ABasePlayerController::ABasePlayerController()
{
	static ConstructorHelpers::FObjectFinder<UObject> NewGameOverlayWidgetClass(TEXT("WidgetBlueprint'/Game/FPSTemplate/HUD/Widgets/WBP_GameOverlay.WBP_GameOverlay_C'"));
	GameOverlayWidgetClass = (UClass*)NewGameOverlayWidgetClass.Object;

	bAutoManageActiveCameraTarget = false;

	bLoadoutChanged = true;
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;



	// Create GameOverlayWidget
	GameOverlayWidget = CreateWidget<UGameOverlayWidget>(this, GameOverlayWidgetClass);
	if (GameOverlayWidget)
	{
		GameOverlayWidget->AddToViewport(10);
	}

	// Init name
	//SetName(UStaticFunctionLibrary::GetPlayerName());

	// Fade vision in
	FadeIn();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ABasePlayerController::OpenScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &ABasePlayerController::CloseScoreboard);

	InputComponent->BindAction("Menu", IE_Pressed, this, &ABasePlayerController::OpenMenu);

	InputComponent->BindAction("EnlargeMiniMap", IE_Pressed, this, &ABasePlayerController::EnlargeMiniMap);
	InputComponent->BindAction("EnlargeMiniMap", IE_Released, this, &ABasePlayerController::MinimizeMiniMap);

}

void ABasePlayerController::Reset()
{
	// Do nothing here
}

void ABasePlayerController::ClientReset_Implementation()
{
	// Neither here
}

void ABasePlayerController::ResetScore()
{
	ABasePlayerState * PS = Cast<ABasePlayerState>(PlayerState);
	if (PS)
	{
		PS->Score = 0.f;
		PS->Kills = 0;
		PS->Deaths = 0;
	}
}

void ABasePlayerController::UnFreeze()
{
	// Move from death state to entry state
	if (State == EPlayerStateEnum::Death)
	{
		AFPSGameMode * GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->MovePlayerToEntryState(this);
		}
	}
}

void ABasePlayerController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	if (InPawn)
	{
		ClientSetViewTarget(InPawn);
	}
}

void ABasePlayerController::FailedToSpawnPawn()
{
	// Do nothing
}

void ABasePlayerController::SetTeam(ETeamEnum InTeam)
{
	ABasePlayerState * BasePlayerState = Cast<ABasePlayerState>(PlayerState);
	if (BasePlayerState)
	{
		BasePlayerState->SetTeam(InTeam);
	}
}

void ABasePlayerController::AddScore(int32 Val)
{
	ABasePlayerState * PS = Cast<ABasePlayerState>(PlayerState);
	if (PS)
	{
		PS->Score += Val;
	}
}

void ABasePlayerController::AddKill()
{
	ABasePlayerState * PS = Cast<ABasePlayerState>(PlayerState);
	for (TObjectIterator<AFirearm> Itr; Itr; ++Itr)
	{
		if (Itr->IsA(AFirearm::StaticClass()))
		{
			AFirearm* firearm = *Itr;
			firearm->moremaganize();
		}
	}
	if (PS)
	{
		PS->Kills++;
	}
}

void ABasePlayerController::AddDeath()
{
	ABasePlayerState * PS = Cast<ABasePlayerState>(PlayerState);
	if (PS)
	{
		PS->Deaths++;
	}
}

void ABasePlayerController::ClientOnPlayerDied_Implementation(TSubclassOf<UBaseDamageType> DamageTypeClass, ABasePlayerState * SoldierPlayerState, ABasePlayerState * KillerPlayerState)
{
	if (GameOverlayWidget && SoldierPlayerState)
	{
		// Forward kill notification to kill feed
		GameOverlayWidget->OnPlayerDied(DamageTypeClass.GetDefaultObject(), SoldierPlayerState, KillerPlayerState);

		// Check if this PC is the killer
		if (KillerPlayerState && KillerPlayerState == PlayerState)
		{
			UBaseDamageType * DamageTypeCDO = DamageTypeClass.GetDefaultObject();
			if (SoldierPlayerState != KillerPlayerState && SoldierPlayerState->IsEnemyFor(KillerPlayerState))
			{
				// Add kill notification
				GameOverlayWidget->OnKilledEnemy(DamageTypeCDO, SoldierPlayerState, DamageTypeCDO != NULL ? DamageTypeCDO->KillReward : 0);
			}
			else
			{
				// Add team kill notification
				GameOverlayWidget->OnKilledTeammate(DamageTypeCDO, SoldierPlayerState, -100);
			}
		}

		if (SoldierPlayerState == PlayerState)
		{
			// Set LastKillerPlayer for DeathHUD
			LastKillerPlayer = KillerPlayerState;
		}
	}
}

bool ABasePlayerController::ClientOnPlayerDied_Validate(TSubclassOf<UBaseDamageType> DamageTypeClass, ABasePlayerState * SoldierPlayerState, ABasePlayerState * KillerPlayerState)
{
	return true;
}

void ABasePlayerController::ClientOnAssistedKill_Implementation(int32 DamageAmount)
{
	if (GameOverlayWidget)
	{
		GameOverlayWidget->OnScored(NSLOCTEXT("HUD", "Kill Assist", "Kill Assist"), DamageAmount);
	}
}

bool ABasePlayerController::ClientOnAssistedKill_Validate(int32 DamageAmount)
{
	return true;
}

void ABasePlayerController::ClientOnDamagedSoldier_Implementation(ASoldier * Soldier, float DamageAmount)
{
	if (Soldier && Soldier != GetPawn() && Soldier->IsEnemyFor(this))
	{
		APlayerHUD * HUD = Cast<APlayerHUD>(MyHUD);
		if (HUD)
		{
			// Notify HUD that an enemy was hit by the player
			HUD->NotifyHitEnemy(DamageAmount);
			//Soldier->Worked();
		}
	}
}

bool ABasePlayerController::ClientOnDamagedSoldier_Validate(ASoldier * Soldier, float DamageAmount)
{
	return true;
}

void ABasePlayerController::OnTookDamage(float DamageAmount, AActor * Causer)
{
	if (GameOverlayWidget)
	{
		GameOverlayWidget->OnTookDamage(DamageAmount, Causer);
	}
}

void ABasePlayerController::OnClientHitActor(AActor * DamagedActor, const FPointDamageEvent & DamageEvent, AActor * DamageCauser)
{
	// Send client hit info to server
	ServerOnClientHitActor(DamagedActor, DamageEvent, DamageCauser);
}

void ABasePlayerController::ServerOnClientHitActor_Implementation(AActor * DamagedActor, const FPointDamageEvent & DamageEvent, AActor * DamageCauser)
{
	if (DamagedActor)
	{
		DamagedActor->TakeDamage(DamageEvent.Damage, DamageEvent, this, DamageCauser);
	}
}

bool ABasePlayerController::ServerOnClientHitActor_Validate(AActor * DamagedActor, const FPointDamageEvent & DamageEvent, AActor * DamageCauser)
{
	return true;
}

void ABasePlayerController::ClientOnRoundOver_Implementation(ETeamEnum WinnerTeam)
{
	if (GameOverlayWidget)
	{
		GameOverlayWidget->OnRoundOver(WinnerTeam == GetTeam());
	}
}

ETeamEnum ABasePlayerController::GetTeam() const
{
	ABasePlayerState * PS = Cast<ABasePlayerState>(PlayerState);
	if (PS)
	{
		return PS->GetTeam();
	}

	return ETeamEnum::None;
}

bool ABasePlayerController::IsEnemyFor(ABasePlayerController * PC) const
{
	ABasePlayerState * PS = Cast<ABasePlayerState>(PlayerState);
	if (PS)
	{
		return PS->IsEnemyFor(PC ? Cast<ABasePlayerState>(PC->PlayerState) : NULL);
	}

	return false;
}


void ABasePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABasePlayerController, State);
}

void ABasePlayerController::EnterTeamSelectionState()
{
	State = EPlayerStateEnum::TeamSelection;

	if (IsLocalController())
	{
		OnEnterTeamSelectionState();
	}
}

void ABasePlayerController::EnterSpawnState()
{
	State = EPlayerStateEnum::Spawn;

	if (IsLocalController())
	{
		OnEnterSpawnState();
	}
}

void ABasePlayerController::EnterPlayingState()
{
	State = EPlayerStateEnum::Playing;

	if (IsLocalController())
	{
		OnEnterPlayingState();
	}
}

void ABasePlayerController::EnterDeathState()
{
	State = EPlayerStateEnum::Death;

	BeginInactiveState();

	if (IsLocalController())
	{
		OnEnterDeathState();
	}
}

void ABasePlayerController::EnterSpectatingState()
{
	State = EPlayerStateEnum::Spectating;

	if (IsLocalController())
	{
		OnEnterSpectatingState();
	}
}

void ABasePlayerController::EnterMatchEndState()
{
	State = EPlayerStateEnum::MatchEnd;

	// Unpossess current pawn
	UnPossess();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ChangeState);

	if (IsLocalController())
	{
		OnEnterMatchEndState();
	}
}

void ABasePlayerController::OnEnterTeamSelectionState()
{
	OnEnterState(ATeamSelectionHUD::StaticClass(), true);
	SetViewTargetToMenuCamera();
}

void ABasePlayerController::OnEnterSpawnState()
{
	bCustomizing = false;

	OnEnterState(ASpawnHUD::StaticClass(), true);
	SetViewTargetToMenuCamera();
}

void ABasePlayerController::OnEnterPlayingState()
{
	OnEnterState(APlayerHUD::StaticClass(), false);
}

void ABasePlayerController::OnEnterDeathState()
{
	OnEnterState(ADeathHUD::StaticClass(), false);
}

void ABasePlayerController::OnEnterSpectatingState()
{
	OnEnterState(ASpectatorHUD::StaticClass(), true);
}

void ABasePlayerController::OnEnterMatchEndState()
{
	OnEnterState(AMatchEndHUD::StaticClass(), true);
	SetViewTargetToMenuCamera();
	CloseMenu();
	OpenScoreboard();
}

void ABasePlayerController::OnEnterState(TSubclassOf<AHUD> HUDClass, bool bMouseEnabled)
{
	if (!bMenuOpen)
	{
		ClientSetHUD(HUDClass);

		bShowMouseCursor = bMouseEnabled;
		if (bMouseEnabled)
		{
			SetInputMode(FInputModeGameAndUI());
		}
		else
		{
			SetInputMode(FInputModeGameOnly());
		}
	}
}

void ABasePlayerController::SetViewTargetToMenuCamera()
{
	ABaseWorldSettings * WorldSettings = Cast<ABaseWorldSettings>(GetWorld()->GetWorldSettings());
	if (WorldSettings && WorldSettings->MenuCameras.Num() > 0 && !WorldSettings->MenuCameras.Contains(GetViewTarget()))
	{
		SetViewTarget(WorldSettings->MenuCameras[FMath::RandRange(0, WorldSettings->MenuCameras.Num() - 1)]);
	}
}

void ABasePlayerController::SwitchToNextCamera()
{
	if (State == EPlayerStateEnum::Spawn)
	{

		ABaseWorldSettings * WorldSettings = Cast<ABaseWorldSettings>(GetWorld()->GetWorldSettings());
		int32 CameraIndex;
		if (WorldSettings && WorldSettings->MenuCameras.Find(Cast<ACameraActor>(GetViewTarget()), CameraIndex))
		{
			++CameraIndex;
			if (CameraIndex > WorldSettings->MenuCameras.Num() - 1)
			{
				CameraIndex = 0;
			}

			SetViewTarget(WorldSettings->MenuCameras[CameraIndex]);
			FadeIn();
		}
	}
}

void ABasePlayerController::FadeIn()
{
	ClientSetCameraFade(true, FColor::Black, FVector2D(1.f, 0.f), 4.f, true);
}

void ABasePlayerController::Suicide()
{
	ASoldier * Soldier = Cast<ASoldier>(GetPawn());
	if (Soldier)
	{
		FPointDamageEvent DamageEvent;
		DamageEvent.DamageTypeClass = UDamageType_Suicide::StaticClass();
		DamageEvent.Damage = 100.f;

		if (GetNetMode() == NM_Client)
		{
			// Let the server apply damage if we are on a client
			ServerOnClientHitActor(Soldier, DamageEvent, NULL);
		}
		else
		{
			Soldier->TakeDamage(DamageEvent.Damage, DamageEvent, this, NULL);
		}
	}
}


void ABasePlayerController::OpenScoreboard()
{
	if (GameOverlayWidget && State != EPlayerStateEnum::MatchEnd)
	{
		GameOverlayWidget->OpenScoreboard();
	}
}

void ABasePlayerController::CloseScoreboard()
{
	if (GameOverlayWidget && State != EPlayerStateEnum::MatchEnd)
	{
		GameOverlayWidget->CloseScoreboard();
	}
}

void ABasePlayerController::EnlargeMiniMap()
{
	if (State == EPlayerStateEnum::Playing)
	{
		bMiniMapEnlarged = true;
	}
}

void ABasePlayerController::MinimizeMiniMap()
{
	if (State == EPlayerStateEnum::Playing)
	{
		bMiniMapEnlarged = false;
	}
}

void ABasePlayerController::OpenMenu()
{
	if (!bMenuOpen && !bCustomizing)
	{
		bMenuOpen = true;

		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());

		if (GameOverlayWidget)
		{
			GameOverlayWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		ClientSetHUD(AMenuHUD::StaticClass());
	}
}

void ABasePlayerController::CloseMenu()
{
	if (bMenuOpen)
	{
		bMenuOpen = false;

		if (GameOverlayWidget)
		{
			GameOverlayWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		OnRep_State();
	}
}

void ABasePlayerController::JoinTeam(ETeamEnum InTeam)
{
	if (GetNetMode() == NM_Client)
	{
		ServerJoinTeam(InTeam);
		return;
	}

	AFPSGameMode * GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->PlayerJoinTeam(this, InTeam);
	}
}

void ABasePlayerController::ServerJoinTeam_Implementation(ETeamEnum InTeam)
{
	JoinTeam(InTeam);
}

bool ABasePlayerController::ServerJoinTeam_Validate(ETeamEnum InTeam)
{
	return true;
}

void ABasePlayerController::AutoJoinTeam()
{
	if (GetNetMode() == NM_Client)
	{
		ServerAutoJoinTeam();
		return;
	}

	AFPSGameMode * GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->PlayerAutoJoinTeam(this);
	}
}

void ABasePlayerController::ServerAutoJoinTeam_Implementation()
{
	AutoJoinTeam();
}

bool ABasePlayerController::ServerAutoJoinTeam_Validate()
{
	return true;
}

void ABasePlayerController::Spawn()
{
	if (GetNetMode() == NM_Client)
	{
		if (bLoadoutChanged)
		{
			ServerSetLoadout(Loadout, LoadoutSlot);
			bLoadoutChanged = false;
		}

		ServerSpawn();
		return;
	}

	AFPSGameMode * GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->PlayerSpawn(this);
	}
}

void ABasePlayerController::ServerSpawn_Implementation()
{
	Spawn();
}

bool ABasePlayerController::ServerSpawn_Validate()
{
	return true;
}


void ABasePlayerController::OnRep_State()
{
	switch (State)
	{
	case (EPlayerStateEnum::TeamSelection) :
		OnEnterTeamSelectionState();
		break;
	case (EPlayerStateEnum::Spawn) :
		OnEnterSpawnState();
		break;
	case (EPlayerStateEnum::Playing) :
		OnEnterPlayingState();
		break;
	case (EPlayerStateEnum::Death) :
		OnEnterDeathState();
		break;
	case (EPlayerStateEnum::Spectating) :
		OnEnterSpectatingState();
		break;
	case (EPlayerStateEnum::MatchEnd) :
		OnEnterMatchEndState();
		break;
	}
}

void ABasePlayerController::SetLoadout(const FLoadoutStruct & NewLoadout, uint8 NewLoadoutSlot)
{
	bLoadoutChanged = true;
	Loadout = NewLoadout;
	LoadoutSlot = NewLoadoutSlot;
}

void ABasePlayerController::ApplyLoadout(const FLoadoutStruct & NewLoadout, uint8 NewLoadoutSlot)
{
	Loadout = NewLoadout;
	LoadoutSlot = NewLoadoutSlot;
}

void ABasePlayerController::ServerSetLoadout_Implementation(const FLoadoutStruct & NewLoadout, uint8 NewLoadoutSlot)
{
	AFPSGameMode * GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		// Let the game mode check the loadout to ensure player didn't send an invalid one
		GameMode->PlayerSetLoadout(this, NewLoadout, NewLoadoutSlot);
	}
}

bool ABasePlayerController::ServerSetLoadout_Validate(const FLoadoutStruct & NewLoadout, uint8 NewLoadoutSlot)
{
	return true;
}