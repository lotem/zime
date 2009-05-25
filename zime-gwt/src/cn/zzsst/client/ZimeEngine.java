package cn.zzsst.client;

import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public abstract class ZimeEngine {

	protected ZimeModule module;

	protected ZimeDict dict;


	public ZimeEngine(ZimeModule module) {
		this.module = module;
	}
	
	public abstract String getName();

	public abstract boolean processKeyDownEvent(KeyDownEvent event);

	public abstract boolean processKeyUpEvent(KeyUpEvent event);
	
}
