package cn.zzsst.client;

public abstract class StaticFileDictLookupCallback extends StaticFileCallback {

    protected String key;
    protected Callback callback;

    public StaticFileDictLookupCallback(String key, Callback callback) {
        this.key = key;
        this.callback = callback;
    }

}
