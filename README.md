# HIKSDK
阿里云：https://www.aliyundrive.com/s/U6WvVLhx8v3  
提取码：44tk  
压缩包密码：dzming-git
# HIKDLL
阿里云：https://www.aliyundrive.com/s/CcLDXsi67TH   
提取码：0gn0  
压缩包密码：dzming-git
# PlayCtrl
阿里云：https://www.aliyundrive.com/s/BZjeEmb39Xb  
提取码：4d1q  
压缩包密码：dzming-git
# 文件结构
├─Dependent  
│  ├─HIKSDK  
│  │  ├─Include  
│  │  │      DataType.h  
│  │  │      DecodeCardSdk.h  
│  │  │      HCNetSDK.h  
│  │  │  
│  │  └─Lib  
│  │      │  GdiPlus.lib  
│  │      │  HCCore.lib  
│  │      │  HCNetSDK.lib  
│  │      │  
│  │      └─HCNetSDKCom  
│  │              HCAlarm.lib  
│  │              HCGeneralCfgMgr.lib  
│  │              HCPreview.lib  
│  │  
│  └─PlayCtrl  
│          PlayCtrl.lib  
│          PlayM4.h  
│          WindowsPlayM4.h  
## DLL
dll文件放在Relese文件夹下，与生成的*.exe文件在同一目录  
下列文件在HIKDLL中  
HCCore.dll  
HCCoreDevCfg.dll  
HCNetSDK.dll  
HCPreview.dll  
libeay32.dll  
ssleay32.dll  
SystemTransform.dll  
下列文件在PlayCtrl中，不可以使用HIKDLL中的dll  
AudioRender.dll  
PlayCtrl.dll  
SuperRender.dll  
下列文件在opencv中  
opencv_world345.dll  
下列文件由该项目生成
hikvision.dll  