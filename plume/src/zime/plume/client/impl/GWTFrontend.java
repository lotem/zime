package zime.plume.client.impl;

import zime.plume.client.CandidateList;
import zime.plume.client.Frontend;
import zime.plume.client.util.Logger;

import com.google.gwt.core.client.JavaScriptObject;
import com.google.gwt.core.client.JsArray;
import com.google.gwt.dom.client.NativeEvent;
import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.event.dom.client.ChangeHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyDownHandler;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.event.dom.client.KeyUpHandler;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.VerticalPanel;

public class GWTFrontend implements Frontend {
    
    private VerticalPanel panel;
    private PreeditBox preeditBox;
    private TextArea editor;
    private ListBox schemaChooser;
    private boolean nastyBrowser;

    public GWTFrontend() {
        
        nastyBrowser = true;
        
        panel = new VerticalPanel();
        panel.setWidth("100%");
        preeditBox = new PreeditBox();
        preeditBox.setWidth("100%");
        preeditBox.addKeyDownHandler(new KeyDownHandler() {
            @Override
            public void onKeyDown(KeyDownEvent event) {
                if (nativeProcessKeyEvent(event.getNativeEvent()))
                    preeditBox.cancelKey();
            }
        });
        preeditBox.addKeyUpHandler(new KeyUpHandler() {
            @Override
            public void onKeyUp(KeyUpEvent event) {
                if (nativeProcessKeyEvent(event.getNativeEvent()))
                    preeditBox.cancelKey();
            }
        });
        editor = new TextArea();
        editor.setVisibleLines(7);
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

        schemaChooser = new ListBox();
        schemaChooser.addChangeHandler(new ChangeHandler() {
            @Override
            public void onChange(ChangeEvent event) {
                int index = schemaChooser.getSelectedIndex();
                if (index != -1) {
                    preeditBox.setEnabled(false);
                    editor.setEnabled(false);
                    String schemaName = schemaChooser.getValue(index);
                    nativeLoadSchema(schemaName);
                }
            }
        });
        
        preeditBox.setEnabled(false);
        editor.setEnabled(false);
        
        panel.add(schemaChooser);
        panel.add(preeditBox);
        panel.add(editor);
        RootPanel.get().add(panel);
        
        initialize();
    }

	@Override
   public void commit(String str) {
        String t = editor.getText();
        int cursorPos = editor.getCursorPos();
        int selLength = editor.getSelectionLength();
        if (cursorPos != 0) {
            nastyBrowser = false;
        }
        if (nastyBrowser) {
            // temporary work-around for IE
            editor.setText(t + str);
            editor.setCursorPos(t.length() + str.length());
        }
        else {
            String result = t.substring(0, cursorPos) + str + 
                            t.substring(cursorPos + selLength);
            editor.setText(result);
            cursorPos += str.length();
            editor.setCursorPos(cursorPos);
        }
        // to work around Google Chrome focus moving issue
        preeditBox.setFocus(true);
    }

    @Override
    public void updatePreedit(String preedit, int selStart, int selEnd) {
        Logger.debug("GWTFrontend.updatePreedit: [" + preedit + "] " + selStart + ", " + selEnd);
        preeditBox.setText(preedit);
        if (selStart < selEnd) {
            preeditBox.setSelectionRange(selStart, selEnd - selStart);
        }
    }

    @Override
    public void hideCandidates() {
        preeditBox.hideCandidates();
    }

    @Override
    public void showCandidates() {
        preeditBox.showCandidates();
    }

    @Override
    public void updateCandidates(CandidateList candidateList) {
        preeditBox.updateCandidates(candidateList);
        if (candidateList.isEmpty()) {
            preeditBox.hideCandidates();
        } else {
            preeditBox.showCandidates();
        }
    }

    @Override
    public void submit() {
        editor.selectAll();
        editor.setFocus(true);
    }

    @Override
    public void setSelectionRange(int start, int length) {
        if (length == 0)
            start = preeditBox.getText().length();
        preeditBox.setSelectionRange(start, length);
    }

	protected native void initialize() /*-{
		var frontend = Frontend.create();
	}-*/;

	protected native void nativeLoadSchema(String schemaName) /*-{
		frontend.loadSchema(schemaName);
	}-*/;

	protected native boolean nativeProcessKeyEvent(NativeEvent nativeEvent) /*-{
		// TODO Auto-generated method stub
		return false;
	}-*/;

    public void onSchemaReady() {
        editor.setEnabled(true);
        preeditBox.setEnabled(true);
        preeditBox.setFocus(true);
	}

	public void onSchemaListReady(JsArray<SchemaListItem> array) {
		if (array == null)
			return;
		for (int i = 0; i < array.length(); i++) {
			SchemaListItem item = array.get(i);
			schemaChooser.addItem(item.getDisplayName(), item.getSchemaName());
		}
	}

	static class SchemaListItem extends JavaScriptObject {
		protected SchemaListItem() {}
		public final native String getDisplayName() /*-{ return this.displayName; }-*/;
		public final native String getSchemaName() /*-{ return this.schema; }-*/;
	}

}
