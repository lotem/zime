package cn.zzsst.zime.client;

public interface Dict {
    void lookup(String key, LookupCallback callback);
    boolean exist(String key);
}
