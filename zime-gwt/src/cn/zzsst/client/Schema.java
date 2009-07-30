package cn.zzsst.client;


public class Schema {
    
    private static final String CODE_NAME_DEFAULT = "luomazy";
    
    private String codeName;

    private Dict dict;
    
    public static Schema create(String codeName) {
        if (codeName == null || "".equals(codeName))
            codeName = CODE_NAME_DEFAULT;
        // TODO: check out schema registry
        return new Schema(codeName);
    }

    public Schema(String codeName) {
        this.codeName = codeName;
        dict = new StaticFileDict(codeName);
    }

    public String getCodeName() {
        return codeName;
    }

    public Dict getDict() {
        return dict;
    }
    
    public String getName() {
        // TODO read config
        return "ZIME";
    }

    public int getMaxWordLength() {
        // TODO read config
        return 6;
    }

    public int getMaxQueryWords() {
        // TODO read config
        return 4;
    }

    public ZimeEngine createEngine(ZimeModule module) {
        // TODO read config
        return new RomanEngine(module, this);
    }

}
