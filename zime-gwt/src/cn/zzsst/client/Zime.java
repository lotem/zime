package cn.zzsst.client;

import java.util.ArrayList;

import cn.zzsst.client.engine.RomanEngine;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyDownHandler;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.event.dom.client.KeyUpHandler;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.VerticalPanel;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class Zime implements EntryPoint, ZimeModule {
	
    private VerticalPanel panel;
    private Label engineName;
    private PreeditBox preeditBox;
    private TextArea editor;
    
	private CandidateList candidateList;
    private ZimeEngine engine;
    
	/**
     * This is the entry point method.
     */
    public void onModuleLoad() {
        panel = new VerticalPanel();
        panel.setWidth("100%");
        engineName = new Label("ZIME");
        preeditBox = new PreeditBox();
        preeditBox.addKeyDownHandler(new KeyDownHandler() {

            @Override
            public void onKeyDown(KeyDownEvent event) {
                if (engine.processKeyDownEvent(event))
                    preeditBox.cancelKey();
            }
            
        });
        preeditBox.addKeyUpHandler(new KeyUpHandler() {

            @Override
            public void onKeyUp(KeyUpEvent event) {
                if (engine.processKeyUpEvent(event))
                    preeditBox.cancelKey();
            }
            
        });
        editor = new TextArea();
        editor.setVisibleLines(5);
        editor.setWidth("100%");
        editor.addKeyDownHandler(new KeyDownHandler() {

            @Override
            public void onKeyDown(KeyDownEvent event) {
                if (event.getNativeKeyCode() == KeyCodes.KEY_TAB) {
                    preeditBox.setFocus(true);
                    editor.cancelKey();
                }
            }
            
        });

        panel.add(engineName);
        panel.add(preeditBox);
        panel.add(editor);
        RootPanel.get().add(panel);
        
        preeditBox.setFocus(true);
        
        candidateList = new CandidateList();
    	setEngine(new RomanEngine(this));
    }

	public void setEngine(ZimeEngine engine) {
		this.engine = engine;
    	engineName.setText(engine.getName()); 
	}

	public ZimeEngine getEngine() {
		return engine;
	}

	@Override
	public CandidateList getCandidateList() {
		return this.candidateList;
	}

	@Override
	public String getPreedit() {
		return preeditBox.getText();
	}

	@Override
	public void setPreedit(String s) {
		preeditBox.setText(s);
	}

	@Override
	public void commitString(String s) {
		String t = editor.getText();
		int cursorPos = editor.getCursorPos();
		int selLength = editor.getSelectionLength();
		String result = t.substring(0, cursorPos) + s + 
		                t.substring(cursorPos + selLength);
		editor.setText(result);
		editor.setCursorPos(cursorPos + s.length());
	}

	@Override
	public void pageDown() {
		int page = candidateList.getCurrentPage() + 1;
		int start = page * candidateList.getPageSize();
		if (start < candidateList.getCandidates().size())
			candidateList.setCurrentPage(page);
		preeditBox.updateMenuItems(candidateList);
	}

	@Override
	public void pageUp() {
		int page = candidateList.getCurrentPage() - 1;
		if (page >= 0)
			candidateList.setCurrentPage(page);
		preeditBox.updateMenuItems(candidateList);
	}

	@Override
	public void showCandidates() {
		preeditBox.showMenu();
	}
	
	@Override
	public void hideCandidates() {
		preeditBox.hideMenu();
	}
	
	@Override
	public void updateCandidates(ArrayList<String> list) {
		if (list == null || list.size() == 0) {
			candidateList.getCandidates().clear();
			hideCandidates();
		}
		else {
			candidateList.setCandidates(list);
			preeditBox.updateMenuItems(candidateList);
			showCandidates();
		}
	}

    @Override
    public void submit() {
        editor.selectAll();
        editor.setFocus(true);
    }

}
