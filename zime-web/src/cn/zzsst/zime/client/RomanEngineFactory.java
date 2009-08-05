package cn.zzsst.zime.client;

public class RomanEngineFactory implements EngineFactory {
    
    public static final String CODE_NAME_ROMAN = "roman";

    @Override
    public Engine create(Module module, Schema schema) {
        return new RomanEngine(module, schema);
    }

    @Override
    public String getCodeName() {
        return CODE_NAME_ROMAN;
    }

}
