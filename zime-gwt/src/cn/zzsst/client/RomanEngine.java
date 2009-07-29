package cn.zzsst.client;

import java.util.ArrayList;


import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public class RomanEngine extends ZimeEngine {
	
	public RomanEngine(ZimeModule module, Schema schema) {
		super(module, schema);
	}

	@Override
	public boolean processKeyDownEvent(KeyDownEvent event) {
        int c = event.getNativeKeyCode();
        switch (c) {
        case KeyCodes.KEY_ESCAPE:
            return true;
        case KeyCodes.KEY_ENTER:
            return true;
        default:
            if (isPageUpKey(c)) {
                return true;
            }
            else if (isPageDownKey(c)) {
                return true;
            } 
            else if (isSelectionKey(c)) {
                return true;
            }
        }

		return false;
	}

	@Override
	public boolean processKeyUpEvent(KeyUpEvent event) {
		int c = event.getNativeKeyCode();
		switch (c) {
        case KeyCodes.KEY_TAB:
            return false;
		case KeyCodes.KEY_ESCAPE:
			clear();
			break;
		case KeyCodes.KEY_ENTER:
			if (context.isEmpty()) {
			    module.submit();
			} 
			else {
			    module.commitString(context.getPreedit());
			}
			clear();
			break;
		case KeyCodes.KEY_BACKSPACE:
		    updateContext(module.getPreedit());
			updateUI();
			break;
		default:
            if (isPageUpKey(c)) {
                if (candidateList.getCurrentPage() > 0) {
                    candidateList.pageUp();
                }
                else if (!context.getCache().isEmpty()) {
                    ArrayList<String> candidates = context.fetchCache(1);
                    candidateList.setCandidates(candidates);
                }
                module.updateCandidates(candidateList);
		    }
		    else if (isPageDownKey(c)) {
		        if (candidateList.getCurrentPage() < candidateList.getLastPage()) {
		            candidateList.pageDown();
		        }
		        else {
                    if (context.getCache().isEmpty()) {
                        cacheCandidates();
                    }
                    ArrayList<String> candidates = context.fetchCache(-1);
                    candidateList.setCandidates(candidates);
		        }
		        module.updateCandidates(candidateList);
		    } 
		    else if (isSelectionKey(c)) {
    			int selection = 0;
    			if (c != KEYCODE_SPACE)
    				selection = c - '1';
    			String s = candidateList.getSelectedCandidate(selection);
    			if (s != null) {
    				module.commitString(s);
    				updateContext(context.rest());
    				updateUI();
    			}
		    }
            else {
                updateContext(module.getPreedit());
                updateUI();
            }
		}

		return true;
	}

	private void cacheCandidates() {
        ArrayList<ArrayList<String>> cache = context.getCache();
        for (int i = 0; i < 4; ++i)
            cache.add(null);
        int candidateLength = context.getCandidateLength();
        int len = candidateLength - 1;
        cache.set(len, candidateList.getCandidates());
        ArrayList<String> result = null;
        while (len > 0) {
            result = findCandidates(len);
            if (result == null)
                break;
            --len;
            cache.set(len, result);
        }
        context.setCandidateLength(candidateLength);
    }

    private ArrayList<String> findCandidates(int len) {
        ArrayList<String> result = null;
        while (len > 0) {
            context.setCandidateLength(len);
            result = dict.lookup(context);
            if (result != null)
                break;
            --len;
        }
        if (result == null)
            context.setCandidateLength(0);
        return result;
    }

    protected void clear() {
		updateContext("");
		updateUI();
	}

	protected void updateUI() {
	    int len = context.getLength();
	    candidateList.setCandidates(findCandidates(len));
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

    private boolean isPageUpKey(int c) {
        return c == KeyCodes.KEY_PAGEUP || 
               c == KEYCODE_COMMA && !candidateList.isEmpty();
    }

    private boolean isPageDownKey(int c) {
        return c == KeyCodes.KEY_PAGEDOWN || 
               c == KEYCODE_PERIOD && !candidateList.isEmpty();
    }

    private boolean isSelectionKey(int c) {
        return (c >= '0' && c <= '9' || c == KEYCODE_SPACE) && 
               !candidateList.isEmpty();
    }

}
