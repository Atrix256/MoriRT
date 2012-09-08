/*======================================================

WebService.h

A singleton to help interface with the web service

======================================================*/

#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include "PlatformBase.h"
#include <string>
#include "Callback.h"
#include "RayTracer.h"


//the rate in seconds that we should ping the match
#define WEBSERVICE_MATCHPINGTIME  10.0f

#define DECLAREWEBSERVICECALL_NOARGS(CallName) \
	public: \
		void CallName(WebServiceCallback pCallback, void *pUserData, const char *szLoadingText = NULL); \
		void Cancel##CallName() {CRayTracer::GetSingleton().GetPlatform().CancelHTTPRequestsOfType(CPlatformBase::k##CallName);} \
		static void StaticCB_Cancel##CallName(void *) {WebService::GetSingleton().Cancel##CallName();} \
	private: \
		static void StaticCB_On##CallName##HTTPDone(void * arg1,ERequestFinishCode arg2,const char * arg3) \
		{ \
			WebService::GetSingleton().On##CallName##HTTPDone(arg1,arg2,arg3); \
		} \
		void On##CallName##HTTPDone(void *,ERequestFinishCode,const char *);

#define DECLAREWEBSERVICECALL_TWOARGS(CallName,Arg1,Arg2) \
	public: \
		void CallName(Arg1,Arg2,WebServiceCallback pCallback, void *pUserData, const char *szLoadingText = NULL); \
		void Cancel##CallName() {CRayTracer::GetSingleton().GetPlatform().CancelHTTPRequestsOfType(CPlatformBase::k##CallName);} \
		static void StaticCB_Cancel##CallName(void *) {WebService::GetSingleton().Cancel##CallName();} \
	private: \
		static void StaticCB_On##CallName##HTTPDone(void * arg1,ERequestFinishCode arg2,const char * arg3) \
		{ \
			WebService::GetSingleton().On##CallName##HTTPDone(arg1,arg2,arg3); \
		} \
		void On##CallName##HTTPDone(void *,ERequestFinishCode,const char *);

#define DECLAREWEBSERVICECALL_THREEARGS(CallName,Arg1,Arg2,Arg3) \
	public: \
		void CallName(Arg1,Arg2,Arg3,WebServiceCallback pCallback, void *pUserData, const char *szLoadingText = NULL); \
		void Cancel##CallName() {CRayTracer::GetSingleton().GetPlatform().CancelHTTPRequestsOfType(CPlatformBase::k##CallName);} \
		static void StaticCB_Cancel##CallName(void *) {WebService::GetSingleton().Cancel##CallName();} \
	private: \
		static void StaticCB_On##CallName##HTTPDone(void * arg1,ERequestFinishCode arg2,const char * arg3) \
		{ \
			WebService::GetSingleton().On##CallName##HTTPDone(arg1,arg2,arg3); \
		} \
		void On##CallName##HTTPDone(void *,ERequestFinishCode,const char *);

#define DECLAREWEBSERVICECALL_FOURARGS(CallName,Arg1,Arg2,Arg3,Arg4) \
	public: \
		void CallName(Arg1,Arg2,Arg3,Arg4,WebServiceCallback pCallback, void *pUserData, const char *szLoadingText = NULL); \
		void Cancel##CallName() {CRayTracer::GetSingleton().GetPlatform().CancelHTTPRequestsOfType(CPlatformBase::k##CallName);} \
		static void StaticCB_Cancel##CallName(void *) {WebService::GetSingleton().Cancel##CallName();} \
	private: \
		static void StaticCB_On##CallName##HTTPDone(void * arg1,ERequestFinishCode arg2,const char * arg3) \
		{ \
			WebService::GetSingleton().On##CallName##HTTPDone(arg1,arg2,arg3); \
		} \
		void On##CallName##HTTPDone(void *,ERequestFinishCode,const char *);

class CUISheet_Loading;
typedef void(*ButtonClickCallback)(void *);

class WebService
{
public:

	static WebService &GetSingleton();

	void Logout();
	const char *GetUserName() {return m_username.c_str();}
	void SetGameName(const char *szGameName) {m_gamename = szGameName;}

	void Update(float fElapsedSeconds);

	//web service response stuff
	enum EWebServiceResponse
	{
		kError,
		kCancel,
		kOK,
		kInvalidLogin,
		kAlreadyExists,
		kPasswordMismatch
	};
	typedef void(*WebServiceCallback)(void *pUserData, EWebServiceResponse eResponse);
	struct SCallbackData
	{
		SCallbackData(WebServiceCallback pCallback, void *pUserData, const char *szLoadingText,ButtonClickCallback pCancelCallback, void *pCancelCallbackUserData);

		WebServiceCallback m_pCallback;
		void *m_pUserData;
		CUISheet_Loading *m_pLoadingDialog;
	};

	//Web service calls
	DECLAREWEBSERVICECALL_TWOARGS(Login, const char *szUsername, const char *szPassword)
	DECLAREWEBSERVICECALL_FOURARGS(CreateAccount, const char *szUsername, const char *szPassword1, const char *szPassword2, const char *szEmail)
	DECLAREWEBSERVICECALL_THREEARGS(CreateMatch, const char *szMatchName, int nNumPlayers, bool bPrivateMatch)
	DECLAREWEBSERVICECALL_NOARGS(MatchPing)

	bool IsInMatch() {return m_matchid.c_str()[0] != 0;}
	bool IsLoggedIn() {return m_userid.c_str()[0] != 0;}

private:

	bool EnterMatch(const char *szMatchId);
	void LeaveMatch();

	static EWebServiceResponse MapHttpErrorToWebServiceError(ERequestFinishCode eCode);
	void HandleCallback(void *pData, EWebServiceResponse eResponse);

	WebService();

	std::string m_gamename;  //the name of the game (such as "dots"), for use in various web services
	std::string m_username;  //displayable username
	std::string m_userid;    //"object id" of user in mongo
	std::string m_matchid;   //"object id" of current match in mongo

	bool m_bMatchPingInFlight;    //whether or not a match ping is in flight
	float m_fTimeSinceMatchPing;  //how long in seconds since we did a match ping
};

#endif // WEBSERVICE_H