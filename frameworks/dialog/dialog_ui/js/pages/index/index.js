import router from '@ohos.router'

export default {
    data: {
        message: router.getParams().message,
        cancelButton: router.getParams().cancelButton,
    },
    onInit() {
        console.info('getParams: ' + router.getParams());
    },
    onCancel() {
        console.info('click cancel');
        callNativeHandler("EVENT_CANCEL", "cancel");
    }
}
