package cn.zzsst.client.engine;

import cn.zzsst.client.ZimeDict;

public class RomanSchema {
    
    public static final String ENGINE_NAME = "中州羅馬字";

	private static final int KEYCODE_APOSTROPHE = 222;

    public ZimeDict getDict() {
		return new DummyDict("luomazy");
	}
	
    public boolean isInput(int c) {
        return c >= 'A' && c <= 'Z' || c == KEYCODE_APOSTROPHE;
    }

	public boolean isSelection(int c) {
		return c >= '0' && c <= '9';
	}

}
