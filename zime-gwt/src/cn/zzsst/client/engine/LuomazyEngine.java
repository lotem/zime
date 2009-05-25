package cn.zzsst.client.engine;

import java.util.ArrayList;

import cn.zzsst.client.ZimeEngine;
import cn.zzsst.client.ZimeModule;

import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public class LuomazyEngine extends ZimeEngine {
	
	private static final String ENGINE_NAME = "中州羅馬字";

	protected LuomazySchema schema;

	protected String preedit;

	public LuomazyEngine(ZimeModule module) {
		super(module);
		schema = new LuomazySchema();
		dict = schema.getDict();
		preedit = "";
	}

	@Override
	public String getName() {
		return ENGINE_NAME;
	}

	@Override
	public boolean processKeyDownEvent(KeyDownEvent event) {
		int c = event.getNativeKeyCode();
		switch (c) {
		case KeyCodes.KEY_BACKSPACE:
			return false;
		default:
			if (schema.isInput(c))
				return false;
		}
		return true;
	}

	@Override
	public boolean processKeyUpEvent(KeyUpEvent event) {
		int c = event.getNativeKeyCode();
		switch (c) {
		case KeyCodes.KEY_PAGEUP:
			module.pageUp();
			break;
		case KeyCodes.KEY_PAGEDOWN:
			module.pageDown();
			break;
		case KeyCodes.KEY_ESCAPE:
			clear();
			break;
		case KeyCodes.KEY_ENTER:
			module.commitString(preedit);
			clear();
			break;
		case KeyCodes.KEY_BACKSPACE:
			preedit = module.getPreedit();
			updatePreedit();
			updateCandidates();
			break;
		default:
			if (schema.isInput(c)) {
				preedit = module.getPreedit();
				updatePreedit();
				updateCandidates();
				break;
			}
			int selection = 0;
			if (schema.isSelection(c))
				selection = c - '1';
			String s = module.getCandidateList().getSelectedCandidate(selection);
			if (s != null) {
				module.commitString(s);
				clear();
			}
		}

		return true;
	}

	protected void clear() {
		preedit = "";
		updatePreedit();
		updateCandidates();
	}

	protected void updatePreedit() {
		module.setPreedit(preedit);
	}

	protected void updateCandidates() {
		ArrayList<String> list = dict.lookup(preedit);
		module.updateCandidates(list);
	}

}
