package cn.zzsst.zime.client;

public interface EngineFactory {
    Engine create(Module module, Schema schema);
    String getCodeName();
}
