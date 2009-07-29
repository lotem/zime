package cn.zzsst.client;


public class Schema {
    
    private static final String CODE_NAME_DEFAULT = "luomazy";
    
    private String codeName;
    
    public Schema(String codeName) {
        this.codeName = codeName;
    }

    public String getName() {
        return "ZIME";
    }

    public Dict getDict() {
		return new DummyDict(codeName);
	}

    public ZimeEngine createEngine(ZimeModule module) {
        return new RomanEngine(module, this);
    }

    public static Schema create(String codeName) {
        if (codeName == null || "".equals(codeName))
            codeName = CODE_NAME_DEFAULT;
        return new Schema(codeName);
    }
	
}
