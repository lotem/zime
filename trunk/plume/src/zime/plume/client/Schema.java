package zime.plume.client;

import java.util.List;

public class Schema {

    private String name;
    private String displayName;
    transient private Config config;
    
    public Schema(String name, String displayName) {
        this.name = name;
        this.displayName = displayName;
        config = null;
    }

    public String getName() {
        return name;
    }
    
    public String getDisplayName() {
        return displayName;
    }

    public String getParserName() {
        String result = config.getValue("Parser");
        return result != null ? result : "roman";
    }
    
    public void setConfig(Config config) {
        this.config = config;
    }

    public String getConfigValue(String key) {
        return config.getValue(key);
    }

    public List<String> getConfigList(String key) {
        return config.getList(key);
    }

    public String getConfigCharSequence(String key) {
        return config.getCharSequence(key);
    }

}
