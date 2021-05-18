//
//  jsb_youmetalk.cpp
//  TalkJS
//
//  Created by kilo on 17/3/3
//
//

#include "jsb_youmetalk.h"

#include "cocos2d.h"
#include "spidermonkey_specifics.h"
#include "ScriptingCore.h"
#include "cocos2d_specifics.hpp"

#include <IYouMeVoiceEngine.h>
//#include <audio/include/AudioEngine.h>

#ifndef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
#define JS_ReportError JS_ReportErrorUTF8
#define JSVAL_NULL JS::NullValue()
// From native type to jsval
JS::Value int32_to_jsval( JSContext *cx, int32_t number )
{
    return JS::Int32Value(number);
}
JS::Value uint32_to_jsval( JSContext *cx, uint32_t number ){
    return JS::Int32Value(number);
}
#endif

using namespace cocos2d;


class CYouMeTalkJsWrapper;
CYouMeTalkJsWrapper* g_SingleInstance;

JSClass  *js_cocos2dx_youmetalk_class;
JSObject *js_cocos2dx_youmetalk_prototype;



class CYouMeTalkJsWrapper: public IYouMeEventCallback,
public IYouMePcmCallback,
public IRestApiCallback,
public IYouMeMemberChangeCallback,
public IYouMeChannelMsgCallback
{
public:
    
    CYouMeTalkJsWrapper()
    {
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        _JSDelegate.construct(cx);
#endif
    }
    
public:
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
    mozilla::Maybe<JS::PersistentRootedObject> _JSDelegate;
#else
    JS::PersistentRootedObject *_JSDelegate;
#endif
    
    
    void onEvent( const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param ) override;
    
    //远端pcm数据回调
    void onPcmDataRemote(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) override;
    //本地录音pcm数据回调
    void onPcmDataRecord(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) override;
    //远端pcm和本地录音pcm的混合pcm数据回调
    void onPcmDataMix(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte) override;

    void onRequestRestAPI( int requestID, const YouMeErrorCode &iErrorCode, const char * strQuery, const char *  strResult ) override;
    
    void onMemberChange( const char* channel, const char* listMemberChange , bool bUpdate ) override ;
    
    void onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* strContent) override;
};

//监听初始化完成情况
void CYouMeTalkJsWrapper::onEvent (const YouMeEvent event, const YouMeErrorCode error, const char * channel, const char * param)
{
    std::string strChannel = channel;
    std::string strParam = param;
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        jsval params[4];
        params[0] = int32_to_jsval(cx, event);
        params[1] = int32_to_jsval(cx, error);
        params[2] = std_string_to_jsval( cx, strChannel );
        params[3] = std_string_to_jsval( cx, strParam );
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnEvent", 4, params);
#else
        JSContext *cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::AutoValueVector valArr(cx);
        JS::RootedValue argv(cx);
        
        valArr.append(JS::Int32Value(event));
        valArr.append(JS::Int32Value(error));
        std_string_to_jsval(cx, strChannel, &argv);
        valArr.append(argv);
        std_string_to_jsval(cx, strParam, &argv);
        valArr.append(argv);
        
        JS::HandleValueArray args(valArr);
        JS::RootedValue delegate(cx, JS::ObjectOrNullValue(g_SingleInstance->_JSDelegate->get()));
        ScriptingCore::getInstance()->executeFunctionWithOwner(delegate, "OnEvent", args);
#endif
        
    });
}

//远端pcm数据回调
void CYouMeTalkJsWrapper::onPcmDataRemote(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte){
    //todo:暂时不暴露
}

//本地录音pcm数据回调
void CYouMeTalkJsWrapper::onPcmDataRecord(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte){
    //todo:暂时不暴露
}

//远端pcm和本地录音pcm的混合pcm数据回调
void CYouMeTalkJsWrapper::onPcmDataMix(int channelNum, int samplingRateHz, int bytesPerSample, void* data, int dataSizeInByte){
    //todo:暂时不暴露
}

void CYouMeTalkJsWrapper::onRequestRestAPI( int requestID, const YouMeErrorCode &iErrorCode, const char* strQuery, const char*   strResult ){
    std::string query = strQuery;
    std::string result = strResult;
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        jsval params[4];
        params[0] = int32_to_jsval(cx, requestID);
        params[1] = int32_to_jsval(cx, iErrorCode);
        params[2] = std_string_to_jsval( cx, query );
        params[3] = std_string_to_jsval( cx, result );
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnRequestRestApi", 4, params);
#else
        JSContext *cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::AutoValueVector valArr(cx);
        JS::RootedValue argv(cx);
        
        valArr.append(JS::Int32Value(requestID));
        valArr.append(JS::Int32Value(iErrorCode));
        std_string_to_jsval(cx, query, &argv);
        valArr.append(argv);
        std_string_to_jsval(cx, result, &argv);
        valArr.append(argv);
        
        JS::HandleValueArray args(valArr);
        JS::RootedValue delegate(cx, JS::ObjectOrNullValue(g_SingleInstance->_JSDelegate->get()));
        ScriptingCore::getInstance()->executeFunctionWithOwner(delegate, "OnRequestRestApi", args);
#endif
    });
}


void CYouMeTalkJsWrapper::onMemberChange( const char* channel, const char* listMemberChange , bool bUpdate ){
    std::string strChannel = channel;
    std::string strMemberJson = listMemberChange;
    
    Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::RootedObject jsretArr(cx, JS_NewArrayObject(cx, 0));
    
        jsval params[3];
        
        params[0] = std_string_to_jsval( cx, strChannel );
        params[1] = std_string_to_jsval( cx, strMemberJson );
        params[2] = BOOLEAN_TO_JSVAL( bUpdate );
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(g_SingleInstance->_JSDelegate.ref()), "OnMemberChange", 3, params);
#else
        JSContext *cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::AutoValueVector valArr(cx);
        JS::RootedValue argv(cx);
        
        JS::RootedObject jsretArr(cx, JS_NewArrayObject(cx, 0));
        
        std_string_to_jsval(cx, strChannel, &argv);
        valArr.append(argv);
        std_string_to_jsval(cx, listMemberChange, &argv);
        valArr.append(argv);
        valArr.append(JS::Int32Value(bUpdate));
        
        JS::HandleValueArray args(valArr);
        JS::RootedValue delegate(cx, JS::ObjectOrNullValue(g_SingleInstance->_JSDelegate->get()));
        ScriptingCore::getInstance()->executeFunctionWithOwner(delegate, "OnMemberChange", args);
#endif
    });
    
}

void CYouMeTalkJsWrapper::onBroadcast(const YouMeBroadcast bc, const char* channel, const char* param1, const char* param2, const char* content)
{
    std::string strChannel = channel;
    std::string strParam1 = param1;
    std::string strParam2 = param2;
    std::string strContent = content;
    cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread([=] {
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
        jsval params[5];
        params[0] = int32_to_jsval(cx, bc);
        params[1] = std_string_to_jsval(cx, strChannel);
        params[2] = std_string_to_jsval(cx, strParam1);
        params[3] = std_string_to_jsval(cx, strParam2);
        params[4] = std_string_to_jsval(cx, strContent);
        
        ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(this->_JSDelegate.ref()), "OnBroadcast", 5, params);
#else
        JSContext *cx = ScriptingCore::getInstance()->getGlobalContext();
        JS::AutoValueVector valArr(cx);
        JS::RootedValue argv(cx);
        
        std_string_to_jsval(cx, strChannel, &argv);
        valArr.append(argv);
        std_string_to_jsval(cx, strParam1, &argv);
        valArr.append(argv);
        std_string_to_jsval(cx, strParam2, &argv);
        valArr.append(argv);
        std_string_to_jsval(cx, strContent, &argv);
        valArr.append(argv);
        
        JS::HandleValueArray args(valArr);
        JS::RootedValue delegate(cx, JS::ObjectOrNullValue(g_SingleInstance->_JSDelegate->get()));
        ScriptingCore::getInstance()->executeFunctionWithOwner(delegate, "OnBroadcast", args);
#endif
        
    });
}

static void js_cocos2dx_YouMetalk_finalize(JSFreeOp *fop, JSObject *obj) {
    CCLOG("jsbindings: finalizing JS object %p (YouMeTalk)", obj);
}


bool js_cocos2dx_extension_YouMetalk_constructor(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    if (argc == 0)
    {
        //        assert(g_SingleInstance == NULL);
        //        if (g_SingleInstance != NULL)
        //        {
        //            delete g_SingleInstance;
        //        }
        
        if (g_SingleInstance == NULL)
        {
            g_SingleInstance = new CYouMeTalkJsWrapper();
        }
        
        // link the native object with the javascript object
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
        JS::RootedObject obj(cx, JS_NewObject(cx, js_cocos2dx_youmetalk_class, JS::RootedObject(cx, js_cocos2dx_youmetalk_prototype), JS::NullPtr()));
        g_SingleInstance->_JSDelegate.ref() = obj;
        js_proxy_t *p = jsb_new_proxy(g_SingleInstance, obj);
        JS::AddNamedObjectRoot(cx, &p->obj, "YouMeTalk");
        args.rval().set(OBJECT_TO_JSVAL(obj));
#else
        JS::RootedObject proto(cx, js_cocos2dx_youmetalk_prototype);
        JS::RootedObject obj(cx, JS_NewObjectWithGivenProto(cx, js_cocos2dx_youmetalk_class, proto));
        js_add_FinalizeHook(cx, obj, false);
        if (g_SingleInstance->_JSDelegate) {
            CC_SAFE_DELETE(g_SingleInstance->_JSDelegate);
        }
        g_SingleInstance->_JSDelegate = new (std::nothrow) JS::PersistentRootedObject(cx, obj);
        jsb_new_proxy(cx, g_SingleInstance, obj);
        JS_SetPrivate(obj.get(), g_SingleInstance);
        args.rval().set(JS::ObjectOrNullValue(obj));
#endif
        
        return true;
    }
    
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

//引擎初始化，只需要调用一次
bool js_cocos2dx_extension_talk_Init(JSContext *cx, uint32_t argc, JS::Value *vp) {
    if (argc == 4)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        //获取两个参数，appkey，appsecrect
        std::string strAppKey;
        jsval_to_std_string(cx, args.get(0), &strAppKey);
        std::string strAppSecret;
        jsval_to_std_string(cx, args.get(1), &strAppSecret);
        
        int serverRegionID = 0;
        jsval_to_int32(cx, args.get(2), &serverRegionID);
        std::string strExtServerRegionName;
        jsval_to_std_string(cx, args.get(3), &strExtServerRegionName);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->init(g_SingleInstance,
                                                                strAppKey.c_str(),
                                                                strAppSecret.c_str(),
                                                                (YOUME_RTC_SERVER_REGION)serverRegionID,
                                                                strExtServerRegionName.c_str() );
        
//        IYouMeVoiceEngine::getInstance()->setPcmCallback( g_SingleInstance );
        IYouMeVoiceEngine::getInstance()->setRestApiCallback( g_SingleInstance );
        IYouMeVoiceEngine::getInstance()->setMemberChangeCallback( g_SingleInstance );
        IYouMeVoiceEngine::getInstance()->setNotifyCallback(g_SingleInstance);
        
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 4);
    return false;
}

bool js_cocos2dx_extension_talk_UnInit(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->unInit();
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetToken(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strToken;
        jsval_to_std_string(cx, args.get(0), &strToken);
        
        IYouMeVoiceEngine::getInstance()->setToken( strToken.c_str() );
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetServerRegion(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 3)
    {
        //todo
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int serverRegionID = 0;
        jsval_to_int32(cx, args.get(0), &serverRegionID);
        std::string strExtServerRegionName;
        jsval_to_std_string(cx, args.get(1), &strExtServerRegionName);
        int bAppend = 0;
        jsval_to_int32(cx, args.get(2), &bAppend);
        
        
        
        IYouMeVoiceEngine::getInstance()->setServerRegion((YOUME_RTC_SERVER_REGION)serverRegionID,
                                                          strExtServerRegionName.c_str(), (bool)bAppend);
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}

bool js_cocos2dx_extension_talk_SetOutputToSpeaker(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int bOutputToSpeaker = 0;
        jsval_to_int32(cx, args.get(0), &bOutputToSpeaker);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setOutputToSpeaker((bool)bOutputToSpeaker);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetSpeakerMute(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int bMute = 0;
        jsval_to_int32(cx, args.get(0), &bMute);
        
        IYouMeVoiceEngine::getInstance()->setSpeakerMute((bool)bMute);
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

//返回值将为int类型
bool js_cocos2dx_extension_talk_GetSpeakerMute(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int result = IYouMeVoiceEngine::getInstance()->getSpeakerMute();
        args.rval().set(int32_to_jsval(cx, result));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetMicrophoneMute(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int bMute = 0;
        jsval_to_int32(cx, args.get(0), &bMute);
        
        IYouMeVoiceEngine::getInstance()->setMicrophoneMute((bool)bMute);
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_GetMicrophoneMute(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int result = IYouMeVoiceEngine::getInstance()->getMicrophoneMute();
        args.rval().set(int32_to_jsval(cx, result));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetAutoSendStatus(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int bAutoSend = 0;
        jsval_to_int32(cx, args.get(0), &bAutoSend);
        
        IYouMeVoiceEngine::getInstance()->setAutoSendStatus((bool)bAutoSend);
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetVolume(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        unsigned int  vol = 0;
        jsval_to_uint32(cx, args.get(0), &vol);
        
        IYouMeVoiceEngine::getInstance()->setVolume(vol);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_GetVolume(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        unsigned int  result = IYouMeVoiceEngine::getInstance()->getVolume();
        args.rval().set(uint32_to_jsval(cx, result));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetUseMobileNetworkEnabled(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int bOn = 0;
        jsval_to_int32(cx, args.get(0), &bOn);
        
        IYouMeVoiceEngine::getInstance()->setUseMobileNetworkEnabled((bool)bOn);
        
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_GetUseMobileNetworkEnabled(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int result = IYouMeVoiceEngine::getInstance()->getUseMobileNetworkEnabled();
        args.rval().set(int32_to_jsval(cx, result));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_JoinChannelSingleMode(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 3)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUserID;
        jsval_to_std_string(cx, args.get(0), &strUserID);
        std::string strRoomID;
        jsval_to_std_string(cx, args.get(1), &strRoomID);
        int iRoleType  = 0;
        jsval_to_int32(cx, args.get(2), &iRoleType);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->joinChannelSingleMode(strUserID.c_str(),
                                                                                 strRoomID.c_str(),
                                                                                 (YouMeUserRole_t)iRoleType);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }

    if (argc == 4)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUserID;
        jsval_to_std_string(cx, args.get(0), &strUserID);
        std::string strRoomID;
        jsval_to_std_string(cx, args.get(1), &strRoomID);
        int iRoleType  = 0;
        jsval_to_int32(cx, args.get(2), &iRoleType);
        int bCheckRoomExist = 0;
        jsval_to_int32(cx, args.get(3), &bCheckRoomExist);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->joinChannelSingleMode(strUserID.c_str(),
                                                                                 strRoomID.c_str(),
                                                                                 (YouMeUserRole_t)iRoleType,
                                                                                 (bool)bCheckRoomExist);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 4);
    return false;
}

bool js_cocos2dx_extension_talk_JoinChannelMultiMode(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 3)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUserID;
        jsval_to_std_string(cx, args.get(0), &strUserID);
        std::string strRoomID;
        jsval_to_std_string(cx, args.get(1), &strRoomID);
        int iRoleType  = 0;
        jsval_to_int32(cx, args.get(2), &iRoleType);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->joinChannelMultiMode( strUserID.c_str(),
                                                                                 strRoomID.c_str(),
                                                                                (YouMeUserRole_t)iRoleType);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }

    if (argc == 4)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUserID;
        jsval_to_std_string(cx, args.get(0), &strUserID);
        std::string strRoomID;
        jsval_to_std_string(cx, args.get(1), &strRoomID);
        int iRoleType  = 0;
        jsval_to_int32(cx, args.get(2), &iRoleType);
        int bCheckRoomExist = 0;
        jsval_to_int32(cx, args.get(3), &bCheckRoomExist);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->joinChannelMultiMode( strUserID.c_str(),
                                                                                 strRoomID.c_str(),
                                                                                (YouMeUserRole_t)iRoleType,
                                                                                (bool)bCheckRoomExist );
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 4);
    return false;
}

bool js_cocos2dx_extension_talk_SpeakToChannel(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strChannel;
        jsval_to_std_string(cx, args.get(0), &strChannel);
        
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->speakToChannel( strChannel.c_str() );
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetWhiteUserList(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strChannel;
        jsval_to_std_string(cx, args.get(0), &strChannel);
        
        std::string strWhiteUserList;
        jsval_to_std_string(cx, args.get(1), &strWhiteUserList);
        
        // std::vector<std::string> vecWhiteUserList;
        // std::string::size_type pos1, pos2;
        // pos2 = strWhiteUserList.find_first_of('|');
        // pos1 = 0;
        // while (std::string::npos != pos2)
        // {
        //     if(pos2-pos1) {
        //         vecWhiteUserList.push_back(strWhiteUserList.substr(pos1, pos2 - pos1));
        //     }
        //     pos1 = pos2 + 1;
        //     pos2 = strWhiteUserList.find_first_of('|', pos1);
        // }
        // if (pos1 != strWhiteUserList.length())
        //     vecWhiteUserList.push_back(strWhiteUserList.substr(pos1));
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setWhiteUserList( strChannel.c_str(), strWhiteUserList.c_str());
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

bool js_cocos2dx_extension_talk_SetUserRole(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int iRoleType  = 0;
        jsval_to_int32(cx, args.get(0), &iRoleType);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setUserRole((YouMeUserRole_t)iRoleType);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_GetUserRole(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int result = (int)(IYouMeVoiceEngine::getInstance()->getUserRole());
        args.rval().set(int32_to_jsval(cx, result));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_LeaveChannelMultiMode(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strChannel;
        jsval_to_std_string(cx, args.get(0), &strChannel);
        
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->leaveChannelMultiMode( strChannel.c_str() );
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_LeaveChannelAll(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->leaveChannelAll();
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_GetChannelUserList(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 3)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strChannel;
        jsval_to_std_string(cx, args.get(0), &strChannel);
        
        int maxCount  = 0;
        jsval_to_int32(cx, args.get(1), &maxCount);
        
        
        int notifyMemChange  = 0;
        jsval_to_int32(cx, args.get(2), &notifyMemChange);
        
        
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->getChannelUserList( strChannel.c_str(), maxCount, notifyMemChange );
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetOtherMicMute(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUser;
        jsval_to_std_string(cx, args.get(0), &strUser);
        int bMute = 0;
        jsval_to_int32(cx, args.get(1), &bMute);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setOtherMicMute( strUser.c_str(), (bool)bMute);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

bool js_cocos2dx_extension_talk_SetOtherSpeakerMute(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUser;
        jsval_to_std_string(cx, args.get(0), &strUser);
        int bMute = 0;
        jsval_to_int32(cx, args.get(1), &bMute);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setOtherSpeakerMute( strUser.c_str(), (bool)bMute);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}



bool js_cocos2dx_extension_talk_SetListenOtherVoice(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUser;
        jsval_to_std_string(cx, args.get(0), &strUser);
        int bOn = 0;
        jsval_to_int32(cx, args.get(1), &bOn);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setListenOtherVoice( strUser.c_str(), (bool)bOn);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}


bool js_cocos2dx_extension_talk_PlayBackgroundMusic(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string strFilePath;
        jsval_to_std_string(cx, args.get(0), &strFilePath);
        int repeat = 0;
        jsval_to_int32(cx, args.get(1), &repeat);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->playBackgroundMusic(strFilePath.c_str() ,(bool)repeat);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}


bool js_cocos2dx_extension_talk_StopBackgroundMusic(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->stopBackgroundMusic();
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetBackgroundMusicVolume(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int vol = 0;
        jsval_to_int32(cx, args.get(0), &vol);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setBackgroundMusicVolume(vol);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_GetBackgroundMusicVolume(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int result = IYouMeVoiceEngine::getInstance()->getBackgroundMusicVolume();
        args.rval().set(int32_to_jsval(cx, result));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetHeadsetMonitorOn(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int bMicEnabled = 0;
        jsval_to_int32(cx, args.get(0), &bMicEnabled);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setHeadsetMonitorOn((bool)bMicEnabled);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int bMicEnabled = 0;
        jsval_to_int32(cx, args.get(0), &bMicEnabled);

        int bBgmEnabled = 0;
        jsval_to_int32(cx, args.get(1), &bBgmEnabled);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setHeadsetMonitorOn((bool)bMicEnabled, (bool)bBgmEnabled);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

bool js_cocos2dx_extension_talk_setReverbEnabled(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int bEnabled = 0;
        jsval_to_int32(cx, args.get(0), &bEnabled);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setReverbEnabled((bool)bEnabled);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetVadCallbackEnabled(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int bEnabled = 0;
        jsval_to_int32(cx, args.get(0), &bEnabled);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setVadCallbackEnabled((bool)bEnabled);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetMicLevelCallback(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int maxLevel = 0;
        jsval_to_int32(cx, args.get(0), &maxLevel);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setMicLevelCallback( maxLevel );
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_PauseChannel(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->pauseChannel();
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_ResumeChannel(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->resumeChannel();
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_GetSoundtouchPitchSemiTones(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int pitchSemiTones = (int) (IYouMeVoiceEngine::getInstance()->getSoundtouchPitchSemiTones());
        args.rval().set(int32_to_jsval(cx, pitchSemiTones));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_SetSoundtouchPitchSemiTones(JSContext *cx, uint32_t argc, JS::Value *vp) {

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int pitchSemiTones = 0;
        jsval_to_int32(cx, args.get(0), &pitchSemiTones);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setSoundtouchPitchSemiTones( (float)pitchSemiTones) ;
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetRecordingTimeMs(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        unsigned int tiemMS = 0;
        jsval_to_uint32(cx, args.get(0), &tiemMS);
        
        IYouMeVoiceEngine::getInstance()->setRecordingTimeMs( tiemMS );
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetPlayingTimeMs(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        unsigned int tiemMS = 0;
        jsval_to_uint32(cx, args.get(0), &tiemMS);
        
        IYouMeVoiceEngine::getInstance()->setPlayingTimeMs( tiemMS );
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}


bool js_cocos2dx_extension_talk_SetReleaseMicWhenMute(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int enable = 0;
        jsval_to_int32(cx, args.get(0), &enable);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setReleaseMicWhenMute((bool)enable);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_SetExitCommModeWhenHeadsetPlugin(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        int enable = 0;
        jsval_to_int32(cx, args.get(0), &enable);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->setExitCommModeWhenHeadsetPlugin((bool)enable);
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_RequestRestApi(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strCommand;
        jsval_to_std_string(cx, args.get(0), &strCommand);
        
        std::string strQueryBody;
        jsval_to_std_string(cx, args.get(1), &strQueryBody);
        
        int requestID = 0;
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->requestRestApi( strCommand.c_str(), strQueryBody.c_str() , &requestID );
        if( iErrorcode < 0 ){
            args.rval().set(int32_to_jsval(cx, iErrorcode));
        }
        else{
            args.rval().set(int32_to_jsval(cx, requestID));
        }
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_sendMessage(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strChannel;
        jsval_to_std_string(cx, args.get(0), &strChannel);
        
        std::string strContent;
        jsval_to_std_string(cx, args.get(1), &strContent);
        
        int requestID = 0;
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->sendMessage(strChannel.c_str(), strContent.c_str(), &requestID );
        if( iErrorcode < 0 ){
            args.rval().set(int32_to_jsval(cx, iErrorcode));
        }
        else{
            args.rval().set(int32_to_jsval(cx, requestID));
        }
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_kickOtherFromChannel(JSContext *cx, uint32_t argc, JS::Value *vp) {
    
    if (argc == 3)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        
        std::string strUserID;
        jsval_to_std_string(cx, args.get(0), &strUserID);
        
        std::string strChannel;
        jsval_to_std_string(cx, args.get(1), &strChannel);
        
        int lastTime = 0;
        jsval_to_int32(cx, args.get(2), &lastTime);
        
        int iErrorcode = IYouMeVoiceEngine::getInstance()->kickOtherFromChannel(strUserID.c_str(), strChannel.c_str(), lastTime );
        args.rval().set(int32_to_jsval(cx, iErrorcode));
        
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_setGrabMicOption(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : Invalid Native Object");
    if (argc == 5) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        int arg2 = 0;
        int arg3 = 0;
        unsigned int arg4 = 0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        ok &= jsval_to_int32(cx, args.get(3), (int32_t *)&arg3);
        ok &= jsval_to_uint32(cx, args.get(4), &arg4);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : Error processing arguments");
        int ret = cobj->setGrabMicOption(arg0, arg1, arg2, arg3, arg4);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setGrabMicOption : wrong number of arguments: %d, was expecting %d", argc, 5);
    return false;
}

bool js_cocos2dx_extension_talk_startGrabMicAction(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : Error processing arguments");
        int ret = cobj->startGrabMicAction(arg0, arg1);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_startGrabMicAction : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

bool js_cocos2dx_extension_talk_stopGrabMicAction(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : Invalid Native Object");
    if (argc == 2) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : Error processing arguments");
        int ret = cobj->stopGrabMicAction(arg0, arg1);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopGrabMicAction : wrong number of arguments: %d, was expecting %d", argc, 2);
    return false;
}

bool js_cocos2dx_extension_talk_requestGrabMic(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : Invalid Native Object");
    if (argc == 4) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        int arg2;
        const char* arg3 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        jsval_to_int32(cx, args.get(2),&arg2);
        std::string arg3_tmp; ok &= jsval_to_std_string(cx, args.get(3), &arg3_tmp); arg3 = arg3_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : Error processing arguments");
        int ret = cobj->requestGrabMic(arg0, arg1, (bool)arg2, arg3);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_requestGrabMic : wrong number of arguments: %d, was expecting %d", argc, 4);
    return false;
}

bool js_cocos2dx_extension_talk_releaseGrabMic(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : Invalid Native Object");
    if (argc == 1) {
        const char* arg0 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : Error processing arguments");
        int ret = cobj->releaseGrabMic(arg0);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_releaseGrabMic : wrong number of arguments: %d, was expecting %d", argc, 1);
    return false;
}

bool js_cocos2dx_extension_talk_setInviteMicOption(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        int arg1 = 0;
        int arg2 = 0;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        ok &= jsval_to_int32(cx, args.get(1), (int32_t *)&arg1);
        ok &= jsval_to_int32(cx, args.get(2), (int32_t *)&arg2);
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : Error processing arguments");
        int ret = cobj->setInviteMicOption(arg0, arg1, arg2);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_setInviteMicOption : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}

bool js_cocos2dx_extension_talk_requestInviteMic(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        const char* arg1 = nullptr;
        const char* arg2 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        std::string arg1_tmp; ok &= jsval_to_std_string(cx, args.get(1), &arg1_tmp); arg1 = arg1_tmp.c_str();
        std::string arg2_tmp; ok &= jsval_to_std_string(cx, args.get(2), &arg2_tmp); arg2 = arg2_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : Error processing arguments");
        int ret = cobj->requestInviteMic(arg0, arg1, arg2);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_requestInviteMic : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}

bool js_cocos2dx_extension_talk_responseInviteMic(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : Invalid Native Object");
    if (argc == 3) {
        const char* arg0 = nullptr;
        int arg1;
        const char* arg2 = nullptr;
        std::string arg0_tmp; ok &= jsval_to_std_string(cx, args.get(0), &arg0_tmp); arg0 = arg0_tmp.c_str();
        jsval_to_int32(cx, args.get(1), &arg1);
        std::string arg2_tmp; ok &= jsval_to_std_string(cx, args.get(2), &arg2_tmp); arg2 = arg2_tmp.c_str();
        JSB_PRECONDITION2(ok, cx, false, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : Error processing arguments");
        int ret = cobj->responseInviteMic(arg0, (bool)arg1, arg2);
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_responseInviteMic : wrong number of arguments: %d, was expecting %d", argc, 3);
    return false;
}

bool js_cocos2dx_extension_talk_stopInviteMic(JSContext *cx, uint32_t argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    /*
     JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
     js_proxy_t *proxy = jsb_get_js_proxy(obj);
     YouMeTalk* cobj = (YouMeTalk *)(proxy ? proxy->ptr : NULL);
     */
    IYouMeVoiceEngine* cobj = IYouMeVoiceEngine::getInstance();
    JSB_PRECONDITION2( cobj, cx, false, "js_cocos2dx_extension_YouMeTalk_stopInviteMic : Invalid Native Object");
    if (argc == 0) {
        int ret = cobj->stopInviteMic();
        JS::Value jsret = JSVAL_NULL;
        jsret = int32_to_jsval(cx, ret);
        args.rval().set(jsret);
        return true;
    }
    
    JS_ReportError(cx, "js_cocos2dx_extension_YouMeTalk_stopInviteMic : wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_ReleaseMicSync (JSContext *cx, uint32_t argc, JS::Value *vp)
{

    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp (argc, vp);

        int result = IYouMeVoiceEngine::getInstance ()->releaseMicSync ();
        args.rval ().set (int32_to_jsval (cx, result));

        return true;
    }
    JS_ReportError (cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

bool js_cocos2dx_extension_talk_ResumeMicSync (JSContext *cx, uint32_t argc, JS::Value *vp)
{

    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp (argc, vp);

        int result = IYouMeVoiceEngine::getInstance ()->resumeMicSync ();
        args.rval ().set (int32_to_jsval (cx, result));
        return true;
    }
    JS_ReportError (cx, "wrong number of arguments: %d, was expecting %d", argc, 0);
    return false;
}

void register_jsb_youmetalk(JSContext *cx, JS::HandleObject global) {
    
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
    js_cocos2dx_youmetalk_class = (JSClass *)calloc(1, sizeof(JSClass));
    js_cocos2dx_youmetalk_class->name = "YouMeTalk";
    js_cocos2dx_youmetalk_class->addProperty = JS_PropertyStub;
    js_cocos2dx_youmetalk_class->delProperty = JS_DeletePropertyStub;
    js_cocos2dx_youmetalk_class->getProperty = JS_PropertyStub;
    js_cocos2dx_youmetalk_class->setProperty = JS_StrictPropertyStub;
    js_cocos2dx_youmetalk_class->enumerate = JS_EnumerateStub;
    js_cocos2dx_youmetalk_class->resolve = JS_ResolveStub;
    js_cocos2dx_youmetalk_class->convert = JS_ConvertStub;
    js_cocos2dx_youmetalk_class->finalize = js_cocos2dx_YouMetalk_finalize;
    js_cocos2dx_youmetalk_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);
#else
    static const JSClassOps cocos2dx_youmetalk_classOps = {
        nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr,
        js_cocos2dx_YouMetalk_finalize,
        nullptr, nullptr, nullptr, nullptr
    };
    static JSClass cocos2dx_youmetalk_class = {
        "YouMeTalk",
        JSCLASS_HAS_PRIVATE | JSCLASS_FOREGROUND_FINALIZE,
        &cocos2dx_youmetalk_classOps
    };
    js_cocos2dx_youmetalk_class = &cocos2dx_youmetalk_class;
#endif
    static JSFunctionSpec funcs[] = { //初始化和反初始化
        JS_FN ("talk_Init", js_cocos2dx_extension_talk_Init, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_UnInit", js_cocos2dx_extension_talk_UnInit, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        JS_FN ("talk_SetServerRegion", js_cocos2dx_extension_talk_SetServerRegion, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        //设置状态

        JS_FN ("talk_SetOutputToSpeaker", js_cocos2dx_extension_talk_SetOutputToSpeaker, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetSpeakerMute", js_cocos2dx_extension_talk_SetSpeakerMute, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetMicrophoneMute", js_cocos2dx_extension_talk_SetMicrophoneMute, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetUseMobileNetworkEnabled", js_cocos2dx_extension_talk_SetUseMobileNetworkEnabled, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetVolume", js_cocos2dx_extension_talk_SetVolume, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetBackgroundMusicVolume", js_cocos2dx_extension_talk_SetBackgroundMusicVolume, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_GetBackgroundMusicVolume", js_cocos2dx_extension_talk_GetBackgroundMusicVolume, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),


        JS_FN ("talk_SetToken", js_cocos2dx_extension_talk_SetToken, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetAutoSendStatus", js_cocos2dx_extension_talk_SetAutoSendStatus, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_GetChannelUserList", js_cocos2dx_extension_talk_GetChannelUserList, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetMicLevelCallback", js_cocos2dx_extension_talk_SetMicLevelCallback, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetRecordingTimeMs", js_cocos2dx_extension_talk_SetRecordingTimeMs, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetPlayingTimeMs", js_cocos2dx_extension_talk_SetPlayingTimeMs, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetReleaseMicWhenMute", js_cocos2dx_extension_talk_SetReleaseMicWhenMute, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetExitCommModeWhenHeadsetPlugin", js_cocos2dx_extension_talk_SetExitCommModeWhenHeadsetPlugin, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        JS_FN ("talk_RequestRestApi", js_cocos2dx_extension_talk_RequestRestApi, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SendMessage", js_cocos2dx_extension_talk_sendMessage, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_KickOtherFromChannel", js_cocos2dx_extension_talk_kickOtherFromChannel, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),


        JS_FN ("talk_SetHeadsetMonitorOn", js_cocos2dx_extension_talk_SetHeadsetMonitorOn, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetReverbEnabled", js_cocos2dx_extension_talk_setReverbEnabled, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetVadCallbackEnabled", js_cocos2dx_extension_talk_SetVadCallbackEnabled, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        //获取状态
        JS_FN ("talk_GetSpeakerMute", js_cocos2dx_extension_talk_GetSpeakerMute, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_GetMicrophoneMute", js_cocos2dx_extension_talk_GetMicrophoneMute, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_GetUseMobileNetworkEnabled", js_cocos2dx_extension_talk_GetUseMobileNetworkEnabled, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_GetVolume", js_cocos2dx_extension_talk_GetVolume, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        //加入、离开会议
        JS_FN ("talk_JoinChannelSingleMode", js_cocos2dx_extension_talk_JoinChannelSingleMode, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_JoinChannelMultiMode", js_cocos2dx_extension_talk_JoinChannelMultiMode, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SpeakToChannel", js_cocos2dx_extension_talk_SpeakToChannel, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_LeaveChannelMultiMode", js_cocos2dx_extension_talk_LeaveChannelMultiMode, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_LeaveChannelAll", js_cocos2dx_extension_talk_LeaveChannelAll, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        JS_FN ("talk_SetOtherMicMute", js_cocos2dx_extension_talk_SetOtherMicMute, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetOtherSpeakerMute", js_cocos2dx_extension_talk_SetOtherSpeakerMute, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetListenOtherVoices", js_cocos2dx_extension_talk_SetListenOtherVoice, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        //播放、停止播放背景音乐
        JS_FN ("talk_PlayBackgroundMusic", js_cocos2dx_extension_talk_PlayBackgroundMusic, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_StopBackgroundMusic", js_cocos2dx_extension_talk_StopBackgroundMusic, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        //暂停、恢复会议
        JS_FN ("talk_PauseChannel", js_cocos2dx_extension_talk_PauseChannel, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_ResumeChannel", js_cocos2dx_extension_talk_ResumeChannel, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        //获取、设置变调效果
        JS_FN ("talk_GetSoundtouchPitchSemiTones", js_cocos2dx_extension_talk_GetSoundtouchPitchSemiTones, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_SetSoundtouchPitchSemiTones", js_cocos2dx_extension_talk_SetSoundtouchPitchSemiTones, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        //获取、设置角色身份
        JS_FN ("talk_SetUserRole", js_cocos2dx_extension_talk_SetUserRole, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_GetUserRole", js_cocos2dx_extension_talk_GetUserRole, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        //抢麦连麦
        JS_FN ("setGrabMicOption", js_cocos2dx_extension_talk_setGrabMicOption, 5, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("setInviteMicOption", js_cocos2dx_extension_talk_setInviteMicOption, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("stopInviteMic", js_cocos2dx_extension_talk_stopInviteMic, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("responseInviteMic", js_cocos2dx_extension_talk_responseInviteMic, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("requestGrabMic", js_cocos2dx_extension_talk_requestGrabMic, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("startGrabMicAction", js_cocos2dx_extension_talk_startGrabMicAction, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("requestInviteMic", js_cocos2dx_extension_talk_requestInviteMic, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("stopGrabMicAction", js_cocos2dx_extension_talk_stopGrabMicAction, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("releaseGrabMic", js_cocos2dx_extension_talk_releaseGrabMic, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        //设置接收白名单
        JS_FN ("talk_SetWhiteUserList", js_cocos2dx_extension_talk_SetWhiteUserList, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_ReleaseMicSync", js_cocos2dx_extension_talk_ReleaseMicSync, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN ("talk_ResumeMicSync", js_cocos2dx_extension_talk_ResumeMicSync, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        JS_FS_END
    };

    static JSFunctionSpec st_funcs[] = {
        JS_FS_END
    };
    
#ifdef JSB_AUTOCOMPARTMENT_WITH_GLOBAL_OBJCET
    js_cocos2dx_youmetalk_prototype = JS_InitClass(
                                                   cx, global,
                                                   JS::NullPtr(),
                                                   js_cocos2dx_youmetalk_class,
                                                   js_cocos2dx_extension_YouMetalk_constructor, 0, // constructor
                                                   NULL,
                                                   funcs,
                                                   NULL, // no static properties
                                                   st_funcs);
    anonEvaluate(cx, global, "(function () { return YouMeTalk; })()").toObjectOrNull();
#else
    js_cocos2dx_youmetalk_prototype = JS_InitClass(
                                                   cx, global,
                                                   nullptr,
                                                   js_cocos2dx_youmetalk_class,
                                                   js_cocos2dx_extension_YouMetalk_constructor, 0, // constructor
                                                   NULL,
                                                   funcs,
                                                   NULL, // no static properties
                                                   st_funcs);
    JS::RootedObject proto(cx, js_cocos2dx_youmetalk_prototype);
    jsb_register_class<CYouMeTalkJsWrapper>(cx, js_cocos2dx_youmetalk_class, proto);
#endif
}



