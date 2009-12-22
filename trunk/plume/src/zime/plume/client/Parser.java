package zime.plume.client;

import java.util.HashMap;

import zime.plume.client.util.Logger;

public abstract class Parser implements KeyEventHandler {

    private static HashMap<String, ParserFactory> parserFactories;
    
    static {
        parserFactories = new HashMap<String, ParserFactory> ();
    }
    
    public static void register(ParserFactory factory) {
        Logger.info("register parser: " + factory.getName());
        parserFactories.put(factory.getName(), factory);
    }

    public static Parser create(Schema schema, KeyEventHandler fallback) {
        String parserName = schema.getParserName();
        Logger.info("create parser: " + parserName);
        ParserFactory factory = parserFactories.get(parserName);
        if (factory == null) {
            Logger.error("unkown parser: " + parserName);
            return null;
        }
        return factory.create(schema, fallback);
    }

    protected Schema schema;
    protected KeyEventHandler fallback;

    public Parser(Schema schema, KeyEventHandler fallback) {
        this.schema = schema;
        this.fallback = fallback;
    }

}
