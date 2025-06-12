#pragma once
#include "sdk.h"
#include <iostream>
#include "offsets.h"
using namespace Globals;
using namespace Camera;
using namespace UE4;

GWorld* UWorld;
GGameInstance* UGameInstance;
GLocalPlayer* ULocalPlayer;
GPlayerController* APlayerController;
GPawn* APawn;
GPrivatePawn* APrivatePawn;
GULevel* ULevel;
GUSkeletalMeshComponent* USkeletalMeshComponent;

bool cached = false;
uintptr_t WorldPtr;
uintptr_t PML4Base = 0;
#define uint unsigned int
#define ushort unsigned short
#define ulong unsigned long
struct FNameEntryHandle {
	uint16_t bIsWide : 1;
	uint16_t Len : 15;
};
struct FNameEntry {

	int32_t ComparisonId;
	FNameEntryHandle Header;
	union
	{
		char    AnsiName[1024]; 
		wchar_t    WideName[1024];
	};


	wchar_t const* GetWideName() const { return WideName; }
	char const* GetAnsiName() const { return AnsiName; }
	bool IsWide() const { return Header.bIsWide; }
};


bool IsCoreIsolationEnabled()
{
	
	HKEY hKey;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\DeviceGuard\\Scenarios\\HypervisorEnforcedCodeIntegrity", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		return false;
	}

	
	DWORD enabled = 0;
	DWORD size = sizeof(enabled);
	if (RegQueryValueExA(hKey, "Enabled", nullptr, nullptr, reinterpret_cast<LPBYTE>(&enabled), &size) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return false;
	}


	RegCloseKey(hKey);


	return (enabled != 0);
}
typedef struct ShadowRegionsDataStructure
{
	uintptr_t OriginalPML4_t;
	uintptr_t ClonedPML4_t;
	uintptr_t GameCr3;
	uintptr_t ClonedCr3;
	uintptr_t FreeIndex;
} ShadowRegionsDataStructure;
typedef struct _gr {
	ULONGLONG* guarded;
} gr, * pgr;
inline uint64_t ROL8(uint64_t value, unsigned int shift) {
	const unsigned int mask = 63; 
	shift &= mask;             
	return (value << shift) | (value >> (64 - shift));
}
__forceinline __int64 decrypt_xor_keys(const uint32_t key, const uintptr_t* state)
{
	__int64 v2;
	unsigned __int64 v3;
	unsigned __int64 v4;
	unsigned __int64 v5;
	unsigned int v6;
	__int64 v7;
	unsigned int v8;
	int v9;
	unsigned int v10;
	unsigned __int64 v11;



	v2 = (unsigned int)(key << 25);
	v3 = 2685821657736338717i64 * ((unsigned int)key ^ v2 ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12)) % 7;
	v4 = state[v3];
	v5 = (2685821657736338717i64 * ((unsigned int)key ^ v2 ^ (((unsigned int)key ^ ((unsigned __int64)(unsigned int)key >> 15)) >> 12))) >> 32;
	v6 = (unsigned int)v3 % 7;
	if (!((unsigned int)v3 % 7))
	{
		v7 = 2 * ((2 * v4) ^ ((2 * v4) ^ (v4 >> 1)) & 0x5555555555555555i64);
		v4 = v7 ^ (v7 ^ (((2 * v4) ^ ((2 * v4) ^ (v4 >> 1)) & 0x5555555555555555i64) >> 1)) & 0x5555555555555555i64;
		goto LABEL_5;
	}
	if (v6 == 1)
	{
		v9 = 2 * v3;
		v8 = v5 + v3;
		v4 = ~(unsigned __int64)(unsigned int)(v5 + v3) ^ (v4 - (unsigned int)(v5 + 2 * v3));
	}
	else
	{
	LABEL_5:
		v8 = v5 + v3;
		v9 = 2 * v3;
	}
	if (v6 == 2)
	{
		v4 = ROL8((2 * v4) ^ ((2 * v4) ^ (v4 >> 1)) & 0x5555555555555555i64, (unsigned __int8)(v8 % 0x3F) + 1);
		goto LABEL_8;
	}
	if (v6 == 3)
	{
		v10 = v5 + v9;
		v4 = ROL8((unsigned int)(v5 + v9) + v4, (unsigned __int8)(v8 % 0x3F) + 1);
	}
	else
	{
	LABEL_8:
		v10 = v9 + v5;
	}
	if (v6 == 4)
	{
		v4 ^= v8;
	LABEL_11:
		if (v6 == 6)
		{
			v11 = ROL8(v4, (unsigned __int8)(v10 % 0x3F) + 1);
			v4 = (2 * v11) ^ ((2 * v11) ^ (v11 >> 1)) & 0x5555555555555555i64;
		}

	}
	else
	{
		if (v6 != 5)
			goto LABEL_11;
		v4 += v8 - (unsigned __int64)v10;

	}
	return (v4 ^ key);

}



uintptr_t DecryptClonedCr3(ShadowRegionsDataStructure SR)
{
	uintptr_t m_VgkAddr = (uintptr_t)driver.get_module_base("vgk.sys");
	const uint8_t b = driver.readv<BYTE>(m_VgkAddr + 0x71C10);
	const uint64_t key = driver.readv<uint64_t>(m_VgkAddr + 0x71CD0);
	const uintptr_t cr3 = SR.ClonedCr3;

	const uint64_t inv_b = ~static_cast<uint64_t>(b);
	const uint64_t masked_and = static_cast<uint64_t>(b & cr3);
	const uint64_t masked_or = cr3 | inv_b;

	const int64_t term1 = (masked_and - masked_or);
	const int64_t term2 = (cr3 ^ b);
	const int64_t term3 = -2 * (b ^ (cr3 | b));
	const int64_t term4 = 2 * (((b | 0xE8ull) ^ 0xFFFFFFFFFFFFFF17ull) + (~cr3 | (b ^ 0xE8ull)));
	const int64_t term5 = -static_cast<int64_t>(b ^ (~cr3 ^ 0xE8ull));
	const int64_t term6 = -3 * ~cr3;
	const int64_t term7 = -232;

	const int64_t multiplierA = term2 + term3 + term4 + term5 + term6 + term7;
	const uintptr_t v5 = (0x49B74B6480000000ull * cr3 + 0xC2D8B464B4418C6Cull * inv_b + 0x66B8CDC1FFFFFFFFull * b + 0x5C1FE6A2B4418C6Dull * term1) * multiplierA;
	const uint64_t innerMul = 0x13D0F34E00000000ull * cr3 + 0x483C4F8900000000ull;
	const uint64_t cr3MulInner = cr3 * innerMul;
	const uint64_t v6_part1 = ((cr3 ^ cr3MulInner) << 63);
	const uint64_t v6_part2 = cr3 * (0x7D90DC33C620C593ull * innerMul + 0x8000000000000000ull);
	const uint64_t term_cr3_qword = 0x55494E5B80000000ull * key + 0xC83B18136241A38Dull * ~key + 0xCE3CE5E180000000ull * ~cr3 + 0x72F1C9B7E241A38Dull * ((key | 0xE8ull) - (231ull - (key & 0xE8ull)));
	const uint64_t term_key_masked = 0x99BF7D2380CF6EC3ull * key + 0x664082DC7F30913Eull * (cr3 | b) + 0x19BF7D2380CF6EC2ull * ~key + 0xE64082DC7F30913Eull * (~cr3 & ~static_cast<uint64_t>(b)) + ((cr3 + (b & (key ^ cr3)) + (key | b)) << 63);
	const uintptr_t v6 = v6_part1 + v6_part2 + (cr3 * term_cr3_qword + 0x71C31A1E80000000ull) * term_key_masked;
	const uint64_t common_expr = 0x8000000000000001ull * key + 0x2FDBF65F8A4AC9C9ull * cr3 + ((key ^ cr3) << 63) + 0x502409A075B53637ull * cr3;
	const uint64_t inner_expr = 0xFD90DC33C620C592ull * ~cr3MulInner + v6 + 0x2183995CC620C592ull;
	const uintptr_t result = 0x137FEEF6AB38CFB4ull * v5 + ((~v5 ^ ~(common_expr * inner_expr)) << 63) + 0x6C80110954C7304Dull * ((v5 & key) - (~key & ~v5) - key) - 0x7FFFFFFFFFFFFFFFull * common_expr * inner_expr - 0x4F167C5CD4C7304Eull;
	return result;
}
uintptr_t context_cr3;
struct ShadowData
{
	uintptr_t DecryptedClonedCr3;
	uintptr_t PML4Base;
};


ShadowData GetVgkShadowData(uintptr_t VgkAddress)
{
	ShadowRegionsDataStructure Data = driver.readv<ShadowRegionsDataStructure>(VgkAddress + 0x71B48);
	printf("ClonedCr3: %p\n", Data.ClonedCr3);
	printf("FreeIndex: %p\n", Data.FreeIndex);

	uintptr_t DecryptedCr3 = DecryptClonedCr3(Data);

	auto dirbase = (PVOID)DecryptedCr3;

	printf("DecryptedCr3: %p\n", DecryptedCr3);
	return ShadowData{ DecryptedCr3, Data.FreeIndex << 0x27 };
}
uintptr_t GetPML4Base()
{
	uintptr_t m_VgkAddr = (uintptr_t)driver.get_module_base("vgk.sys");
	printf("m_VgkAddr: %p\n", m_VgkAddr);

	ShadowData Data = GetVgkShadowData(m_VgkAddr);

	driver._dirbase = Data.DecryptedClonedCr3;


	return Data.PML4Base;
}




class PrecisePatternUWorldFinder {
private:
	uintptr_t pml4_base;
	uintptr_t pattern_prefix;
	std::vector<uintptr_t> priority_offsets;

	void initializeOffsets() {
		priority_offsets.clear();

		priority_offsets.push_back(0x170);
		priority_offsets.push_back(0x100);
		priority_offsets.push_back(0x130);
		priority_offsets.push_back(0x160);
		priority_offsets.push_back(0x190);
		priority_offsets.push_back(0x40);
		priority_offsets.push_back(0xC0);
		priority_offsets.push_back(0xE0);
		priority_offsets.push_back(0xA0);
		priority_offsets.push_back(0xB0);
		priority_offsets.push_back(0xD0);
		priority_offsets.push_back(0xF0);
		priority_offsets.push_back(0x1a0);

		for (uintptr_t offset = 0x10; offset <= 0x300; offset += 0x10) {
			if (std::find(priority_offsets.begin(), priority_offsets.end(), offset) == priority_offsets.end()) {
				priority_offsets.push_back(offset);
			}
		}
	}

	bool matchesExactPattern(uintptr_t addr) {
		if (addr == 0) return false;
		if ((addr & 0x7) != 0) return false;

		uintptr_t high_pattern = (addr >> 40) & 0xFFFFF;

		if (high_pattern == (pattern_prefix >> 40)) {
			return true;
		}

		return false;
	}

	bool isValidPointer(uintptr_t addr) {
		return (
			addr > (pml4_base + 0x1000) &&
			addr < 0x7FFFFFFFFFFF &&
			(addr & 0x7) == 0
			);
	}

	bool verifyUWorld(uintptr_t candidate) {
		try {
			uintptr_t first_value = driver.readv<uintptr_t>(candidate);
			return isValidPointer(first_value);
		}
		catch (...) {
			return false;
		}
	}


public:
	PrecisePatternUWorldFinder(uintptr_t pml4) : pml4_base(pml4) {

		pattern_prefix = pml4_base & 0xFFFFF00000000;

		initializeOffsets();

		printf("[*] PML4 Base: 0x%llX\n", pml4_base);
		printf("[*] Looking for UWorld pattern: 0x%llXXXXXXX\n", pattern_prefix >> 40);
	}

	bool isFromSameRegion(uintptr_t addr) {
		if (!isValidPointer(addr)) return false;

		return ((addr & 0xFFFFF00000000) == (pml4_base & 0xFFFFF00000000));
	}

	uintptr_t findUWorld() {
		printf("[*] Searching for UWorld from PML4 region: 0x%llX...\n", pml4_base);

		for (uintptr_t offset : priority_offsets) {
			try {
				uintptr_t value = driver.readv<uintptr_t>(pml4_base + offset);
				if (!isValidPointer(value)) continue;

				if (isFromSameRegion(value)) {
					if (verifyUWorld(value)) {
						printf("[+] Found UWorld at PML4+0x%X: 0x%llX\n",
							(unsigned int)offset, value);
						return value;
					}
				}
			}
			catch (...) {}
		}

		printf("[-] No valid UWorld found from PML4 region\n");
		return 0;
	}

	void debugScan() {
		printf("\n[DEBUG] Scanning offsets for pattern 0x%llXXXXXXX:\n", pattern_prefix >> 40);

		for (size_t i = 0; i < 10 && i < priority_offsets.size(); i++) {
			uintptr_t offset = priority_offsets[i];
			try {
				uintptr_t value = driver.readv<uintptr_t>(pml4_base + offset);
				if (value != 0) {
					uintptr_t high_pattern = (value >> 40) & 0xFFFFF;
					printf("  Offset 0x%X: 0x%llX (pattern: 0x%X)\n",
						(unsigned int)offset, value, (unsigned int)high_pattern);
				}
			}
			catch (...) {}
		}
	}
};






std::vector<std::thread> threads;





static uintptr_t cached_world = 0;
static DWORD64 last_search = 0;
bool negritoeee = true;
bool printed_once = false;


uintptr_t getfnamekey(uintptr_t base_address) {
	const auto key = driver.readv<uintptr_t>(base_address + (offsets::FnameState + 0x38));
	const auto state = driver.readv<uintptr_t>(base_address + offsets::FnameState);
	const auto ptr = decrypt_xor_keys(key, (uintptr_t*)&state);
	return driver.readv<uintptr_t>(ptr);
}
std::string SLIGHTSTY(int key)
{
	static std::uintptr_t cachedNameKey = 0;
	if (!cachedNameKey)
		cachedNameKey = getfnamekey(baseadd);

	UINT chunkOffset = key >> 16;
	USHORT nameOffset = key & 0xFFFF;

	std::uintptr_t chunkAddr = baseadd + offsets::fnamepool + ((chunkOffset + 2) * 8);
	std::uint64_t namePoolChunk = driver.readv<std::uint64_t>(chunkAddr);
	std::uintptr_t entryOffset = namePoolChunk + (4 * nameOffset);

	FNameEntry nameEntry = driver.readv<FNameEntry>(entryOffset);
	auto name = nameEntry.AnsiName;

	const BYTE* keyBytes = reinterpret_cast<BYTE*>(&cachedNameKey);
	for (std::uint16_t i = 0; i < nameEntry.Header.Len; i++)
	{
		name[i] ^= nameEntry.Header.Len ^ keyBytes[i & 3];
	}

	return std::string(name, nameEntry.Header.Len);
}


auto CacheGame() -> void
{


	if (!Settings::bHVCI)

	{
		auto pml4maw = GetPML4Base();
		PrecisePatternUWorldFinder finder(pml4maw);
		finder.debugScan();

		while (true) {
			//if (Settings::bHVCI) {
			uintptr_t GWORLD;
			DWORD64 current_time = GetTickCount64();

			if (cached_world == 0 || (current_time - last_search) > 10000) {
				uintptr_t found_world = finder.findUWorld();

				if (found_world != 0) {
					cached_world = found_world;
					last_search = current_time;
				}
				else {
					printf("[-] No UWorld found\n");
					cached_world = 0;
				}
			}

			if (cached_world != 0) {
				GWORLD = cached_world;

				static DWORD64 last_print = 0;
				if (current_time - last_print > 2000) {
					printf((const char*)"[*] World: 0x%p\n", GWORLD);
					last_print = current_time;
				}
			}
			else {
				Sleep(1000);
				continue;
			}

			uintptr_t World = GWORLD;
			std::vector<ValEntity> CachedList;

			printf("[DEBUG] World: %p\n", (void*)World);

			auto ULevelPtr = UWorld->ULevel(World);
			printf("[DEBUG] Ulevelptr: %p\n", (void*)ULevelPtr);
			auto UGameInstancePtr = UWorld->GameInstance(World);
			printf("[DEBUG] Gameinstance: %p\n", (void*)UGameInstancePtr);
			auto ULocalPlayerPtr = UGameInstance->ULocalPlayer(UGameInstancePtr);
			printf("[DEBUG]ULocalPlayerPtr: %p\n", (void*)ULocalPlayerPtr);
			auto APlayerControllerPtr = ULocalPlayer->APlayerController(ULocalPlayerPtr);
			printf("[DEBUG]APlayerControllerPtr: %p\n", (void*)APlayerControllerPtr);
			PlayerCameraManager = APlayerController->APlayerCameraManager(APlayerControllerPtr);
			printf("[DEBUG]PlayerCameraManager: %p\n", (void*)PlayerCameraManager);
			auto MyHUD = APlayerController->AHUD(APlayerControllerPtr);
			printf("[DEBUG] MyHUD : %p\n", (void*)MyHUD);
			auto APawnPtr = APlayerController->APawn(APlayerControllerPtr);
			printf("[DEBUG] APawnPtr : %p\n", (void*)APawnPtr);
			if (APawnPtr != 0)
			{
				MyUniqueID = APawn->UniqueID(APawnPtr);
				printf("[DEBUG] MyUniqueID : %p\n", (void*)MyUniqueID);
				MyRelativeLocation = APawn->RelativeLocation(APawnPtr);

			}

			if (MyHUD != 0)
			{
				auto PlayerArray = ULevel->AActorArray(ULevelPtr);
				printf("[DEBUG] count : %d\n", PlayerArray.Count);
				auto lolcountf = driver.readv<float>(ULevelPtr + 0xA8);
				auto lolcounti = driver.readv<int>(ULevelPtr + 0xA8);
				printf("[DEBUG] countf : %d\n", lolcountf);
				printf("[DEBUG] counti : %d\n", lolcounti);
				auto ViewInfo = driver.readv<FMinimalViewInfo>(PlayerCameraManager + 0x1f90);
				printf("[DEBUG] FOV : %d\n", ViewInfo.FOV);
				printf("[DEBUG] FOV2 : %d\n", ViewInfo.Rotation);
				printf("[DEBUG] loc : %d\n", ViewInfo.Location);
				for (uint32_t i = 0; i < PlayerArray.Count; ++i)
				{

					auto Pawns = PlayerArray[i];

					std::string agent_name = SLIGHTSTY(driver.readv<int>(Pawns + 0x18));
					printf("actor: %s\n", agent_name.c_str());

					if (Pawns != APawnPtr)
					{


						ValEntity Entities{ Pawns };
						CachedList.push_back(Entities);

					}

				}

				ValList.clear();
				ValList = CachedList;
				Sleep(1000);
			}
		}


	}
	else
	{
		while (true) {
		
			uintptr_t World;

			uintptr_t uworld = driver.readv<uintptr_t>(baseadd + offsets::GWorld);
			World = driver.readv<uintptr_t>(uworld);
			std::vector<ValEntity> CachedList;

			printf("[DEBUG] World: %p\n", (void*)World);

			auto ULevelPtr = UWorld->ULevel(World);
			printf("[DEBUG] Ulevelptr: %p\n", (void*)ULevelPtr);
			auto UGameInstancePtr = UWorld->GameInstance(World);
			printf("[DEBUG] Gameinstance: %p\n", (void*)UGameInstancePtr);
			auto ULocalPlayerPtr = UGameInstance->ULocalPlayer(UGameInstancePtr);
			printf("[DEBUG]ULocalPlayerPtr: %p\n", (void*)ULocalPlayerPtr);
			auto APlayerControllerPtr = ULocalPlayer->APlayerController(ULocalPlayerPtr);
			printf("[DEBUG]APlayerControllerPtr: %p\n", (void*)APlayerControllerPtr);
			PlayerCameraManager = APlayerController->APlayerCameraManager(APlayerControllerPtr);
			printf("[DEBUG]PlayerCameraManager: %p\n", (void*)PlayerCameraManager);
			auto MyHUD = APlayerController->AHUD(APlayerControllerPtr);
			printf("[DEBUG] MyHUD : %p\n", (void*)MyHUD);
			auto APawnPtr = APlayerController->APawn(APlayerControllerPtr);
			printf("[DEBUG] APawnPtr : %p\n", (void*)APawnPtr);
			if (APawnPtr != 0)
			{
				MyUniqueID = APawn->UniqueID(APawnPtr);
				printf("[DEBUG] MyUniqueID : %p\n", (void*)MyUniqueID);
				MyRelativeLocation = APawn->RelativeLocation(APawnPtr);

			}

			if (MyHUD != 0)
			{
				auto PlayerArray = ULevel->AActorArray(ULevelPtr);
				printf("[DEBUG] count : %d\n", PlayerArray.Count);
				auto lolcountf = driver.readv<float>(ULevelPtr + 0xA8);
				auto lolcounti = driver.readv<int>(ULevelPtr + 0xA8);
				printf("[DEBUG] countf : %d\n", lolcountf);
				printf("[DEBUG] counti : %d\n", lolcounti);
				auto ViewInfo = driver.readv<FMinimalViewInfo>(PlayerCameraManager + 0x1f90);
				printf("[DEBUG] FOV : %d\n", ViewInfo.FOV);
				printf("[DEBUG] FOV2 : %d\n", ViewInfo.Rotation);
				printf("[DEBUG] loc : %d\n", ViewInfo.Location);
				for (uint32_t i = 0; i < PlayerArray.Count; ++i)
				{

					auto Pawns = PlayerArray[i];

					std::string agent_name = SLIGHTSTY(driver.readv<int>(Pawns + 0x18));
					printf("actor: %s\n", agent_name.c_str());

					if (Pawns != APawnPtr)
					{


						ValEntity Entities{ Pawns };
						CachedList.push_back(Entities);

					}

				}

				ValList.clear();
				ValList = CachedList;
				Sleep(1000);
			}
		}

		}
	
}

auto CheatLoop() -> void
{
	for (ValEntity ValEntityList : ValList)
	{
		auto SkeletalMesh = APrivatePawn->USkeletalMeshComponent(ValEntityList.Actor);

		auto RelativeLocation = APawn->RelativeLocation(ValEntityList.Actor);
		auto RelativeLocationProjected = UE4::SDK::ProjectWorldToScreen(RelativeLocation);

		auto RelativePosition = RelativeLocation - CameraLocation;
		auto RelativeDistance = RelativePosition.Length() / 10000 * 2;

		auto HeadBone = UE4::SDK::GetEntityBone(SkeletalMesh, 8);
		auto HeadBoneProjected = UE4::SDK::ProjectWorldToScreen(HeadBone);


		auto RootBone = UE4::SDK::GetEntityBone(SkeletalMesh, 0);
		auto RootBoneProjected = UE4::SDK::ProjectWorldToScreen(RootBone);
		auto RootBoneProjected2 = UE4::SDK::ProjectWorldToScreen(FVector(RootBone.x, RootBone.y, RootBone.z - 15));

		auto Distance = MyRelativeLocation.Distance(RelativeLocation);
		

		float BoxHeight = abs(HeadBoneProjected.y - RootBoneProjected.y);
		float BoxWidth = BoxHeight * 0.40;

		auto ESPColor = ImColor(255, 255, 255);

		auto Health = APawn->Health(ValEntityList.Actor);
		if (Health <= 0) continue;

		if (APawn->bIsDormant(ValEntityList.Actor))
		{
			if (Settings::Visuals::bSnaplines)
				DrawTracers(RootBoneProjected, ESPColor);

			if (Settings::Visuals::bBox)
				
				Draw2DBox(RelativeLocationProjected, BoxWidth, BoxHeight, ESPColor);

			if (Settings::Visuals::bBoxOutlined)
				DrawOutlinedBox(RelativeLocationProjected, BoxWidth, BoxHeight, ESPColor);

			if (Settings::Visuals::bHealth)
				DrawHealthBar(RelativeLocationProjected, BoxWidth, BoxHeight, Health, RelativeDistance);

			if (Settings::Visuals::bDistance)
				
				DrawDistance(RootBoneProjected2, Distance);
		}
	}
}
