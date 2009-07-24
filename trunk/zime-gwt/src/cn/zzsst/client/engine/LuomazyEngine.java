package cn.zzsst.client.engine;

import java.util.ArrayList;

import cn.zzsst.client.Context;
import cn.zzsst.client.ZimeEngine;
import cn.zzsst.client.ZimeModule;

import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public class LuomazyEngine extends ZimeEngine {
	
	private static final String ENGINE_NAME = "中州羅馬字";

	protected LuomazySchema schema;

    protected Context context;

	public LuomazyEngine(ZimeModule module) {
		super(module);
		schema = new LuomazySchema();
		dict = schema.getDict();
        context = new Context();
	}

	@Override
	public String getName() {
		return ENGINE_NAME;
	}

	@Override
	public boolean processKeyDownEvent(KeyDownEvent event) {
		int c = event.getNativeKeyCode();
		switch (c) {
		case KeyCodes.KEY_TAB:
		    return false;
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
        case KeyCodes.KEY_TAB:
            return false;
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
			module.commitString(context.getPreedit());
			clear();
			break;
		case KeyCodes.KEY_BACKSPACE:
		    updatePreedit(module.getPreedit());
			update();
			break;
		default:
			if (schema.isInput(c)) {
			    updatePreedit(module.getPreedit());
				update();
				break;
			}
			int selection = 0;
			if (schema.isSelection(c))
				selection = c - '1';
			String s = module.getCandidateList().getSelectedCandidate(selection);
			if (s != null) {
				module.commitString(s);
				updatePreedit(context.rest());
				update();
			}
		}

		return true;
	}

	protected void clear() {
		updatePreedit("");
		update();
	}

	protected void update() {
	    int len = context.getLength();
	    context.setCandidateLength(len);
        ArrayList<String> result = null;
        while (len > 0) {
	        result = dict.lookup(context);
	        if (result != null)
	            break;
	        --len;
	        context.setCandidateLength(len);
	    }
	    
        module.setPreedit(context.getPreedit());
		module.updateCandidates(result);
	}

    protected void updatePreedit(String preedit) {
        context.clear();
        context.setPreedit(preedit);
        
        int start = 0, end;
        String result = null;
        while (start < preedit.length()) {
            result = dict.parse(preedit.substring(start));
            if (result == null) {
                end = preedit.length();
            } else {
                end = start + result.length();
            }
            context.add(start, end);
            start = end;
            if (start < preedit.length() && preedit.charAt(start) == '\'') {
                ++start;
            }
        }
    }

}
