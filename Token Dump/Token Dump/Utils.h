#pragma once
#include <Windows.h>
#include <sddl.h>
#include <strsafe.h>

/// <summary>
/// Get the serialized name of the type of SID from SID_NAME_USE value
/// </summary>
/// <param name="nUse">SID type value obtained from LookupAccountSidA()</param>
/// <returns>Serialized SID type</returns>
LPCWSTR GetSidType(SID_NAME_USE nUse) {
	switch (nUse) {
	case SidTypeAlias:
		return L"Alias";
	case SidTypeComputer:
		return L"Computer";
	case SidTypeDeletedAccount:
		return L"Deleted Account";
	case SidTypeDomain:
		return L"Domain";
	case SidTypeGroup:
		return L"Group";
	case SidTypeInvalid:
		return L"Invalid";
	case SidTypeLabel:
		return L"Label";
	case SidTypeLogonSession:
		return L"Logon Session";
	case SidTypeUnknown:
		return L"Unknown";
	case SidTypeUser:
		return L"User";
	case SidTypeWellKnownGroup:
		return L"Well Known Group";
	default:
		return L"N/A";
	}
}

/// <summary>
/// Get the token max length. This is required for actually getting the token information
/// </summary>
/// <param name="hTok">Handle to the process token from OpenProcessToken</param>
/// <param name="tokClass">Class name from TOKEN_INFORMATION_CLASS whose information length you want to find out</param>
/// <returns></returns>
DWORD GetTokenInfoLength(HANDLE hTok, TOKEN_INFORMATION_CLASS tokClass) {
	DWORD dwRetLength = 0x0;
	
	GetTokenInformation(hTok, tokClass, NULL, 0x0, &dwRetLength);
	
	return dwRetLength;
}


/// <summary>
/// Serialize the PSID to string and return the reference to the pointer
/// </summary>
/// <param name="pSid">Pointer to SID</param>
/// <returns>Reference to the name</returns>
LPWSTR SIDSerialize(PSID pSid) {
	LPWSTR lpSid = nullptr;
	ConvertSidToStringSidW(pSid, &lpSid);
	return lpSid;
}

/// <summary>
/// Get the account name and domain name from the SID
/// </summary>
/// <param name="pSid">Pointer to SID</param>
/// <param name="lpAccountName">Preallocated pointer to receive the account name</param>
/// <param name="lpDomainName">Preallocated pointer to recieve the domian name</param>
/// <param name="pSidType">Pointer to receieve the Sid Type</param>
/// <returns></returns>
BOOL GetDomainAccount(PSID pSid, LPWSTR lpAccountName, LPWSTR lpDomainName, PSID_NAME_USE eSidType) {
	DWORD dwAccount, dwDomain;
	dwAccount = dwDomain = MAX_PATH;

	if (!LookupAccountSidW(nullptr, pSid, lpAccountName, &dwAccount, lpDomainName, &dwDomain, eSidType)) {
		VirtualFree(lpAccountName, 0x0, MEM_RELEASE);
		VirtualFree(lpDomainName, 0x0, MEM_RELEASE);
		lpAccountName = lpDomainName = nullptr;
		return FALSE;
	}

	return TRUE;
}