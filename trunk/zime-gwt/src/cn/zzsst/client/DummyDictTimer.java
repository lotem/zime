package cn.zzsst.client;

import com.google.gwt.user.client.Timer;

public abstract class DummyDictTimer extends Timer {

    protected String key;
    protected Callback callback;

    public DummyDictTimer(String key, Callback callback) {
        this.key = key;
        this.callback = callback;
    }

}
