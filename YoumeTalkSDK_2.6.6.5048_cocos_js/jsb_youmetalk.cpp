#include "jsb_youmetalk.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "scripting/js-bindings/manual/jsb_conversions.hpp"
#include "scripting/js-bindings/manual/jsb_global.h"
#include "cocos2d.h"
#include "include/IYouMeVoiceEngine.h"
#include "include/IYouMeEventCallback.h"
#include "include/YouMeConstDefine.h"
#include "base/CCScheduler.h"
#include "platform/CCApplication.h"


using namespace cocos2d;

class CYouMeTalkJsWrapper;
CYouMeTalkJsWrapper* g_TalkSingleInstance;

se::Object* __jsb_YouMeTalk_proto = nullptr;
se::Class* __jsb_YouMeTalk_class = nullptr;

class CYouMeTalkJsWrapper : public IYouMeEventCallback,
	public IYouMePcmCallback,
	public IRestApiCallback,
	public IYouMeMemberChangeCallback,
	public IYouMeChannelMsgCallback
{
public:

	CYouMeTalkJsWrapper()
	{
	}

public:
	se::Object* _refObj;

	void onEvent(const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param) override;

	//void onPcmData(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) override;
	
	void  onPcmDataRemote(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte)override;

	void onPcmDataRecord(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte)override;

	void onPcmDataMix(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte)override;

	void onRequestRestAPI(int requestID, const YouMeErrorCode &iErrorCode, const char * strQuery, const char *  strResult) override;

	void onMemberChange(const char* channel, const char* listMemberChange, bool bUpdate) override;

	void onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* strContent) override;
};

//监听初始化完成情况
void CYouMeTalkJsWrapper::onEvent(const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param)
{
	std::string strChannel = channel;
	std::string strParam = param;
		Application::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		se::Value func;
		if (_refObj->getProperty("OnEvent", &func)) {
			se::ScriptEngine::getInstance()->clearException();
			se::AutoHandleScope hs;

			se::ValueArray args;
			args.push_back(se::Value((int)event));
			args.push_back(se::Value((int)error));
			args.push_back(se::Value(strChannel));
			args.push_back(se::Value(strParam));
			func.toObject()->call(args, _refObj);
		}
	});
}



void CYouMeTalkJsWrapper::onPcmDataRemote(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) {


}
void CYouMeTalkJsWrapper::onPcmDataRecord(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) {
	

}

void CYouMeTalkJsWrapper::onPcmDataMix(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) {



}


void CYouMeTalkJsWrapper::onRequestRestAPI(int requestID, const YouMeErrorCode &iErrorCode, const char* strQuery, const char*   strResult) {
	std::string query = strQuery;
	std::string result = strResult;
	int nerr = iErrorCode;
		Application::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		se::Value func;
		if (_refObj->getProperty("OnRequestRestApi", &func)) {
			se::ScriptEngine::getInstance()->clearException();
			se::AutoHandleScope hs;

			se::ValueArray args;
			args.push_back(se::Value(requestID));
			args.push_back(se::Value(nerr));
			args.push_back(se::Value(query));
			args.push_back(se::Value(result));
			func.toObject()->call(args, _refObj);
		}
	});
}


void CYouMeTalkJsWrapper::onMemberChange(const char* channel, const char* listMemberChange, bool bUpdate) {
	std::string strChannel = channel;
	std::string strMemberJson = listMemberChange;

	Application::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		se::Value func;
		if (_refObj->getProperty("OnMemberChange", &func)) {
			se::ScriptEngine::getInstance()->clearException();
			se::AutoHandleScope hs;

			se::ValueArray args;
			args.push_back(se::Value(strChannel));
			args.push_back(se::Value(strMemberJson));
			args.push_back(se::Value(bUpdate));
			func.toObject()->call(args, _refObj);
		}
	});

}

void CYouMeTalkJsWrapper::onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* content)
{
	std::string strChannel = channel;
	std::string strParam1 = param1;
	std::string strParam2 = param2;
	std::string strContent = content;
	cocos2d::Application::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
		se::Value func;
		if (_refObj->getProperty("OnBroadcast", &func)) {
			se::ScriptEngine::getInstance()->clearException();
			se::AutoHandleScope hs;

			se::ValueArray args;
			args.push_back(se::Value((int)bc));
			args.push_back(se::Value(strChannel));
			args.push_back(se::Value(strParam1));
			args.push_back(se::Value(strParam2));
			args.push_back(se::Value(strContent));
			func.toObject()->call(args, _refObj);
		}
	});
}

static bool js_cocos2dx_extension_YouMeTalk_leaveChannelAll(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelAll : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->leaveChannelAll();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelAll : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_leaveChannelAll)

static bool js_cocos2dx_extension_YouMeTalk_getVolume(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getVolume : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		unsigned int result = cobj->getVolume();
		ok &= uint32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getVolume : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getVolume)

static bool js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		bool arg1;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_boolean(args[1], &arg1);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus : Error processing arguments");
		int result = cobj->setListenOtherVoice(arg0, arg1);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus)

static bool js_cocos2dx_extension_YouMeTalk_requestRestApi(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_requestRestApi : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		int* arg2 = 0;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
#pragma warning NO CONVERSION TO NATIVE FOR int*
		ok = false;
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_requestRestApi : Error processing arguments");
		int result = cobj->requestRestApi(arg0, arg1, arg2);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_requestRestApi : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_requestRestApi)

static bool js_cocos2dx_extension_YouMeTalk_SetOutputToSpeaker(se::State& s) {
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_requestRestApi : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_requestRestApi : Error processing arguments");
		int result = cobj->setOutputToSpeaker(arg0);
		ok &= int32_to_seval(result, &s.rval());
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_SetOutputToSpeaker)

static bool js_cocos2dx_extension_YouMeTalk_setSpeakerMute(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setSpeakerMute : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setSpeakerMute : Error processing arguments");
		cobj->setSpeakerMute(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setSpeakerMute)

static bool js_cocos2dx_extension_YouMeTalk_setMicrophoneMute(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setMicrophoneMute : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setMicrophoneMute : Error processing arguments");
		cobj->setMicrophoneMute(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setMicrophoneMute)

static bool js_cocos2dx_extension_YouMeTalk_setOtherSpeakerMute(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setOtherSpeakerMute : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		bool arg1;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setOtherSpeakerMute : Error processing arguments");
		ok &= seval_to_boolean(args[1], &arg1);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setOtherSpeakerMute : Error processing arguments");
		cobj->setOtherSpeakerMute(arg0, arg1);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setOtherSpeakerMute)

static bool js_cocos2dx_extension_YouMeTalk_setOtherMicMute(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setOtherMicMute : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		bool arg1;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setOtherMicMute : Error processing arguments");
		ok &= seval_to_boolean(args[1], &arg1);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setOtherMicMute : Error processing arguments");
		cobj->setOtherMicMute(arg0, arg1);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setOtherMicMute)

static bool js_cocos2dx_extension_YouMeTalk_pauseChannel(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_pauseChannel : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->pauseChannel();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_pauseChannel : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_pauseChannel)

static bool js_cocos2dx_extension_YouMeTalk_pauseBackgroundMusic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_pauseBackgroundMusic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->pauseBackgroundMusic();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_pauseBackgroundMusic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_pauseBackgroundMusic)

static bool js_cocos2dx_extension_YouMeTalk_resumeBackgroundMusic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_resumeBackgroundMusic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->resumeBackgroundMusic();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_resumeBackgroundMusic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_resumeBackgroundMusic)

static bool js_cocos2dx_extension_YouMeTalk_isBackgroundMusicPlaying(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_isBackgroundMusicPlaying : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		bool result = cobj->isBackgroundMusicPlaying();
		ok &= boolean_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_isBackgroundMusicPlaying : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_isBackgroundMusicPlaying)
/*
static bool js_cocos2dx_extension_YouMeTalk_resumeMicSync(se::State& s)
{
IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_resumeMicSync : Invalid Native Object");
const auto& args = s.args();
size_t argc = args.size();
CC_UNUSED bool ok = true;
if (argc == 0) {
bool result = cobj->resumeMicSync();
ok &= boolean_to_seval(result, &s.rval());
SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_resumeMicSync : Error processing arguments");
return true;
}
SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_resumeMicSync)
*/

static bool js_cocos2dx_extension_YouMeTalk_resumeChannel(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_resumeChannel : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->resumeChannel();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_resumeChannel : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_resumeChannel)

static bool js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled : Error processing arguments");
		int result = cobj->setVadCallbackEnabled(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled)

static bool js_cocos2dx_extension_YouMeTalk_setReverbEnabled(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setReverbEnabled : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setReverbEnabled : Error processing arguments");
		int result = cobj->setReverbEnabled(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setReverbEnabled : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setReverbEnabled)

static bool js_cocos2dx_extension_YouMeTalk_getSDKVersion(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getSDKVersion : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->getSDKVersion();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getSDKVersion : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getSDKVersion)

static bool js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		int arg0 = 0;
		ok &= seval_to_int32(args[0], (int32_t*)&arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume : Error processing arguments");
		int result = cobj->setBackgroundMusicVolume(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume)

static bool js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn : Error processing arguments");
		int result = cobj->setHeadsetMonitorOn(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn)

static bool js_cocos2dx_extension_YouMeTalk_setServerRegion(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setServerRegion : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		int arg0 = 0;
		const char* arg1 = nullptr;
		bool arg2;
		ok &= seval_to_int32(args[0], (int32_t*)&arg0);
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		ok &= seval_to_boolean(args[2], &arg2);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setServerRegion : Error processing arguments");
		cobj->setServerRegion((YOUME_RTC_SERVER_REGION)arg0, arg1, arg2);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setServerRegion)

static bool js_cocos2dx_extension_YouMeTalk_speakToChannel(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_speakToChannel : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		const char* arg0 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_speakToChannel : Error processing arguments");
		int result = cobj->speakToChannel(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_speakToChannel : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_speakToChannel)

static bool js_cocos2dx_extension_YouMeTalk_getSpeakerMute(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getSpeakerMute : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		bool result = cobj->getSpeakerMute();
		ok &= boolean_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getSpeakerMute : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getSpeakerMute)

static bool js_cocos2dx_extension_YouMeTalk_setGrabMicOption(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 5) {
		const char* arg0 = nullptr;
		int arg1 = 0;
		int arg2 = 0;
		int arg3 = 0;
		unsigned int arg4 = 0;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_int32(args[1], (int32_t*)&arg1);
		ok &= seval_to_int32(args[2], (int32_t*)&arg2);
		ok &= seval_to_int32(args[3], (int32_t*)&arg3);
		ok &= seval_to_uint32(args[4], (uint32_t*)&arg4);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : Error processing arguments");
		int result = cobj->setGrabMicOption(arg0, arg1, arg2, arg3, arg4);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 5);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setGrabMicOption)

static bool js_cocos2dx_extension_YouMeTalk_setInviteMicOption(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		const char* arg0 = nullptr;
		int arg1 = 0;
		int arg2 = 0;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_int32(args[1], (int32_t*)&arg1);
		ok &= seval_to_int32(args[2], (int32_t*)&arg2);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : Error processing arguments");
		int result = cobj->setInviteMicOption(arg0, arg1, arg2);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setInviteMicOption)

static bool js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->stopBackgroundMusic();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic)

static bool js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled : Error processing arguments");
		cobj->setUseMobileNetworkEnabled(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled)

static bool js_cocos2dx_extension_YouMeTalk_init(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_init : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 4) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		int arg2 = 0;
		const char* arg3 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		ok &= seval_to_int32(args[2], (int32_t*)&arg2);
		std::string arg3_tmp; ok &= seval_to_std_string(args[3], &arg3_tmp); arg3 = arg3_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_init : Error processing arguments");
		int result = cobj->init(g_TalkSingleInstance, arg0, arg1, (YOUME_RTC_SERVER_REGION)arg2, arg3);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_init : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 4);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_init)

static bool js_cocos2dx_extension_YouMeTalk_isInited(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_isInited : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		bool result = cobj->isInited();
		ok &= boolean_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_isInited : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_isInited)

static bool js_cocos2dx_extension_YouMeTalk_stopInviteMic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_stopInviteMic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->stopInviteMic();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_stopInviteMic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_stopInviteMic)

static bool js_cocos2dx_extension_YouMeTalk_responseInviteMic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		const char* arg0 = nullptr;
		bool arg1;
		const char* arg2 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_boolean(args[1], &arg1);
		std::string arg2_tmp; ok &= seval_to_std_string(args[2], &arg2_tmp); arg2 = arg2_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : Error processing arguments");
		int result = cobj->responseInviteMic(arg0, arg1, arg2);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_responseInviteMic)

static bool js_cocos2dx_extension_YouMeTalk_setMicLevelCallback(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setMicLevelCallback : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		int arg0 = 0;
		ok &= seval_to_int32(args[0], (int32_t*)&arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setMicLevelCallback : Error processing arguments");
		int result = cobj->setMicLevelCallback(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setMicLevelCallback : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setMicLevelCallback)

static bool js_cocos2dx_extension_YouMeTalk_getChannelUserList(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getChannelUserList : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		const char* arg0 = nullptr;
		int arg1 = 0;
		bool arg2;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_int32(args[1], (int32_t*)&arg1);
		ok &= seval_to_boolean(args[2], &arg2);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getChannelUserList : Error processing arguments");
		int result = cobj->getChannelUserList(arg0, arg1, arg2);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getChannelUserList : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getChannelUserList)

static bool js_cocos2dx_extension_YouMeTalk_setVolume(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setVolume : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		unsigned int arg0 = 0;
		ok &= seval_to_uint32(args[0], (uint32_t*)&arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setVolume : Error processing arguments");
		cobj->setVolume(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setVolume)

static bool js_cocos2dx_extension_YouMeTalk_requestGrabMic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 4) {
		const char* arg0 = nullptr;
		int arg1 = 0;
		bool arg2;
		const char* arg3 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_int32(args[1], (int32_t*)&arg1);
		ok &= seval_to_boolean(args[2], &arg2);
		std::string arg3_tmp; ok &= seval_to_std_string(args[3], &arg3_tmp); arg3 = arg3_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : Error processing arguments");
		int result = cobj->requestGrabMic(arg0, arg1, arg2, arg3);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 4);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_requestGrabMic)

static bool js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		const char* arg0 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode : Error processing arguments");
		int result = cobj->leaveChannelMultiMode(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode)

static bool js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute : Error processing arguments");
		int result = cobj->setReleaseMicWhenMute(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute)

static bool js_cocos2dx_extension_YouMeTalk_setExitCommModeWhenHeadsetPlugin(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setExitCommModeWhenHeadsetPlugin : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setExitCommModeWhenHeadsetPlugin : Error processing arguments");
		int result = cobj->setExitCommModeWhenHeadsetPlugin(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setExitCommModeWhenHeadsetPlugin : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setExitCommModeWhenHeadsetPlugin)

static bool js_cocos2dx_extension_YouMeTalk_playBackgroundMusic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_playBackgroundMusic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		bool arg1;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_boolean(args[1], &arg1);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_playBackgroundMusic : Error processing arguments");
		int result = cobj->playBackgroundMusic(arg0, arg1);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_playBackgroundMusic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_playBackgroundMusic)

static bool js_cocos2dx_extension_YouMeTalk_startGrabMicAction(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : Error processing arguments");
		int result = cobj->startGrabMicAction(arg0, arg1);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_startGrabMicAction)

static bool js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		bool result = cobj->getUseMobileNetworkEnabled();
		ok &= boolean_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled)

static bool js_cocos2dx_extension_YouMeTalk_requestInviteMic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		const char* arg2 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		std::string arg2_tmp; ok &= seval_to_std_string(args[2], &arg2_tmp); arg2 = arg2_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : Error processing arguments");
		int result = cobj->requestInviteMic(arg0, arg1, arg2);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_requestInviteMic)

static bool js_cocos2dx_extension_YouMeTalk_setListenOtherVoice(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setListenOtherVoice : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		bool arg1;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		ok &= seval_to_boolean(args[1], &arg1);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setListenOtherVoice : Error processing arguments");
		int result = cobj->setListenOtherVoice(arg0, arg1);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setListenOtherVoice : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setListenOtherVoice)

static bool js_cocos2dx_extension_YouMeTalk_stopGrabMicAction(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : Error processing arguments");
		int result = cobj->stopGrabMicAction(arg0, arg1);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_stopGrabMicAction)

static bool js_cocos2dx_extension_YouMeTalk_setToken(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setToken : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		const char* arg0 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setToken : Error processing arguments");
		cobj->setToken(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setToken)

static bool js_cocos2dx_extension_YouMeTalk_setAutoSendStatus(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setAutoSendStatus : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		bool arg0;
		ok &= seval_to_boolean(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setAutoSendStatus : Error processing arguments");
		cobj->setAutoSendStatus(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setAutoSendStatus)

static bool js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc >= 3) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		int arg2 = 0;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		ok &= seval_to_int32(args[2], (int32_t*)&arg2);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : Error processing arguments");
		bool arg3 = false;
		if (argc > 3) {
			ok &= seval_to_boolean(args[3], &arg3);
			SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : Error processing arguments");
		}
		int result = cobj->joinChannelMultiMode(arg0, arg1, (YouMeUserRole_t)arg2, arg3);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode)

static bool js_cocos2dx_extension_YouMeTalk_getMicrophoneMute(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getMicrophoneMute : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		bool result = cobj->getMicrophoneMute();
		ok &= boolean_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getMicrophoneMute : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getMicrophoneMute)

static bool js_cocos2dx_extension_YouMeTalk_unInit(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_unInit : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = cobj->unInit();
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_unInit : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_unInit)

static bool js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc >= 3) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		int arg2 = 0;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		ok &= seval_to_int32(args[2], (int32_t*)&arg2);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode : Error processing arguments");
		bool arg3 = false;
		if (argc > 3) {
			ok &= seval_to_boolean(args[3], &arg3);
			SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode : Error processing arguments");
		}
		int result = cobj->joinChannelSingleMode(arg0, arg1, (YouMeUserRole_t)arg2, arg3);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode)

/*
static bool js_cocos2dx_extension_YouMeTalk_releaseMicSync(se::State& s)
{
IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_releaseMicSync : Invalid Native Object");
const auto& args = s.args();
size_t argc = args.size();
CC_UNUSED bool ok = true;
if (argc == 0) {
bool result = cobj->releaseMicSync();
ok &= boolean_to_seval(result, &s.rval());
SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_releaseMicSync : Error processing arguments");
return true;
}
SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_releaseMicSync)
*/

static bool js_cocos2dx_extension_YouMeTalk_releaseGrabMic(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		const char* arg0 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : Error processing arguments");
		int result = cobj->releaseGrabMic(arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_releaseGrabMic)

static bool js_cocos2dx_extension_YouMeTalk_setRecordingTimeMs(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setRecordingTimeMs : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		unsigned int arg0 = 0;
		ok &= seval_to_uint32(args[0], (uint32_t*)&arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setRecordingTimeMs : Error processing arguments");
		cobj->setRecordingTimeMs(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setRecordingTimeMs)

static bool js_cocos2dx_extension_YouMeTalk_setPlayingTimeMs(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setPlayingTimeMs : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		unsigned int arg0 = 0;
		ok &= seval_to_uint32(args[0], (uint32_t*)&arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setPlayingTimeMs : Error processing arguments");
		cobj->setPlayingTimeMs(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setPlayingTimeMs)

static bool js_cocos2dx_extension_YouMeTalk_getSoundtouchPitchSemiTones(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getSoundtouchPitchSemiTones : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		double result = cobj->getSoundtouchPitchSemiTones();
		ok &= double_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getSoundtouchPitchSemiTones : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getSoundtouchPitchSemiTones)

static bool js_cocos2dx_extension_YouMeTalk_setSoundtouchPitchSemiTones(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setSoundtouchPitchSemiTones : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		double arg0 = 0.0;
		ok &= seval_to_double(args[0], &arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setSoundtouchPitchSemiTones : Error processing arguments");
		cobj->setSoundtouchPitchSemiTones(arg0);
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setSoundtouchPitchSemiTones)

static bool js_cocos2dx_extension_YouMeTalk_sendMessage(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_sendMessage : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		int arg2 = 0;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		ok &= seval_to_int32(args[2], (int32_t*)&arg2);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_sendMessage : Error processing arguments");
		int result = cobj->sendMessage(arg0, arg1, &arg2);
		if (result == YOUME_SUCCESS) {
			ok &= int32_to_seval(arg2, &s.rval());
		}
		else {
			ok &= int32_to_seval(result, &s.rval());
		}

		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_sendMessage : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_sendMessage)

static bool js_cocos2dx_extension_YouMeTalk_setWhiteUserList(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setWhiteUserList : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 2) {
		const char* arg0 = nullptr;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setWhiteUserList : Error processing arguments");
		std::string lst;
		ok &= seval_to_std_string(args[1], &lst);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setWhiteUserList : Error processing arguments");
		int result = cobj->setWhiteUserList(arg0, lst.c_str());
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setWhiteUserList : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 2);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setWhiteUserList)

static bool js_cocos2dx_extension_YouMeTalk_setUserRole(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_setUserRole : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 1) {
		int arg0 = 0;
		ok &= seval_to_int32(args[0], (int32_t*)&arg0);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setUserRole : Error processing arguments");
		int result = cobj->setUserRole((YouMeUserRole_t)arg0);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_setUserRole : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_setUserRole)

static bool js_cocos2dx_extension_YouMeTalk_getUserRole(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_getUserRole : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 0) {
		int result = (int)(cobj->getUserRole());
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_getUserRole : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_getUserRole)

static bool js_cocos2dx_extension_YouMeTalk_kickOtherFromChannel(se::State& s)
{
	IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
	SE_PRECONDITION2(cobj, false, "js_cocos2dx_extension_YouMeTalk_kickOtherFromChannel : Invalid Native Object");
	const auto& args = s.args();
	size_t argc = args.size();
	CC_UNUSED bool ok = true;
	if (argc == 3) {
		const char* arg0 = nullptr;
		const char* arg1 = nullptr;
		int arg2 = 0;
		std::string arg0_tmp; ok &= seval_to_std_string(args[0], &arg0_tmp); arg0 = arg0_tmp.c_str();
		std::string arg1_tmp; ok &= seval_to_std_string(args[1], &arg1_tmp); arg1 = arg1_tmp.c_str();
		ok &= seval_to_int32(args[2], (int32_t*)&arg2);
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_kickOtherFromChannel : Error processing arguments");
		int result = cobj->kickOtherFromChannel(arg0, arg1, arg2);
		ok &= int32_to_seval(result, &s.rval());
		SE_PRECONDITION2(ok, false, "js_cocos2dx_extension_YouMeTalk_kickOtherFromChannel : Error processing arguments");
		return true;
	}
	SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
	return false;
}
SE_BIND_FUNC(js_cocos2dx_extension_YouMeTalk_kickOtherFromChannel)

static bool js_YouMeTalk_finalize(se::State& s)
{
	CCLOGINFO("jsbindings: finalizing JS object %p (YouMeTalk)", s.nativeThisObject());
	auto iter = se::NonRefNativePtrCreatedByCtorMap::find(s.nativeThisObject());
	if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
	{
		se::NonRefNativePtrCreatedByCtorMap::erase(iter);
		IYouMeVoiceEngine* cobj = (IYouMeVoiceEngine*)s.nativeThisObject();
		//delete cobj;
		if (cobj)
			cobj->destroy();
	}
	return true;
}
SE_BIND_FINALIZE_FUNC(js_YouMeTalk_finalize)

static bool js_cocos2dx_extension_YouMeTalk_constructor(se::State& s)
{
	const auto& args = s.args();

	size_t argc = args.size();

	//assert(g_SingleInstance == NULL);
	if (g_TalkSingleInstance == NULL) {
		g_TalkSingleInstance = new CYouMeTalkJsWrapper();
	}

	se::Object* obj = s.thisObject();

	// link the native object with the javascript object
	g_TalkSingleInstance->_refObj = obj;

	IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
	cobj->setRestApiCallback(g_TalkSingleInstance);
	cobj->setNotifyCallback(g_TalkSingleInstance);
	cobj->setMemberChangeCallback(g_TalkSingleInstance);
	//cobj->setPcmCallback(g_SingleInstance, true);

	if (obj) {
		obj->setPrivateData(cobj);
		se::Value func;
		if (obj->getProperty("_ctor", &func)) {
			func.toObject()->call(se::EmptyValueArray, obj);
		}
	}

	return true;
}
SE_BIND_CTOR(js_cocos2dx_extension_YouMeTalk_constructor, __jsb_YouMeTalk_class, js_YouMeTalk_finalize)

bool js_register_cocos2dx_extension_YouMeTalk(se::Object* obj)
{
	auto cls = se::Class::create("YouMeTalk", obj, nullptr, _SE(js_cocos2dx_extension_YouMeTalk_constructor));

	cls->defineFunction("talk_LeaveChannelAll", _SE(js_cocos2dx_extension_YouMeTalk_leaveChannelAll));
	cls->defineFunction("talk_GetVolume", _SE(js_cocos2dx_extension_YouMeTalk_getVolume));
	cls->defineFunction("talk_AvoidOtherVoiceStatus", _SE(js_cocos2dx_extension_YouMeTalk_avoidOtherVoiceStatus));
	cls->defineFunction("talk_RequestRestApi", _SE(js_cocos2dx_extension_YouMeTalk_requestRestApi));
	cls->defineFunction("talk_SetSpeakerMute", _SE(js_cocos2dx_extension_YouMeTalk_setSpeakerMute));
	cls->defineFunction("talk_SetMicrophoneMute", _SE(js_cocos2dx_extension_YouMeTalk_setMicrophoneMute));
	cls->defineFunction("talk_SetOtherSpeakerMute", _SE(js_cocos2dx_extension_YouMeTalk_setOtherSpeakerMute));
	cls->defineFunction("talk_SetOtherMicMute", _SE(js_cocos2dx_extension_YouMeTalk_setOtherMicMute));
	cls->defineFunction("talk_PauseChannel", _SE(js_cocos2dx_extension_YouMeTalk_pauseChannel));
	//cls->defineFunction("talk_resumeMicSync", _SE(js_cocos2dx_extension_YouMeTalk_resumeMicSync));
	cls->defineFunction("talk_ResumeChannel", _SE(js_cocos2dx_extension_YouMeTalk_resumeChannel));
	cls->defineFunction("talk_SetVadCallbackEnabled", _SE(js_cocos2dx_extension_YouMeTalk_setVadCallbackEnabled));
	cls->defineFunction("talk_SetReverbEnabled", _SE(js_cocos2dx_extension_YouMeTalk_setReverbEnabled));
	cls->defineFunction("talk_GetSDKVersion", _SE(js_cocos2dx_extension_YouMeTalk_getSDKVersion));
	cls->defineFunction("talk_SetBackgroundMusicVolume", _SE(js_cocos2dx_extension_YouMeTalk_setBackgroundMusicVolume));
	cls->defineFunction("talk_SetHeadsetMonitorOn", _SE(js_cocos2dx_extension_YouMeTalk_setHeadsetMonitorOn));
	cls->defineFunction("talk_SetServerRegion", _SE(js_cocos2dx_extension_YouMeTalk_setServerRegion));
	cls->defineFunction("talk_SpeakToChannel", _SE(js_cocos2dx_extension_YouMeTalk_speakToChannel));
	cls->defineFunction("talk_GetSpeakerMute", _SE(js_cocos2dx_extension_YouMeTalk_getSpeakerMute));
	cls->defineFunction("talk_SetGrabMicOption", _SE(js_cocos2dx_extension_YouMeTalk_setGrabMicOption));
	cls->defineFunction("talk_SetInviteMicOption", _SE(js_cocos2dx_extension_YouMeTalk_setInviteMicOption));
	cls->defineFunction("talk_PauseBackgroundMusic", _SE(js_cocos2dx_extension_YouMeTalk_pauseBackgroundMusic));
	cls->defineFunction("talk_ResumeBackgroundMusic", _SE(js_cocos2dx_extension_YouMeTalk_resumeBackgroundMusic));
	cls->defineFunction("talk_StopBackgroundMusic", _SE(js_cocos2dx_extension_YouMeTalk_stopBackgroundMusic));
	cls->defineFunction("talk_IsBackgroundMusicPlaying", _SE(js_cocos2dx_extension_YouMeTalk_isBackgroundMusicPlaying));
	cls->defineFunction("talk_SetUseMobileNetworkEnabled", _SE(js_cocos2dx_extension_YouMeTalk_setUseMobileNetworkEnabled));
	cls->defineFunction("talk_Init", _SE(js_cocos2dx_extension_YouMeTalk_init));
	cls->defineFunction("talk_IsInited", _SE(js_cocos2dx_extension_YouMeTalk_isInited));
	cls->defineFunction("talk_StopInviteMic", _SE(js_cocos2dx_extension_YouMeTalk_stopInviteMic));
	cls->defineFunction("talk_ResponseInviteMic", _SE(js_cocos2dx_extension_YouMeTalk_responseInviteMic));
	cls->defineFunction("talk_SetMicLevelCallback", _SE(js_cocos2dx_extension_YouMeTalk_setMicLevelCallback));
	cls->defineFunction("talk_GetChannelUserList", _SE(js_cocos2dx_extension_YouMeTalk_getChannelUserList));
	cls->defineFunction("talk_SetVolume", _SE(js_cocos2dx_extension_YouMeTalk_setVolume));
	cls->defineFunction("talk_RequestGrabMic", _SE(js_cocos2dx_extension_YouMeTalk_requestGrabMic));
	cls->defineFunction("talk_LeaveChannelMultiMode", _SE(js_cocos2dx_extension_YouMeTalk_leaveChannelMultiMode));
	cls->defineFunction("talk_SetReleaseMicWhenMute", _SE(js_cocos2dx_extension_YouMeTalk_setReleaseMicWhenMute));
	cls->defineFunction("talk_SetExitCommModeWhenHeadsetPlugin", _SE(js_cocos2dx_extension_YouMeTalk_setExitCommModeWhenHeadsetPlugin));
	cls->defineFunction("talk_PlayBackgroundMusic", _SE(js_cocos2dx_extension_YouMeTalk_playBackgroundMusic));
	cls->defineFunction("talk_StartGrabMicAction", _SE(js_cocos2dx_extension_YouMeTalk_startGrabMicAction));
	cls->defineFunction("talk_GetUseMobileNetworkEnabled", _SE(js_cocos2dx_extension_YouMeTalk_getUseMobileNetworkEnabled));
	cls->defineFunction("talk_RequestInviteMic", _SE(js_cocos2dx_extension_YouMeTalk_requestInviteMic));
	cls->defineFunction("talk_SetListenOtherVoice", _SE(js_cocos2dx_extension_YouMeTalk_setListenOtherVoice));
	cls->defineFunction("talk_StopGrabMicAction", _SE(js_cocos2dx_extension_YouMeTalk_stopGrabMicAction));
	cls->defineFunction("talk_SetToken", _SE(js_cocos2dx_extension_YouMeTalk_setToken));
	cls->defineFunction("talk_SetAutoSendStatus", _SE(js_cocos2dx_extension_YouMeTalk_setAutoSendStatus));
	cls->defineFunction("talk_JoinChannelMultiMode", _SE(js_cocos2dx_extension_YouMeTalk_joinChannelMultiMode));
	cls->defineFunction("talk_GetMicrophoneMute", _SE(js_cocos2dx_extension_YouMeTalk_getMicrophoneMute));
	cls->defineFunction("talk_UnInit", _SE(js_cocos2dx_extension_YouMeTalk_unInit));
	cls->defineFunction("talk_JoinChannelSingleMode", _SE(js_cocos2dx_extension_YouMeTalk_joinChannelSingleMode));
	//cls->defineFunction("talk_releaseMicSync", _SE(js_cocos2dx_extension_YouMeTalk_releaseMicSync));
	cls->defineFunction("talk_ReleaseGrabMic", _SE(js_cocos2dx_extension_YouMeTalk_releaseGrabMic));
	cls->defineFunction("talk_SetRecordingTimeMs", _SE(js_cocos2dx_extension_YouMeTalk_setRecordingTimeMs));
	cls->defineFunction("talk_SetPlayingTimeMs", _SE(js_cocos2dx_extension_YouMeTalk_setPlayingTimeMs));
	cls->defineFunction("talk_GetSoundtouchPitchSemiTones", _SE(js_cocos2dx_extension_YouMeTalk_getSoundtouchPitchSemiTones));
	cls->defineFunction("talk_SetSoundtouchPitchSemiTones", _SE(js_cocos2dx_extension_YouMeTalk_setSoundtouchPitchSemiTones));
	cls->defineFunction("talk_SendMessage", _SE(js_cocos2dx_extension_YouMeTalk_sendMessage));
	cls->defineFunction("talk_SetWhiteUserList", _SE(js_cocos2dx_extension_YouMeTalk_setWhiteUserList));
	cls->defineFunction("talk_SetUserRole", _SE(js_cocos2dx_extension_YouMeTalk_setUserRole));
	cls->defineFunction("talk_GetUserRole", _SE(js_cocos2dx_extension_YouMeTalk_getUserRole));
	cls->defineFunction("talk_KickOtherFromChannel", _SE(js_cocos2dx_extension_YouMeTalk_kickOtherFromChannel));

	cls->defineFinalizeFunction(_SE(js_YouMeTalk_finalize));
	cls->install();
	//JSBClassType::registerClass<IYouMeVoiceEngine>(cls);

	__jsb_YouMeTalk_proto = cls->getProto();
	__jsb_YouMeTalk_class = cls;

	se::ScriptEngine::getInstance()->clearException();
	return true;
}

bool register_jsb_youmetalk(se::Object* obj)
{
	/*
	// Get the ns
	se::Value nsVal;
	if (!obj->getProperty("ym", &nsVal))
	{
	se::HandleObject jsobj(se::Object::createPlainObject());
	nsVal.setObject(jsobj);
	obj->setProperty("ym", nsVal);
	}
	se::Object* ns = nsVal.toObject();

	js_register_cocos2dx_extension_YouMeTalk(ns);
	return true;
	*/

	return js_register_cocos2dx_extension_YouMeTalk(obj);
}

#endif //#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
