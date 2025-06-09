// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponSelection.h"
#include "ThegameCharacter.h"
#include "WeaponSystem.h"
#include "ComboSkillSystem.h"
#include "NPC.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UWeaponSelectionUI::NativeConstruct()
{
    Super::NativeConstruct();
    
    // Bind button click events
    if (WeaponAButton)
    {
        WeaponAButton->OnClicked.AddDynamic(this, &UWeaponSelectionUI::OnWeaponAClicked);
    }
    
    if (WeaponBButton)
    {
        WeaponBButton->OnClicked.AddDynamic(this, &UWeaponSelectionUI::OnWeaponBClicked);
    }
    
    if (WeaponCButton)
    {
        WeaponCButton->OnClicked.AddDynamic(this, &UWeaponSelectionUI::OnWeaponCClicked);
    }
    
    if (WeaponDButton)
    {
        WeaponDButton->OnClicked.AddDynamic(this, &UWeaponSelectionUI::OnWeaponDClicked);
    }
    
    if (ConfirmButton)
    {
        ConfirmButton->OnClicked.AddDynamic(this, &UWeaponSelectionUI::OnConfirmClicked);
    }
    
    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UWeaponSelectionUI::OnCancelClicked);
    }
    
    // Initialize with no selection
    SelectedAttackWeapon = EWeaponType::None;
    SelectedSupportWeapon = EWeaponType::None;
}
void UWeaponSelectionUI::OnWeaponAClicked()
{
    SelectAttackWeapon(EWeaponType::WeaponA);
}

void UWeaponSelectionUI::OnWeaponBClicked()
{
    SelectSupportWeapon(EWeaponType::WeaponB);
}

void UWeaponSelectionUI::OnWeaponCClicked()
{
    SelectAttackWeapon(EWeaponType::WeaponC);
}

void UWeaponSelectionUI::OnWeaponDClicked()
{
    SelectSupportWeapon(EWeaponType::WeaponD);
}

void UWeaponSelectionUI::OnConfirmClicked()
{
    ConfirmWeaponSelection();
}

void UWeaponSelectionUI::OnCancelClicked()
{
    CloseWeaponSelection();
}

// Fixes for WeaponSelectionUI to ensure all weapons A, B, C, and D can be used
// even if C and D don't have meshes yet

// Update the SelectAttackWeapon method to handle testing weapons without meshes
void UWeaponSelectionUI::SelectAttackWeapon(EWeaponType WeaponType)
{
    // For testing: Allow A and C to be attack weapons regardless of mesh availability
    if (WeaponType == EWeaponType::WeaponA || WeaponType == EWeaponType::WeaponC)
    {
        SelectedAttackWeapon = WeaponType;
        UpdateWeaponPreview();
        
        // Log the selection for debugging
        UE_LOG(LogTemp, Display, TEXT("Attack weapon selected: %d"), (int32)WeaponType);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to select non-attack weapon type: %d"), (int32)WeaponType);
    }
}

// Update the SelectSupportWeapon method to handle testing weapons without meshes
void UWeaponSelectionUI::SelectSupportWeapon(EWeaponType WeaponType)
{
    // For testing: Allow B and D to be support weapons regardless of mesh availability
    if (WeaponType == EWeaponType::WeaponB || WeaponType == EWeaponType::WeaponD)
    {
        SelectedSupportWeapon = WeaponType;
        UpdateWeaponPreview();
        
        // Log the selection for debugging
        UE_LOG(LogTemp, Display, TEXT("Support weapon selected: %d"), (int32)WeaponType);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Attempted to select non-support weapon type: %d"), (int32)WeaponType);
    }
}

// Update the SetupWeaponSelection method to enable all weapon buttons for testing
void UWeaponSelectionUI::SetupWeaponSelection(AThegameCharacter* PlayerCharacter)
{
   Player = PlayerCharacter;
    
    if (Player)
    {
        WeaponSystem = Player->WeaponSystem;
        
        if (WeaponSystem)
        {
            // Default to current weapons if any are equipped
            if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot1))
            {
                SelectedAttackWeapon = WeaponSystem->ActiveWeaponSlot1;
            }
            else if (WeaponSystem->IsAttackWeapon(WeaponSystem->ActiveWeaponSlot2))
            {
                SelectedAttackWeapon = WeaponSystem->ActiveWeaponSlot2;
            }
            else
            {
                SelectedAttackWeapon = EWeaponType::WeaponA; // Default to A if no attack weapon is equipped
            }
            
            if (WeaponSystem->IsSupportWeapon(WeaponSystem->ActiveWeaponSlot1))
            {
                SelectedSupportWeapon = WeaponSystem->ActiveWeaponSlot1;
            }
            else if (WeaponSystem->IsSupportWeapon(WeaponSystem->ActiveWeaponSlot2))
            {
                SelectedSupportWeapon = WeaponSystem->ActiveWeaponSlot2;
            }
            else
            {
                SelectedSupportWeapon = EWeaponType::WeaponB; // Default to B if no support weapon is equipped
            }
            
            // FOR TESTING: Enable all weapon buttons regardless of mesh availability
            if (WeaponCButton)
            {
                WeaponCButton->SetIsEnabled(true);
                UE_LOG(LogTemp, Display, TEXT("Weapon C button enabled for testing"));
            }
            
            if (WeaponDButton)
            {
                WeaponDButton->SetIsEnabled(true);
                UE_LOG(LogTemp, Display, TEXT("Weapon D button enabled for testing"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("WeaponSystem not found on player character"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No player character provided to SetupWeaponSelection"));
    }
    
    // Update the preview after initialization
    UpdateWeaponPreview();
    
    // Initialize the stat display
    UpdateStatDisplay();
}

// Update the UpdateWeaponPreview method to handle weapons without meshes or icons
void UWeaponSelectionUI::UpdateWeaponPreview()
{
    // Handle the case where WeaponSystem might be null
    if (!WeaponSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPreview: WeaponSystem is null"));
        return;
    }
    
    // Update attack weapon preview
    if (AttackWeaponPreview && SelectedAttackWeapon != EWeaponType::None)
    {
        FWeaponData AttackData = WeaponSystem->GetWeaponData(SelectedAttackWeapon);
        
        // For testing: Set a default color for the preview if no icon is available
        if (AttackData.WeaponIcon)
        {
            AttackWeaponPreview->SetBrushFromTexture(AttackData.WeaponIcon);
        }
        else
        {
            // Create a solid color brush for testing
            FLinearColor PreviewColor = (SelectedAttackWeapon == EWeaponType::WeaponA) 
                ? FLinearColor(0.5f, 0.2f, 0.2f, 1.0f)  // Red-ish for Weapon A
                : FLinearColor(0.2f, 0.2f, 0.5f, 1.0f); // Blue-ish for Weapon C
                
            AttackWeaponPreview->SetBrushTintColor(PreviewColor);
        }
    }
    
    // Update support weapon preview
    if (SupportWeaponPreview && SelectedSupportWeapon != EWeaponType::None)
    {
        FWeaponData SupportData = WeaponSystem->GetWeaponData(SelectedSupportWeapon);
        
        // For testing: Set a default color for the preview if no icon is available
        if (SupportData.WeaponIcon)
        {
            SupportWeaponPreview->SetBrushFromTexture(SupportData.WeaponIcon);
        }
        else
        {
            // Create a solid color brush for testing
            FLinearColor PreviewColor = (SelectedSupportWeapon == EWeaponType::WeaponB) 
                ? FLinearColor(0.2f, 0.5f, 0.2f, 1.0f)  // Green-ish for Weapon B
                : FLinearColor(0.5f, 0.5f, 0.2f, 1.0f); // Yellow-ish for Weapon D
                
            SupportWeaponPreview->SetBrushTintColor(PreviewColor);
        }
    }
    
    // Update weapon names
    if (AttackWeaponName && SelectedAttackWeapon != EWeaponType::None)
    {
        FWeaponData AttackData = WeaponSystem->GetWeaponData(SelectedAttackWeapon);
        
        // Use display name if available, otherwise use a default name
        if (!AttackData.DisplayName.IsEmpty())
        {
            AttackWeaponName->SetText(AttackData.DisplayName);
        }
        else
        {
            FString DefaultName = (SelectedAttackWeapon == EWeaponType::WeaponA) 
                ? "Sword (A)" 
                : "Attack Weapon C (Test)";
                
            AttackWeaponName->SetText(FText::FromString(DefaultName));
        }
    }
    
    if (SupportWeaponName && SelectedSupportWeapon != EWeaponType::None)
    {
        FWeaponData SupportData = WeaponSystem->GetWeaponData(SelectedSupportWeapon);
        
        // Use display name if available, otherwise use a default name
        if (!SupportData.DisplayName.IsEmpty())
        {
            SupportWeaponName->SetText(SupportData.DisplayName);
        }
        else
        {
            FString DefaultName = (SelectedSupportWeapon == EWeaponType::WeaponB) 
                ? "Shield (B)" 
                : "Support Weapon D (Test)";
                
            SupportWeaponName->SetText(FText::FromString(DefaultName));
        }
    }
    
    // Update combo name text if both weapons are selected
    if (ComboNameText && SelectedAttackWeapon != EWeaponType::None && SelectedSupportWeapon != EWeaponType::None && Player && Player->ComboSkillSystem)
    {
        FComboSkillData ComboData = Player->ComboSkillSystem->GetComboData(SelectedAttackWeapon, SelectedSupportWeapon);
        
        // If combo name is available, use it; otherwise create a test name
        if (!ComboData.ComboName.IsEmpty())
        {
            ComboNameText->SetText(FText::FromString(ComboData.ComboName));
        }
        else
        {
            // Create a test combo name using the weapon type letters
            FString AttackChar = (SelectedAttackWeapon == EWeaponType::WeaponA) ? "A" : "C";
            FString SupportChar = (SelectedSupportWeapon == EWeaponType::WeaponB) ? "B" : "D";
            FString TestComboName = FString::Printf(TEXT("Test Combo: %s+%s"), *AttackChar, *SupportChar);
            
            ComboNameText->SetText(FText::FromString(TestComboName));
        }
    }
     // Handle the case where WeaponSystem might be null
    if (!WeaponSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateWeaponPreview: WeaponSystem is null"));
        return;
    }
    
    // Update attack weapon preview
    if (AttackWeaponPreview && SelectedAttackWeapon != EWeaponType::None)
    {
        FWeaponData AttackData = WeaponSystem->GetWeaponData(SelectedAttackWeapon);
        
        // For testing: Set a default color for the preview if no icon is available
        if (AttackData.WeaponIcon)
        {
            AttackWeaponPreview->SetBrushFromTexture(AttackData.WeaponIcon);
        }
        else
        {
            // Create a solid color brush for testing
            FLinearColor PreviewColor = (SelectedAttackWeapon == EWeaponType::WeaponA) 
                ? FLinearColor(0.5f, 0.2f, 0.2f, 1.0f)  // Red-ish for Weapon A
                : FLinearColor(0.2f, 0.2f, 0.5f, 1.0f); // Blue-ish for Weapon C
                
            AttackWeaponPreview->SetBrushTintColor(PreviewColor);
        }
    }
    
    // Update support weapon preview
    if (SupportWeaponPreview && SelectedSupportWeapon != EWeaponType::None)
    {
        FWeaponData SupportData = WeaponSystem->GetWeaponData(SelectedSupportWeapon);
        
        // For testing: Set a default color for the preview if no icon is available
        if (SupportData.WeaponIcon)
        {
            SupportWeaponPreview->SetBrushFromTexture(SupportData.WeaponIcon);
        }
        else
        {
            // Create a solid color brush for testing
            FLinearColor PreviewColor = (SelectedSupportWeapon == EWeaponType::WeaponB) 
                ? FLinearColor(0.2f, 0.5f, 0.2f, 1.0f)  // Green-ish for Weapon B
                : FLinearColor(0.5f, 0.5f, 0.2f, 1.0f); // Yellow-ish for Weapon D
                
            SupportWeaponPreview->SetBrushTintColor(PreviewColor);
        }
    }
    
    // Update weapon names
    if (AttackWeaponName && SelectedAttackWeapon != EWeaponType::None)
    {
        FWeaponData AttackData = WeaponSystem->GetWeaponData(SelectedAttackWeapon);
        
        // Use display name if available, otherwise use a default name
        if (!AttackData.DisplayName.IsEmpty())
        {
            AttackWeaponName->SetText(AttackData.DisplayName);
        }
        else
        {
            FString DefaultName = (SelectedAttackWeapon == EWeaponType::WeaponA) 
                ? "Sword (A)" 
                : "Attack Weapon C (Test)";
                
            AttackWeaponName->SetText(FText::FromString(DefaultName));
        }
    }
    
    if (SupportWeaponName && SelectedSupportWeapon != EWeaponType::None)
    {
        FWeaponData SupportData = WeaponSystem->GetWeaponData(SelectedSupportWeapon);
        
        // Use display name if available, otherwise use a default name
        if (!SupportData.DisplayName.IsEmpty())
        {
            SupportWeaponName->SetText(SupportData.DisplayName);
        }
        else
        {
            FString DefaultName = (SelectedSupportWeapon == EWeaponType::WeaponB) 
                ? "Shield (B)" 
                : "Support Weapon D (Test)";
                
            SupportWeaponName->SetText(FText::FromString(DefaultName));
        }
    }
    
    // Update combo name text if both weapons are selected
    if (ComboNameText && SelectedAttackWeapon != EWeaponType::None && SelectedSupportWeapon != EWeaponType::None && Player && Player->ComboSkillSystem)
    {
        FComboSkillData ComboData = Player->ComboSkillSystem->GetComboData(SelectedAttackWeapon, SelectedSupportWeapon);
        
        // If combo name is available, use it; otherwise create a test name
        if (!ComboData.ComboName.IsEmpty())
        {
            ComboNameText->SetText(FText::FromString(ComboData.ComboName));
        }
        else
        {
            // Create a test combo name using the weapon type letters
            FString AttackChar = (SelectedAttackWeapon == EWeaponType::WeaponA) ? "A" : "C";
            FString SupportChar = (SelectedSupportWeapon == EWeaponType::WeaponB) ? "B" : "D";
            FString TestComboName = FString::Printf(TEXT("Test Combo: %s+%s"), *AttackChar, *SupportChar);
            
            ComboNameText->SetText(FText::FromString(TestComboName));
        }
    }
    
    // NEW CODE: Update the stat display with the currently selected weapons
    UpdateStatDisplay();
}

// Update the ConfirmWeaponSelection method to handle test weapons
void UWeaponSelectionUI::ConfirmWeaponSelection()
{
     if (Player && SelectedAttackWeapon != EWeaponType::None && SelectedSupportWeapon != EWeaponType::None)
    {
        // For testing, always allow the selection to proceed
        UE_LOG(LogTemp, Display, TEXT("Confirming weapon selection: Attack=%d, Support=%d"), 
            (int32)SelectedAttackWeapon, (int32)SelectedSupportWeapon);
            
        // Apply the weapon selection to the player
        Player->SelectWeapons(SelectedAttackWeapon, SelectedSupportWeapon);
        
        // Find the owning NPC to properly close the weapon selection
        TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), FoundActors);
        
        bool bFoundNPC = false;
        for (AActor* Actor : FoundActors)
        {
            ANPC* NPC = Cast<ANPC>(Actor);
            if (NPC && NPC->IsWeaponSelectionWidgetInstanceActive(this))
            {
                // Call the NPC's method to properly close the selection
                UE_LOG(LogTemp, Display, TEXT("Found owning NPC, calling CloseWeaponSelection"));
                NPC->CloseWeaponSelection();
                bFoundNPC = true;
                break;
            }
        }
        
        // If we didn't find the NPC, just remove ourselves from parent
        if (!bFoundNPC)
        {
            UE_LOG(LogTemp, Warning, TEXT("Could not find owning NPC, just closing widget"));
            RemoveFromParent();
            
            // Try to restore player input as a fallback
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (PlayerController && Player)
            {
                PlayerController->SetInputMode(FInputModeGameAndUI());
                PlayerController->bShowMouseCursor = true;
                Player->EnableInput(PlayerController);
                UE_LOG(LogTemp, Display, TEXT("Fallback: Re-enabled player input"));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot confirm weapon selection: Invalid player or weapon selection"));
    }
}

void UWeaponSelectionUI::CloseWeaponSelection()
{
    // This should now be simpler since ConfirmWeaponSelection handles the NPC notification
    RemoveFromParent();
    
    // For cancel button, we still need to find the NPC
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), FoundActors);
    
    for (AActor* Actor : FoundActors)
    {
        ANPC* NPC = Cast<ANPC>(Actor);
        if (NPC && NPC->IsWeaponSelectionWidgetInstanceActive(this))
        {
            // Call the NPC's method to properly close the selection
            UE_LOG(LogTemp, Display, TEXT("Found owning NPC on cancel, calling CloseWeaponSelection"));
            NPC->CloseWeaponSelection();
            return;
        }
    }
    
    // Fallback if NPC not found
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController && Player)
    {
        PlayerController->SetInputMode(FInputModeGameAndUI());
        PlayerController->bShowMouseCursor = true;
        Player->EnableInput(PlayerController);
        UE_LOG(LogTemp, Display, TEXT("Fallback: Re-enabled player input on cancel"));
    }
}
void UWeaponSelectionUI::UpdateStatDisplay()
{
    if (!Player || !WeaponSystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("UpdateStatDisplay: Player or WeaponSystem is null"));
        return;
    }

    // Get base stats from player character
    float BaseMaxHealth = Player->BaseMaxHealth;
    float BaseSpeed = Player->BaseSpeed;
    float BaseDamage = Player->BaseDamageAmount;

    // Display base stats
    if (BaseHPText)
    {
        BaseHPText->SetText(FText::FromString(FString::Printf(TEXT("HP: %.0f"), BaseMaxHealth)));
    }
    
    if (BaseSpeedText)
    {
        BaseSpeedText->SetText(FText::FromString(FString::Printf(TEXT("Speed: %.0f"), BaseSpeed)));
    }
    
    if (BaseAttackText)
    {
        BaseAttackText->SetText(FText::FromString(FString::Printf(TEXT("Attack: %.0f"), BaseDamage)));
    }

    // Calculate and display buff stats based on selected weapons
    float HealthBonus = 0.0f;
    float SpeedMultiplier = 1.0f;
    float DamageMultiplier = 1.0f;

    // Check attack weapon buffs
    if (SelectedAttackWeapon != EWeaponType::None)
    {
        switch (SelectedAttackWeapon)
        {
        case EWeaponType::WeaponA: // Sword
            // Increase damage by 30%
            DamageMultiplier = 1.3f;
            break;
        case EWeaponType::WeaponC:
            // Increase damage by 20%
            DamageMultiplier = 1.2f;
            break;
        default:
            break;
        }
    }

    // Check support weapon buffs
    if (SelectedSupportWeapon != EWeaponType::None)
    {
        switch (SelectedSupportWeapon)
        {
        case EWeaponType::WeaponB: // Shield
            // Increase health by 40
            HealthBonus += 40.0f;
            break;
        case EWeaponType::WeaponD:
            // Increase speed by 10% and health by 20
            SpeedMultiplier = 1.1f;
            HealthBonus += 20.0f;
            break;
        default:
            break;
        }
    }

    // Display buff changes
    if (BuffHPText)
    {
        if (HealthBonus > 0)
        {
            BuffHPText->SetText(FText::FromString(FString::Printf(TEXT("(+%.0f)"), HealthBonus)));
        }
        else
        {
            BuffHPText->SetText(FText::FromString(TEXT("")));
        }
    }
    
    if (BuffSpeedText)
    {
        if (SpeedMultiplier > 1.0f)
        {
            float SpeedBonus = BaseSpeed * (SpeedMultiplier - 1.0f);
            BuffSpeedText->SetText(FText::FromString(FString::Printf(TEXT("(+%.0f)"), SpeedBonus)));
        }
        else
        {
            BuffSpeedText->SetText(FText::FromString(TEXT("")));
        }
    }
    
    if (BuffAttackText)
    {
        if (DamageMultiplier > 1.0f)
        {
            float DamageBonus = BaseDamage * (DamageMultiplier - 1.0f);
            BuffAttackText->SetText(FText::FromString(FString::Printf(TEXT("(+%.0f)"), DamageBonus)));
        }
        else
        {
            BuffAttackText->SetText(FText::FromString(TEXT("")));
        }
    }
}