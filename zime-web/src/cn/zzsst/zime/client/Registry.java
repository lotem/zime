package cn.zzsst.zime.client;

import java.util.ArrayList;
import java.util.HashMap;

public class Registry {

    private static ArrayList<String[]> schemas;
    private static DictFactory dictFactory;
    private static HashMap<String, EngineFactory> engineFactories;
    
    static {
        schemas = new ArrayList<String[]>();
        dictFactory = null;
        engineFactories = new HashMap<String, EngineFactory>();
    }

    public static void load(final Module module) {
        new StaticFileCallback() {
            @Override
            public void onFailure(Throwable caught) {
                Logger.log("missing registry file.");
            }
            @Override
            public void onSuccess(String text) {
                try {
                    String[] lines = text.split("\\n+");
                    for (String x : lines) {
                        schemas.add(x.split("\\s+"));
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }
                module.setSchemaList(schemas);
            }
        }.fetch(StaticFileCallback.PREFIX + "registry.txt");
    }

    public static Dict createDict(String codeName) {
        return dictFactory.create(codeName);
    }

    public static Engine createEngine(Module module, Schema schema) {
        Logger.log("createEngine: " + schema.getEngineName());
        EngineFactory factory = engineFactories.get(schema.getEngineName());
        if (factory == null) {
            Logger.log("unkown engine: " + schema.getEngineName());
            return null;
        }
        return factory.create(module, schema);
    }

    public static void register(EngineFactory factory) {
        Logger.log("register: " + factory.getCodeName());
        engineFactories.put(factory.getCodeName(), factory);
    }

    public static void register(DictFactory factory) {
        dictFactory = factory;
    }

}
