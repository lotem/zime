package cn.zzsst.client;

public interface Dict {
    void lookup(String key, Callback callback);
    boolean exist(String key);
}
