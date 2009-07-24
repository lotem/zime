package cn.zzsst.client.engine;

import cn.zzsst.client.ZimeDict;

public class LuomazySchema {
    
	private static final int KEYCODE_APOSTROPHE = 222;

    public ZimeDict getDict() {
		// TODO
		return new DummyDict();
	}
	
    public boolean isInput(int c) {
        return c >= 'A' && c <= 'Z' || c == KEYCODE_APOSTROPHE;
    }

	public boolean isSelection(int c) {
		return c >= '0' && c <= '9';
	}

}
