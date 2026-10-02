#ifndef GUARD_SECRET_BASE_H
#define GUARD_SECRET_BASE_H

#if FREE_SECRET_BASES == FALSE
void HideSecretBaseDecorationSprites(void);
void CopyCurSecretBaseOwnerName_StrVar1(void);
void ClearJapaneseSecretBases(struct SecretBase *bases);
void SetPlayerSecretBaseParty(void);
u8 *GetSecretBaseMapName(u8 *dest);
const u8 *GetSecretBaseTrainerLoseText(void);
void SetOccupiedSecretBaseEntranceMetatiles(struct MapEvents const *events);
void InitSecretBaseAppearance(bool8 hidePC);
bool8 CurMapIsSecretBase(void);
#endif
void SecretBasePerStepCallback(u8 taskId);
#if FREE_SECRET_BASES == FALSE
bool8 TrySetCurSecretBase(void);
void CheckInteractedWithFriendsPosterDecor(void);
void CheckInteractedWithFriendsFurnitureBottom(void);
void CheckInteractedWithFriendsFurnitureMiddle(void);
void CheckInteractedWithFriendsFurnitureTop(void);
void WarpIntoSecretBase(const struct MapPosition *position, const struct MapEvents *events);
#endif
bool8 SecretBaseMapPopupEnabled(void);
#if FREE_SECRET_BASES == FALSE
void CheckLeftFriendsSecretBase(void);
void ClearSecretBases(void);
void SetCurSecretBaseIdFromPosition(const struct MapPosition *position, const struct MapEvents *events);
void TrySetCurSecretBaseIndex(void);
#endif
void CheckPlayerHasSecretBase(void);
#if FREE_SECRET_BASES == FALSE
void ToggleSecretBaseEntranceMetatile(void);
void ReceiveSecretBasesData(void *secretBases, size_t recordSize, u8 linkIdx);

#endif // FREE_SECRET_BASES == FALSE

#endif
