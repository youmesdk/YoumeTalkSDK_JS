# Talk SDK for Cocos2d Js API接口手册

## 相关异步/同步处理方法介绍
游密语音引擎SDK提供的全部为Js接口,接口调用都会立即返回,凡是本身需要较长耗时的接口调用都会采用异步回调的方式,所有接口都可以在主线程中直接使用。回调在主线程。

## API调用说明
添加如下代码：
`var youmetalk = new YouMeTalk;`
API的调用可使用“youmetalk.”来直接操作。
接口使用的基本流程为`初始化`->`收到初始化成功回调通知`->`加入语音频道`->`收到加入频道成功回调通知`->`使用其它接口`->`离开语音频道`->`反初始化`，要确保严格按照上述的顺序使用接口。

## 实现回调
需要实现以下回调：

  ``` js
  //通用事件回调
  //event：参见YouMeEvent定义
  //errorcode：参见YouMeErrorCode定义
  youmetalk.OnEvent = function( event,  errorcode, channel, param ){
     cc.log("OnEvent:event_"+event + ",error_" + errorcode + ",channel_" + channel + ",param_" + param)

  }

  //RestApi回调
  youmetalk.OnRequestRestApi = function( requestid, errcode, query, result ){
  }

 //频道内用户列表的回调
 // channel: 频道
 // memberListJsonString: 变化的用户列表
 // isUpdate: false为第一次查询，true为增量通知
 youmetalk.OnMemberChange = function (channel,  memberListJsonString, isUpdate){
    cc.log("OnMemberChange:%s,%s,%s", channel,  memberListJsonString ,isUpdate);
    //memberListJsonString eg:
    //{"channelid":"123","memchange":[{"isJoin":true,"userid":"u541"},{"isJoin":true,"userid":"u948"}]}
}
 ```

## 初始化
* **语法**

```
talk_Init( strAppKey, strAPPSecret, serverRegionId, strExtServerRegionName );
```

* **功能**
初始化语音引擎，做APP验证和资源初始化。（只有appkey,appsecret,serverRegionId 都一样时,才可以进行语音互通,改任意一个视为另外一个区服）;

* **参数说明**
`strAPPKey`：string，从游密后台申请到的 app key, 这个是你们应用程序的唯一标识。
`strAPPSecret`：string， 对应 strAPPKey 的私钥, 这个需要妥善保存，不要暴露给其他人。
`serverRegionId`：int，设置首选连接服务器的区域码，参见状态码的YOUME_RTC_SERVER_REGION定义。如果在初始化时不能确定区域，可以填10001，后面确定时通过 talk_SetServerRegion 设置。如果YOUME_RTC_SERVER_REGION定义的区域码不能满足要求，可以把这个参数设为10000，然后通过后面的参数strExtServerRegionName 设置一个自定的区域值（如中国用 "cn" 或者 “ch"表示），然后把这个自定义的区域值同步给游密，我们将通过后台配置映射到最佳区域的服务器。
`strExtServerRegionName`：string,自定义的扩展的服务器区域名。可为空字符串“”。只有前一个参数serverRegionId设为10000时，此参数才有效（否则都将当空字符串“”处理）。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
// 涉及到的主要回调事件有：
// OnEvent
// event: （0） - 表明初始化成功
// event: （1） - 表明初始化失败，最常见的失败原因是网络错误或者 AppKey-AppSecret 错误

```
## 语音频道管理

### 加入语音频道（单频道）

* **语法**

```
talk_JoinChannelSingleMode ( strUserID, strChannelID, roleType, bCheckRoomExist);
```
* **功能**
加入语音频道（单频道模式，每个时刻只能在一个语音频道里面）。

* **参数说明**
`strUserID`：string,全局唯一的用户标识，全局指在当前应用程序的范围内。
`strChannelID`：string, 全局唯一的频道标识，全局指在当前应用程序的范围内。
`roleType`：int,用户在语音频道里面的角色，见YouMeUserRole定义。
`bCheckRoomExist `：是否检查频道存在时才加入，默认为false: true 当频道存在时加入、不存在时返回错误（多用于观众角色），false 无论频道是否存在都加入频道

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//涉及到的主要回调事件有：
//OnEvent
//event:2 - 成功进入语音频道
//event:3 - 进入语音频道失败，可能原因是网络或服务器有问题，或是bCheckRoomExist为true时频道还未创建
```

### 加入语音频道（多频道）

* **语法**

```
talk_JoinChannelMultiMode ( strUserID,strChannelID,iRoleType,bCheckRoomExist);
```
* **功能**
加入语音频道（多频道模式，可以同时听多个语音频道的内容，但每个时刻只能对着一个频道讲话）。

* **参数说明**
`strUserID`：string,全局唯一的用户标识，全局指在当前应用程序的范围内。
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。
`iRoleType` ：YouMeUserRole_t，用户在语音频道里面的角色，见YouMeUserRole定义
`bCheckRoomExist `：是否检查频道存在时才加入，默认为false: true 当频道存在时加入、不存在时返回错误（多用于观众角色），false 无论频道是否存在都加入频道

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
// 涉及到的主要回调事件有：
// OnEvent
// event: 2 - 成功进入语音频道
// event: 3 - 进入语音频道失败，进入语音频道失败，可能原因是网络或服务器有问题，或是bCheckRoomExist为true时频道还未创建
```

### 指定讲话频道

* **语法**

```
talk_SpeakToChannel ( strChannelID);
```
* **功能**
多频道模式下，指定当前要讲话的频道。

* **参数说明**
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//涉及到的主要回调事件有：
// OnEvent
// event: 8 - 成功切入到指定语音频道
// event: 9 - 切入指定语音频道失败，可能原因是网络或服务器有问题

```

### 退出指定的语音频道
* **语法**

```
talk_LeaveChannelMultiMode (strChannelID);
```
* **功能**
多频道模式下，退出指定的语音频道。

* **参数说明**
`strChannelID`：string,全局唯一的频道标识，全局指在当前应用程序的范围内。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//涉及到的主要回调事件有：
// OnEvent
// event: 4 - 退出指定语音频道完成
```

### 退出所有语音频道

* **语法**

```
talk_LeaveChannelAll ();
```
* **功能**
退出所有的语音频道（单频道模式下直接调用此函数离开频道即可）。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//涉及到的主要回调事件有：
// OnEvent
// event: 5 - 退出所有语音频道完成
```

### 设置白名单用户

* **语法**

```
talk_SetWhiteUserList ( strChannelID, strWhiteUserList);
```
* **功能**
设置当前用户的语音消息接收白名单，其语音消息只会转发到白名单的用户，不设置该接口则默认转发至频道内所有人。

* **参数说明**
`strChannelID`：要设置的频道(兼容多频道模式，单频道模式下传入当前频道即可)。
`strWhiteUserList`：白名单用户列表, 以|分隔，如：User1|User2|User3；表示语音消息只发送到User1,User2,User3. "all"表示转发至频道内所有人；""（空字符串）表示不转发至任何用户。

* **返回值**
int，返回0才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//涉及到的主要回调事件有：
// OnEvent
// event: 62 - 成功在指定语音频道设置白名单，有异常用户会返回错误码-501
// event: 63 - 在指定语音频道设置白名单失败，可能原因是网络或服务器有问题

```

## 设备状态管理

### 切换语音输出设备

* **语法**

```
talk_SetOutputToSpeaker (bOutputToSpeaker);
```
* **功能**
默认输出到扬声器，在加入房间成功后设置（iOS受系统限制，如果已释放麦克风则无法切换到听筒）

* **参数说明**
`bOutputToSpeaker `：true——输出到扬声器，false——输出到听筒。

* **返回值**
int，如果成功返回0，否则返回错误码，请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

### 设置扬声器状态

* **语法**

```
talk_SetSpeakerMute (mute);
```
* **功能**
打开/关闭扬声器。**该状态值在加入房间成功后设置才有效。**（该接口是异步接口,调用后会有一段时间生效）


* **参数说明**
`mute`:true——关闭扬声器，false——开启扬声器。

### 获取扬声器状态

* **语法**

```
talk_GetSpeakerMute();
```

* **功能**
获取当前扬声器状态。（该接口为异步接口,尽量避免逻辑上设置了扬声器状态后立刻调用该接口）

* **返回值**
true——扬声器关闭，false——扬声器开启。

### 设置麦克风状态

* **语法**

```
talk_SetMicrophoneMute (mute);
```

* **功能**
打开／关闭麦克风。**该状态值在加入房间成功后设置才有效。**（该接口是异步接口,调用后会有一段时间生效）


* **参数说明**
`mute`:true——关闭麦克风，false——开启麦克风。

### 获取麦克风状态

* **语法**

```
talk_GetMicrophoneMute ();
```

* **功能**
获取当前麦克风状态。（该接口为异步接口,尽量避免逻辑上设置了扬声器状态后立刻调用该接口）

* **返回值**
true——麦克风关闭，false——麦克风开启。

### 设置是否通知别人麦克风和扬声器的开关

* **语法**

```
talk_SetAutoSendStatus( bAutoSend );
```

* **功能**
设置是否通知别人,自己麦克风和扬声器的开关状态（建议进入频道后调用，设置后其他当前在同一个频道的用户可以获取该用户的麦克风和扬声器的开关状态 对应回调事件YouMeEvent.YOUME_EVENT_OTHERS_MIC_ON,YouMeEvent.YOUME_EVENT_OTHERS_MIC_OFF,YouMeEvent.YOUME_EVENT_OTHERS_SPEAKER_ON,YouMeEvent.YOUME_EVENT_OTHERS_SPEAKER_OFF,事件对应具体详细请参考状态码）

* **参数说明**
`bAutoSend`:true——通知，false——不通知。


* **相关回调**

```
// 涉及到的主要回调事件有(房间里的其他人会收到)：
// OnEvent
// event: 16 - 其他用户麦克风打开
// event: 17 - 其他用户麦克风关闭
// event: 18 - 其他用户扬声器打开
// event: 19 - 其他用户扬声器关闭
```

### 设置音量

* **语法**

```
talk_SetVolume (uiVolume);
```

* **功能**
设置当前程序输出音量大小。建议该状态值在加入房间成功后按需再重置一次。

* **参数说明**
`uiVolume`:unsigned int，当前音量大小，范围[0-100]。

### 获取音量

* **语法**

```
talk_GetVolume ();
```

* **功能**
获取当前程序输出音量大小。

* **返回值**
unsigned int ，当前音量大小，范围[0-100]。

## 设置网络

### 设置是否允许使用移动网络

* **语法**

```
talk_SetUseMobileNetworkEnabled (bEnabled);
```

* **功能**
设置是否允许使用移动网络。在WIFI和移动网络都可用的情况下会优先使用WIFI，在没有WIFI的情况下，如果设置允许使用移动网络，那么会使用移动网络进行语音通信，否则通信会失败。


* **参数说明**
`bEnabled`:true——允许使用移动网络，false——禁止使用移动网络。

### 获取是否允许使用移动网络

* **语法**

```
talk_GetUseMobileNetworkEnabled () ;
```

* **功能**
获取是否允许SDK在没有WIFI的情况使用移动网络进行语音通信。

* **返回值**
true——允许使用移动网络，false——禁止使用移动网络，默认情况下允许使用移动网络。

## 控制他人麦克风

* **语法**

```
talk_SetOtherMicMute ( strUserID, mute);
```

* **功能**
控制他人的麦克风状态（也就是关闭或者开启他人的麦克风）

* **参数说明**
`strUserID`：string,要控制的用户ID
`mute`：是否静音。true:静音别人的麦克风，false：开启别人的麦克风

* **返回值**
int，如果成功返回0，否则返回错误码，请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **相关回调**
```
// 涉及到的主要回调事件有(被控制方收到)：
// OnEvent
// event: 23 - 麦克风被其他用户打开
// event: 24 - 麦克风被其他用户关闭
```

## 控制他人扬声器

* **语法**

```
talk_SetOtherSpeakerMute ( strUserID, mute);
```

* **功能**
控制他人的扬声器状态（也就是关闭或者开启他人的扬声器）

* **参数说明**
`strUserID`：string,要控制的用户ID
`mute`：是否静音。true:静音别人的扬声器，false：开启别人的扬声器

* **返回值**
int，如果成功返回0，否则返回错误码，请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **相关回调**
```
// 涉及到的主要回调事件有(被控制方收到)：
// OnEvent
// event: 25 - 扬声器被其他用户打开
// event: 26 - 扬声器被其他用户关闭
```

## 设置是否听某人的语音

* **语法**

```
talk_SetListenOtherVoices (strUserID, on);
```

* **功能**
设置是否听某人的语音。

* **参数说明**
`strUserID`：string,要控制的用户ID。
`on`：true表示开启接收指定用户的语音，false表示屏蔽指定用户的语音。

* **返回值**
int，如果成功返回0，否则返回错误码，请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**
```
// 涉及到的主要回调事件有(被控制方收到)：
// OnEvent
// event: 27 - 取消屏蔽某人语音
// event: 28 - 屏蔽某人语音
```

## 设置当麦克风静音时，是否释放麦克风设备

* **语法**
```
talk_SetReleaseMicWhenMute(enabled);
```

* **功能**
设置当麦克风静音时，是否释放麦克风设备（需要在初始化成功后，加入房间之前调用）

* **参数说明**
`enabled`： true--当麦克风静音时，释放麦克风设备，此时允许第三方模块使用麦克风设备录音。在Android和iOS上，语音由通话音轨切换到媒体音轨，回归到双声道高音质输出体验（如果媒体音量和通话音量不同时切换会产生音量变大变小的感觉）；false--不管麦克风是否静音，麦克风设备都会被占用。

* **返回值**
返回YOUME_SUCCESS才会有异步回调通知。其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

## 设置插入耳机时，是否自动退出系统通话模式(禁用手机系统硬件信号前处理)

* **语法**
```
talk_SetExitCommModeWhenHeadsetPlugin(enabled);
```

* **功能**
设置插入耳机时，是否自动退出系统通话模式(禁用手机系统硬件信号前处理)
系统提供的前处理效果包括回声消除、自动增益等，有助于抑制背景音乐等回声噪音，减少系统资源消耗，对Windows系统无效
由于插入耳机可从物理上阻断回声产生，故可设置禁用该效果以保留背景音乐的原生音质效果
默认为false，在初始化之后、加入房间之前调用

* **参数说明**
`enabled`： true--当插入耳机时，自动禁用系统硬件信号前处理，拔出时还原；false--插拔耳机不做处理。

* **返回值**
成功返回YOUME_SUCCESS，其它返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

## 通话管理

### 暂停通话

* **语法**

```
talk_PauseChannel();
```

* **功能**
暂停通话，释放对麦克风等设备资源的占用。当需要用第三方模块临时录音时，可调用这个接口。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//主要回调事件：
// OnEvent
// event: 6 - 暂停语音频道完成
```

### 恢复通话

* **语法**

```
talk_ResumeChannel();
```

* **功能**
恢复通话，调用PauseChannel暂停通话后，可调用这个接口恢复通话。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//主要回调事件：
// OnEvent
// event: 7 - 恢复语音频道完成
```

## 设置语音检测

* **语法**

```
talk_SetVadCallbackEnabled(bEnabled);
```

* **功能**
设置是否开启语音检测回调，开启后频道内有人正在讲话与结束讲话都会发起相应回调通知。（该状态值在加入房间成功后设置才有效，并且需要联系游密官方人员进行开启后方才生效）

* **参数说明**
`bEnabled`:true——打开，false——关闭。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

## 设置是否开启讲话音量回调

* **语法**


```
talk_SetMicLevelCallback(int maxLevel)

```

* **功能**
设置是否开启监听自己讲话音量回调, 并设置相应的参数。

* **参数说明**
`maxLevel`:音量最大时对应的级别，最大可设100。根据实际需要设置小于100的值可以减少回调的次数（注意设置较高的值可能会产生大量回调，特别在Unity上会影响其它事件到达，一般建议不超过30）。比如你只在UI上呈现10级的音量变化，那就设10就可以了。设 0 表示关闭回调。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。


## 背景音乐管理

### 播放背景音乐

* **语法**

```
talk_PlayBackgroundMusic ( strFilePath, bRepeat);
```

* **功能**
播放指定的音乐文件。播放的音乐将会通过扬声器输出，并和语音混合后发送给接收方。这个功能必须进入频道传入身份为主播/指挥才能使用。

* **参数说明**
`strFilePath`：音乐文件的路径。
`bRepeat`：是否重复播放，true——重复播放，false——只播放一次就停止播放。

* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//主要回调事件：
// OnEvent
// event: 13 - 通知背景音乐播放结束
// event: 14 - 通知背景音乐播放失败
```

### 停止播放背景音乐
* **语法**

```
talk_StopBackgroundMusic();
```

* **功能**
停止播放当前正在播放的背景音乐。
这是一个同步调用接口，函数返回时，音乐播放也就停止了。

* **返回值**
int，如果成功返回0，表明成功停止了音乐播放流程；否则返回错误码，请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

### 设置背景音乐播放音量
* **语法**

```
talk_SetBackgroundMusicVolume(vol);
```

* **功能**
设定背景音乐的音量。这个接口用于调整背景音乐和语音之间的相对音量，使得背景音乐和语音混合听起来协调。
这是一个同步调用接口。

* **参数说明**
`vol`:int,背景音乐的音量，范围 [0-100]。

* **返回值**
int，如果成功（表明成功设置了背景音乐的音量）返回0，否则返回错误码，具体请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

## 设置监听
* **语法**

```
talk_SetHeadsetMonitorOn(micEnabled, bgmEnabled);
```

* **功能**
设置是否用耳机监听自己的声音或背景音乐，当不插耳机时，这个设置不起作用
这是一个同步调用接口。

* **参数说明**
`micEnabled`:是否监听麦克风 true 监听，false 不监听。
`bgmEnabled`:是否监听背景音乐 true 监听，false 不监听。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

## 获取变声音调

* **语法**
```
talk_GetSoundtouchPitchSemiTones ();
```

* **功能**
获取变声音调（增值服务，需要后台配置开启）

* **返回值**
int, 当前变声音调的值，范围[-12 ~ +12]。

## 设置变声音调

* **语法**
```
talk_SetSoundtouchPitchSemiTones (int fPitchSemiTones);
```

* **功能**
设置变声音调（增值服务，需要后台配置开启），需在进入房间成功后调用，仅对当前房间有效，退出房间时重置为0

* **参数说明**
`fPitchSemiTones`:变声音调的值，范围[-12 ~ +12]。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

## 设置混响音效

* **语法**

```
talk_SetReverbEnabled( bEnabled);
```

* **功能**
设置是否开启混响音效，这个主要对主播/指挥有用。

* **参数说明**
`bEnabled`:true——打开，false——关闭。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

## 设置时间戳
### 设置录音时间戳

* **语法**

```
talk_SetRecordingTimeMs( timeMs);
```

* **功能**
设置当前录音的时间戳。当通过录游戏脚本进行直播时，要保证观众端音画同步，在主播端需要进行时间对齐。
这个接口设置的就是当前游戏画面录制已经进行到哪个时间点了。

* **参数说明**
`timeMs`:unsigned int, 当前游戏画面对应的时间点，单位为毫秒。

* **返回值**
无。

### 设置播放时间戳

* **语法**

```
talk_SetPlayingTimeMs( timeMs);
```

* **功能**
设置当前声音播放的时间戳。当通过录游戏脚本进行直播时，要保证观众端音画同步，游戏画面的播放需要和声音播放进行时间对齐。
这个接口设置的就是当前游戏画面播放已经进行到哪个时间点了。

* **参数说明**
`timeMs`:unsigned int,当前游戏画面播放对应的时间点，单位为毫秒。

* **返回值**
无。

## 设置服务器区域

* **语法**

```
talk_SetServerRegion(serverRegionId,strExtRegionName);
```

* **功能**
设置首选连接服务器的区域码,初始化前调用，新版本以集成到Init里面，参数三，不填默认为中国

* **参数说明**
`serverRegionId`：int,如果YOUME_RTC_SERVER_REGION定义的区域码不能满足要求，可以把这个参数设为 10000，然后通过后面的参数strExtServerRegionName 设置一个自定的区域值（如中国用 "cn" 或者 “ch"表示），然后把这个自定义的区域值同步给游密，我们将通过后台配置映射到最佳区域的服务器。
`strExtServerRegionName`：string,自定义的扩展的服务器区域名。可为空字符串“”。只有前一个参数serverRegionId设为10000时，此参数才有效（否则都将当空字符串“”处理）。


## RestApi——支持主播相关信息查询
* **语法**

```
talk_RequestRestApi( strCommand , strQueryBody  );
```
* **功能**
Rest API , 向服务器请求额外数据。支持主播信息，主播排班等功能查询。需要的话，请联系我们获取命令详情文档。

* **参数说明**
`strCommand`：请求的命令字符串，标识命令类型。
`strQueryBody`：请求需要的参数,json格式。

* **代码示例**

```
var body = "{\"ChannelID\":\"123456\",\"AreaID\":0}";
var errorcode = youmetalk.talk_RequestRestApi( "query_talk_channel_user_count", body);

```

* **返回值**
int，小于0，表示错误码，大于0，表示本次查询的requestID。

* **异步回调**

```
//requestID:int,回传ID
//iErrorCode:int,错误码
//query:string,回传查询请求，json格式，包含command（回传strCommand参数）和query（回传strQueryBody参数）字段。
//result:string,查询结果，json格式。
onRequestRestAPI( requestid, errcode, query, result )
```

* **回调示例**

```
query:{"command":"query_talk_channel_user_count","query":"{\"ChannelID\":\"123456\",\"AreaID\":0}"}
result:{"ActionStatus":"OK","ChannelID":"123456","ErrorCode":0,"ErrorInfo":"","UserCount":0}
```

## 安全验证码设置

* **语法**

```
talk_SetToken( strToken );
```

* **功能**
设置身份验证的token，需要配合后台接口。

* **参数说明**
`strToken`：string,身份验证用token，设置空字符串，清空token值，不进行身份验证。

## 查询频道用户列表

* **语法**

```
talk_GetChannelUserList( strChannelID,maxCount, notifyMemChange );
```

* **功能**
查询频道当前的用户列表， 并设置是否获取频道用户进出的通知。（必须自己在频道中）

* **参数说明**
`strChannelID`：string,频道ID。
`maxCount`：int,想要获取的最大人数。-1表示获取全部列表。
`notifyMemChange`：其他用户进出房间时，是否要收到通知。（设置为true，自己所在频道有人进出 都会有对应回调产生，反之false就没有对应回调）
* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。

* **异步回调**

```
//strChannel:string,频道ID
//memchanges:对象数组，查询获得的用户列表，或变更列表。
//每一个数组的数据，包含userID和isJoin两个字段
onMemberChange(channel,  memberListJsonString, isUpdate)
```

## 广播消息
* **语法**

```
talk_SendMessage( channelID,  content );
```

* **功能**
在语音频道内，广播一个文本消息。


* **参数说明**
`channelID`：频道ID（自己需要进入这个频道）。
`content`：要广播的文本内容。

* **返回值**
int，小于0，表示错误码，大于0，表示本次查询的requestID。

* **异步回调**

```
// 涉及到的主要回调事件有：
// OnEvent
// event: （60） - 发送消息的结果，param为requestID的字符串
// event: （61） - 频道内其他人收到消息的通知。param为文本内容

```

## 把人踢出房间
* **语法**

```
talk_KickOtherFromChannel( userid, channelid, lastTime );
```

* **功能**
把人踢出房间。


* **参数说明**
`userid`：被踢的用户ID。
`channelid`：从哪个房间踢出（自己需要在房间）。
`lastTime`：踢出后，多长时间内不允许再次进入（单位：秒）。


* **返回值**
int，返回0才会有异步回调通知。其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。


* **异步回调**

```
// 涉及到的主要回调事件有：
// OnEvent
// event: （64） - 踢人方收到，发送消息的结果回调，param为被踢者ID
// event: （65） - 被踢方收到，被踢通知，会自动退出所在频道。param: （踢人者ID，被踢原因，被禁时间）


```

## 设置日志等级
* **语法**

```
talk_SetLogLevel( level)
```

* **功能**
设置日志等级


* **参数说明**
`level`：日志等级。小于等于该等级的日志会打印。有效值参考YOUME_LOG_LEVEL类型定义。


## 反初始化

* **语法**

```
talk_UnInit ();
```

* **功能**
反初始化引擎，可在退出游戏时调用，以释放SDK所有资源。

* **返回值**
int，如果成功则返回0，其他返回值请参考[YouMeErrorCode类型定义](TalkCocosJsStatusCode.php#YouMeErrorCode定义)。


