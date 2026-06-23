#include <libkern/libkern.h>
#include <mach/mach_types.h>
#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>

#define JK_PR(fmt, ...)  IOLog("[JackiOS] " fmt "\n", ##__VA_ARGS__)

class com_jackandroid_JackAndroid : public IOService {
    OSDeclareDefaultStructors(com_jackandroid_JackAndroid)
public:
    virtual bool start(IOService *provider) override;
    virtual void stop(IOService *provider) override;
};

OSDefineMetaClassAndStructors(com_jackandroid_JackAndroid, IOService)

bool com_jackandroid_JackAndroid::start(IOService *provider)
{
    if (!IOService::start(provider))
        return false;

    JK_PR("============================================");
    JK_PR(" JackAndroid - by digson-kirky-CE");
    JK_PR(" JackiOS KEXT loaded on XNU kernel");
    JK_PR(" Level1 (iOS) — .Mac service will start via launchd");
    JK_PR("============================================");

    /* 注：KEXT 不能直接 fork/exec，.Mac 守护进程由 launchd plist 拉起 */
    /* 此处可注册 IOUserClient 供用户态与内核通信（可选扩展）*/

    registerService();
    return true;
}

void com_jackandroid_JackAndroid::stop(IOService *provider)
{
    JK_PR("JackAndroid KEXT unloading.");
    IOService::stop(provider);
}

/* ---- kmod 入口（被 kextload 调用）---- */
extern "C" kern_return_t JackAndroid_start(kmod_info_t *, void *)
{
    JK_PR("JackAndroid KEXT init");
    if (!com_jackandroid_JackAndroid::superClass::checkPassive())
        return KERN_FAILURE;
    return KERN_SUCCESS;
}

extern "C" kern_return_t JackAndroid_stop(kmod_info_t *, void *)
{
    JK_PR("JackAndroid KEXT fini");
    return KERN_SUCCESS;
}
