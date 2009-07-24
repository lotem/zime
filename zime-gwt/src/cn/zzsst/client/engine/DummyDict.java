package cn.zzsst.client.engine;

import java.util.ArrayList;
import java.util.HashMap;

import cn.zzsst.client.Context;
import cn.zzsst.client.ZimeDict;

public class DummyDict implements ZimeDict {

	static protected HashMap<String, String> map;
	
	static {
		map = new HashMap<String, String>();
		map.put("zheo", "這");
		map.put("na", "那");
		map.put("bu", "不");
		map.put("shy", "是");
		map.put("di", "的");
		map.put("le", "的");
		map.put("ni", "你");
		map.put("wo", "我");
		map.put("ta", "他");
		map.put("shang", "上");
		map.put("zhung", "中");
		map.put("xa", "下");
		map.put("cian", "前");
		map.put("hou", "後");
		map.put("zuo", "左");
		map.put("you", "右");
	}
	
	@Override
	public ArrayList<String> lookup(Context context) {
	    if (context.getCandidateLength() != 1)
	        return null;
		String s = map.get(context.getWord(0));
		if (s == null)
			return null;
		ArrayList<String> list = new ArrayList<String>();
		list.add(s);
		return list;
	}

    public String parse(String preedit) {
        for (int i = preedit.length(); i > 0; --i) {
            final String s = preedit.substring(0, i);
            if (map.containsKey(s))
                return s;
        }
        return null;
    }
    
}
