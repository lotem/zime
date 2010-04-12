package cn.zzsst.zime.client;

import java.util.ArrayList;
import java.util.HashMap;


public class Schema {
    
    private HashMap<String, String> values;
    private HashMap<String, ArrayList<String>> lists;
    
    private static final String DIRECTIVE_SIGIL = "%";

    private static final String DIRECTIVE_SCHEMA = "schema";

    private static final String LIST_BEGIN = "begin";

    private static final String LIST_END = "end";

    private String codeName;

    private Dict dict;

    public static void load(final String codeName, final Module module) {
        Logger.log("Schema.load: " + codeName);
        new StaticFileCallback() {
            @Override
            public void onFailure(Throwable caught) {
                Logger.log("missing schema file.");
            }
            @Override
            public void onSuccess(String text) {
                module.setSchema(Schema.create(text));
            }
        }.fetch(StaticFileCallback.PREFIX + codeName + "/schema.txt");
     }

    private static Schema create(String text) {
        Schema schema = null;
        ArrayList<String> list = null;
        try {
            String[] lines = text.split("\\n+");
            for (String x : lines) {
                if (x.startsWith(DIRECTIVE_SIGIL)) {
                    //Logger.log("processing directive: " + x);
                    String[] a = x.substring(DIRECTIVE_SIGIL.length()).split("\\s+", 2);
                    if (a.length < 2)
                        continue;
                    if (DIRECTIVE_SCHEMA.equals(a[0])) {
                        Logger.log("creating schema: " + a[1]);
                        schema = new Schema(a[1]);
                    }
                    else if (LIST_BEGIN.equals(a[1])) {
                        String key = a[0];
                        list = new ArrayList<String>();
                        schema.lists.put(key, list);
                    }
                    else if (LIST_END.equals(a[1])) {
                        list = null;
                    }
                    else {
                        //Logger.log("config value: " + a[0] + " = " + a[1]);
                        schema.values.put(a[0], a[1]);
                    }
                }
                else { // non-directive
                    list.add(x);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return schema;
    }

    public Schema(String codeName) {
        this.codeName = codeName;
        dict = Registry.createDict(codeName);
        values = new HashMap<String, String>();
        lists = new HashMap<String, ArrayList<String>>();
    }

    public String getCodeName() {
        return codeName;
    }

    public Dict getDict() {
        return dict;
    }
    
    private String getValue(String key) {
        return values.containsKey(key) ? values.get(key) : null;
    }

    private  ArrayList<String> getList(String key) {
        return lists.containsKey(key) ? lists.get(key) : null;
    }

    public String getName() {
        return getValue("name");
    }

    public Object getEngineName() {
        return getValue("engine");
    }

    public int getMaxWordLength() {
        return Integer.parseInt(values.get("max-word-length"));
    }

    public int getMaxKeywordLength() {
        return Integer.parseInt(values.get("max-keyword-length"));
    }

    public int getMaxQueryKeywords() {
        return Integer.parseInt(values.get("max-query-keywords"));
    }

    public String getDelimiters() {
        return getValue("delimiters");
    }
    
    public ArrayList<String> getSplitRules() {
        return getList("split-rules");
    }

}
