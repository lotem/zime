package cn.zzsst.client;


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
    private Label schemaName;
    private PreeditBox preeditBox;
    private TextArea editor;
    
    private ZimeEngine engine;
    
	/**
     * This is the entry point method.
     */
    public void onModuleLoad() {
        panel = new VerticalPanel();
        panel.setWidth("100%");
        schemaName = new Label("ZIME");
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

        panel.add(schemaName);
        panel.add(preeditBox);
        panel.add(editor);
        RootPanel.get().add(panel);
        
        preeditBox.setFocus(true);
        
    	setSchema(new Schema("luomazy"));
    }

	public void setSchema(Schema schema) {
		this.engine = schema.createEngine(this);
    	schemaName.setText(schema.getName()); 
	}

	public ZimeEngine getEngine() {
		return engine;
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
	public void updateCandidates(CandidateList candidateList) {
        preeditBox.updateMenuItems(candidateList);
        if (candidateList.isEmpty()) {
            preeditBox.hideMenu();
        } else {
            preeditBox.showMenu();
        }
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
    public void submit() {
        editor.selectAll();
        editor.setFocus(true);
    }

}
