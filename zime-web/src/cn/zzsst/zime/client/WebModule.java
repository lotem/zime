package cn.zzsst.zime.client;

import java.util.ArrayList;

import com.google.gwt.event.dom.client.ChangeEvent;
import com.google.gwt.event.dom.client.ChangeHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyDownHandler;
import com.google.gwt.event.dom.client.KeyUpEvent;
import com.google.gwt.event.dom.client.KeyUpHandler;
import com.google.gwt.user.client.Window;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextArea;
import com.google.gwt.user.client.ui.VerticalPanel;

public class WebModule implements Module {
    private Engine engine;

    private VerticalPanel panel;
    private PreeditBox preeditBox;
    private TextArea editor;
    private ListBox schemaChooser;
    
    public WebModule() {
        panel = new VerticalPanel();
        panel.setWidth("100%");
        preeditBox = new PreeditBox();
        preeditBox.setWidth("100%");
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

        final Module module = this;
        schemaChooser = new ListBox();
        schemaChooser.addChangeHandler(new ChangeHandler() {
            @Override
            public void onChange(ChangeEvent event) {
                int index = schemaChooser.getSelectedIndex();
                if (index != -1) {
                    preeditBox.setEnabled(false);
                    editor.setEnabled(false);
                    Schema.load(schemaChooser.getValue(index), module);
                }
                
            }
        });
        
        preeditBox.setEnabled(false);
        editor.setEnabled(false);
        
        panel.add(schemaChooser);
        panel.add(preeditBox);
        panel.add(editor);
        RootPanel.get().add(panel);
    }

    @Override
    public void setSchema(Schema schema) {
        engine = Registry.createEngine(this, schema);
        editor.setEnabled(true);
        preeditBox.setEnabled(true);
        preeditBox.setFocus(true);
    }
    
    @Override
    public void setSchemaList(ArrayList<String[]> schemas) {
        schemaChooser.clear();
        for (String[] x : schemas) {
            schemaChooser.addItem(x[1], x[0]);
        }
        int index = (schemas.size() > 0) ? 0 : -1;
        String param = Window.Location.getParameter("schema");
        for (int i = 0; i < schemas.size(); ++i) {
            if (schemas.get(i)[0].equals(param)) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            schemaChooser.setSelectedIndex(index);
            Schema.load(schemas.get(index)[0], this);
        }
    }

    @Override
    public void commitString(String str) {
        String t = editor.getText();
        int cursorPos = editor.getCursorPos();
        int selLength = editor.getSelectionLength();
        String result = t.substring(0, cursorPos) + str + 
                        t.substring(cursorPos + selLength);
        editor.setText(result);
        editor.setCursorPos(cursorPos + str.length());
    }

    @Override
    public String getPreedit() {
        return preeditBox.getText();
    }

    @Override
    public void hideCandidates() {
        preeditBox.hideMenu();
    }

    @Override
    public void setPreedit(String str) {
        preeditBox.setText(str);
    }

    @Override
    public void showCandidates() {
        preeditBox.showMenu();
    }

    @Override
    public void submit() {
        editor.selectAll();
        editor.setFocus(true);
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

}
