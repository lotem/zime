package cn.zzsst.client.engine;

import cn.zzsst.client.ZimeDict;

public class LuomazySchema {

	public ZimeDict getDict() {
		// TODO
		return new DummyDict();
	}
	
	public boolean isInput(int c) {
		return c >= 'A' && c <= 'Z';
	}

	public boolean isSelection(int c) {
		return c >= '0' && c <= '9';
	}

}
