package cn.zzsst.client;

import java.util.ArrayList;


import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public class RomanEngine extends ZimeEngine {
	
	private Callback callback = new Callback() {
	    public void onReady(ArrayList<String> result) {
	        System.err.println("RomanEngine.callback.onReady()");
	        System.err.println("    " + result);
	        candidateList.setCandidates(result);
	        module.updateCandidates(candidateList);
	    }
	};

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
			update(module.getPreedit());
			break;
		default:
            if (isPageUpKey(c)) {
                if (candidateList.getCurrentPage() > 0) {
                    candidateList.pageUp();
                }
                else {
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
    				String p = context.rest();
    				System.err.println("    rest: " + p);
                    module.setPreedit(p);
    				update(p);
    			}
		    }
            else {
                update(module.getPreedit());
            }
		}

		return true;
	}

   protected void clear() {
        module.setPreedit("");
		context.clear();
        candidateList.setCandidates(null);
        module.updateCandidates(candidateList);
	}

	protected void update(String p) {
	    System.err.println("RomanEngine.update(): " + p);
	    context.update(p, callback);
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
