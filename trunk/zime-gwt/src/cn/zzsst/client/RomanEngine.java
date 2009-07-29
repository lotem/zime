package cn.zzsst.client;

import java.util.ArrayList;


import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public class RomanEngine extends ZimeEngine {
	
    private static final int KEYCODE_APOSTROPHE = 222;

	public RomanEngine(ZimeModule module, Schema schema) {
		super(module, schema);
	}

    public boolean isInput(int c) {
        return c >= 'A' && c <= 'Z' || c == KEYCODE_APOSTROPHE;
    }

    public boolean isSelection(int c) {
        return c >= '0' && c <= '9';
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
			if (isInput(c))
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
			candidateList.pageUp();
			break;
		case KeyCodes.KEY_PAGEDOWN:
		    candidateList.pageDown();
			break;
		case KeyCodes.KEY_ESCAPE:
			clear();
			break;
		case KeyCodes.KEY_ENTER:
			if (context.isEmpty()) {
			    module.submit();
			} else {
			    module.commitString(context.getPreedit());
			}
			clear();
			break;
		case KeyCodes.KEY_BACKSPACE:
		    updateContext(module.getPreedit());
			updateUI();
			break;
		default:
			if (isInput(c)) {
			    updateContext(module.getPreedit());
				updateUI();
				break;
			}
			int selection = 0;
			if (isSelection(c))
				selection = c - '1';
			String s = candidateList.getSelectedCandidate(selection);
			if (s != null) {
				module.commitString(s);
				updateContext(context.rest());
				updateUI();
			}
		}

		return true;
	}

	protected void clear() {
		updateContext("");
		updateUI();
	}

	protected void updateUI() {
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
        candidateList.clear();
        if (result != null)
            candidateList.addCandidates(result);
        module.setPreedit(context.getPreedit());
        module.updateCandidates(candidateList);
	}

    protected void updateContext(String preedit) {
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
            context.addWordRange(start, end);
            start = end;
            if (start < preedit.length() && preedit.charAt(start) == '\'') {
                ++start;
            }
        }
    }

}
