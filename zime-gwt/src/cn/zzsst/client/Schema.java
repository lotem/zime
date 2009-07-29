package cn.zzsst.client;


public class Schema {
    
    private String codeName;
    public Schema(String codeName) {
        this.codeName = codeName;
    }

    public String getName() {
        return "中州羅馬字";
    }

    public Dict getDict() {
		return new DummyDict(codeName);
	}

    public ZimeEngine createEngine(ZimeModule module) {
        return new RomanEngine(module, this);
    }
	
}
