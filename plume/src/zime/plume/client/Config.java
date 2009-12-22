package zime.plume.client;

import java.util.List;

public class Config {

    private String prefix;
    private List<String[]> settings;

    public Config(String prefix, String text) {
        this.prefix = prefix;
        // TODO
        this.settings = null;
    }

    public String getValue(String key) {
        return readSetting(prefix + key);
    }
    
    public String getCharSequence(String key) {
        String s = readSetting(prefix + key);
        if (s != null && s.startsWith("[") && s.endsWith("]")) {
            return s.substring(1, s.length() - 1);
        }
        return s;
    }

    public List<String> getList(String key) {
        return readSettingList(prefix + key);
    }
    
    public List<String[]> getItems(String dir) {
        // TODO Auto-generated method stub
        return null;
    }

    private String readSetting(String key) {
        // TODO Auto-generated method stub
        return null;
    }

    private List<String> readSettingList(String key) {
        // TODO Auto-generated method stub
        return null;
    }

}
