var youmetalk = new YouMeTalk;

var g_userID = "userID";
var g_roomName = "room";
var g_roleType = 0;

var addTips = function( strTips ){
    var event = new cc.EventCustom("Tips");
    event.setUserData( strTips );
    cc.eventManager.dispatchEvent( event );
}

youmetalk.OnRequestRestApi = function( requestid, errcode, command, result ){
    cc.log("OnRequestRestApi:%d,%d", requestid, errcode );
    cc.log("command:%s", command );
    cc.log("result:%s", result );
}

youmetalk.OnMemberChange = function (channel, memberListJsonString, isUpdate){
    cc.log("OnMemberChange");
    cc.log("OnMemberChange:%s,%s,%s", channel,  memberListJsonString ,isUpdate);
    //memberListJsonString eg: {"channelid":"123","memchange":[{"isJoin":true,"userid":"u541"},{"isJoin":true,"userid":"u948"}]}
}

youmetalk.OnBroadcast = function(bctype, channel, param1, param2, content){
	switch( bctype){
		case 1:	//YOUME_BROADCAST_GRABMIC_BROADCAST_GETMIC
			cc.log("有人抢到了麦");
			break;
		case 2:	//YOUME_BROADCAST_GRABMIC_BROADCAST_FREEMIC
			cc.log("有人释放了麦");
			break;
		case 3:	//YOUME_BROADCAST_INVITEMIC_BROADCAST_CONNECT
			cc.log("A和B正在连麦");
			break;
		case 4:	//YOUME_BROADCAST_INVITEMIC_BROADCAST_DISCONNECT
			cc.log("A和B取消了连麦");
			break;
		default:
			break;
	}
}

youmetalk.OnEvent = function( event,  errorcode, channel, param )
{
    cc.log("OnEvent:event_"+event + ",error_" + errorcode + ",channel_" + channel + ",param_" + param)
    switch( event ){
         case 0: //YOUME_EVENT_INIT_OK:
            cc.log("Talk 初始化成功");
            addTips("初始化成功");
            break;
        case 1://YOUME_EVENT_INIT_FAILED:
            cc.log("Talk 初始化失败");
            addTips("初始化失败");
            break;
        case 2://YOUME_EVENT_JOIN_OK:
        {
            cc.log("Talk 进入房间成功");
            addTips( "进入房间成功，ID：" + channel  );
            var event = new cc.EventCustom("EnterRoom");
            cc.eventManager.dispatchEvent( event );
        }
            break;
        case 3://YOUME_EVENT_JOIN_FAILED:
            cc.log("Talk 进入房间失败");
            addTips( ssTips.str() );
            break;
        case 4://YOUME_EVENT_LEAVED_ONE:
            addTips("退出房间");
            cc.log("Talk 离开单个房间");
            break;
        case 5://YOUME_EVENT_LEAVED_ALL:
            addTips("退出房间");
            cc.log("Talk 离开所有房间");
            break;
        case 6://YOUME_EVENT_PAUSED:
            addTips("暂停");
            cc.log("Talk 暂停");
            break;
        case 7://YOUME_EVENT_RESUMED:
            addTips("恢复");
            cc.log("Talk 恢复");
            break;
        case 8://YOUME_EVENT_SPEAK_SUCCESS:///< 切换对指定频道讲话成功（适用于多频道模式）
            break;
        case 9://YOUME_EVENT_SPEAK_FAILED:///< 切换对指定频道讲话失败（适用于多频道模式）
            break;
        case 10://YOUME_EVENT_RECONNECTING:///< 断网了，正在重连
            cc.log("Talk 正在重连");
            break;
        case 11://YOUME_EVENT_RECONNECTED:///< 断网重连成功
            cc.log("Talk 重连成功");
            break;
        case 12://YOUME_EVENT_REC_PERMISSION_STATUS:///< 通知录音权限状态，成功获取权限时错误码为YOUME_SUCCESS，获取失败为YOUME_ERROR_REC_NO_PERMISSION（此时不管麦克风mute状态如何，都没有声音输出）
            cc.log("录音权限状态,%d",errorcode);
            break;
        case 13://YOUME_EVENT_BGM_STOPPED:///< 通知背景音乐播放结束
            cc.log("背景音乐播放结束,%s",param);
            addTips( "背景音乐播放结束" );
            break;
        case 14://YOUME_EVENT_BGM_FAILED:///< 通知背景音乐播放失败
            cc.log("背景音乐播放失败,%s",param);
            addTips( "背景音乐播放失败" );
            break;
        case 15://YOUME_EVENT_MEMBER_CHANGE:///< 频道成员变化
            cc.log("频道成员变化,%s",param);
            break;
        case 16://YOUME_EVENT_OTHERS_MIC_ON:///< 其他用户麦克风打开
            cc.log("其他用户麦克风打开,%s",param);
            break;
        case 17://YOUME_EVENT_OTHERS_MIC_OFF:///< 其他用户麦克风关闭
            cc.log("其他用户麦克风关闭,%s",param);
            break;
        case 18://YOUME_EVENT_OTHERS_SPEAKER_ON:///< 其他用户扬声器打开
            cc.log("其他用户扬声器打开,%s",param);
            break;
        case 19://YOUME_EVENT_OTHERS_SPEAKER_OFF: ///< 其他用户扬声器关闭
            cc.log("其他用户扬声器关闭,%s",param);
            break;
        case 20://YOUME_EVENT_OTHERS_VOICE_ON: ///<其他用户进入讲话状态
            cc.log("其他用户进入讲话状态,%s",param);
            break;
        case 21://YOUME_EVENT_OTHERS_VOICE_OFF: ///<其他用户进入静默状态
            cc.log("其他用户进入静默状态,%s",param);
            break;
        case 22://YOUME_EVENT_MY_MIC_LEVEL: ///<麦克风的语音级别
            cc.log("麦克风的语音级别,%s",param);
            break;
        case 23://YOUME_EVENT_MIC_CTR_ON: ///<麦克风被其他用户打开
            cc.log("麦克风被其他用户打开,%s",param);
            break;
        case 24://YOUME_EVENT_MIC_CTR_OFF: ///<麦克风被其他用户关闭
            cc.log("麦克风被其他用户关闭,%s",param);
            break;
        case 25://YOUME_EVENT_SPEAKER_CTR_ON: ///<扬声器被其他用户打开
            cc.log("扬声器被其他用户打开,%s",param);
            break;
        case 26://YOUME_EVENT_SPEAKER_CTR_OFF: ///<扬声器被其他用户关闭
            cc.log("扬声器被其他用户关闭,%s",param);
            break;
        case 27://YOUME_EVENT_LISTEN_OTHER_ON: ///<取消屏蔽某人语音
            cc.log("取消屏蔽某人语音,%s",param);
            break;
        case 28://YOUME_EVENT_LISTEN_OTHER_OFF: ///<屏蔽某人语音
            cc.log("屏蔽某人语音,%s",param);
            break;
        default:
            break;
    }
};






