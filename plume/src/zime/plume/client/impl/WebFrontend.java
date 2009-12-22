package zime.plume.client.impl;

import java.util.ArrayList;
import java.util.List;

import zime.plume.client.Backend;
import zime.plume.client.CandidateList;
import zime.plume.client.Config;
import zime.plume.client.Engine;
import zime.plume.client.Frontend;
import zime.plume.client.Schema;
import zime.plume.client.util.Logger;

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

public class WebFrontend implements Frontend {
    
    private Backend backend;
    private Engine engine;
    private List<Schema> schemaList;

    private VerticalPanel panel;
    private PreeditBox preeditBox;
    private TextArea editor;
    private ListBox schemaChooser;
    private boolean nastyBrowser;

    public WebFrontend(Backend backend) {
        this.backend = backend;
        
        nastyBrowser = true;
        
        panel = new VerticalPanel();
        panel.setWidth("100%");
        preeditBox = new PreeditBox();
        preeditBox.setWidth("100%");
        preeditBox.addKeyDownHandler(new KeyDownHandler() {
            @Override
            public void onKeyDown(KeyDownEvent event) {
                if (engine.processKeyDown(event))
                    preeditBox.cancelKey();
            }
        });
        preeditBox.addKeyUpHandler(new KeyUpHandler() {
            @Override
            public void onKeyUp(KeyUpEvent event) {
                if (engine.processKeyUp(event))
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

        final WebFrontend frontend = this;
        schemaChooser = new ListBox();
        schemaChooser.addChangeHandler(new ChangeHandler() {
            @Override
            public void onChange(ChangeEvent event) {
                int index = schemaChooser.getSelectedIndex();
                if (index != -1) {
                    preeditBox.setEnabled(false);
                    editor.setEnabled(false);
                    String codeName = schemaChooser.getValue(index);
                    for (Schema s : schemaList)
                        if (s.getName().equals(codeName))
                            frontend.loadSchema(s);
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
        Logger.debug("WebFrontend.updatePreedit: [" + preedit + "] " + selStart + ", " + selEnd);
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

    public void initialize() {
        schemaChooser.clear();
        schemaList = new ArrayList<Schema>();
        backend.loadSchemaList(new Backend.LoadSchemaListCallback() {
            @Override
            public void onFailure() {
                // TODO
            }
            @Override
            public void onReady(List<Schema> list) {
                
                for (Schema schema : list) {
                    schemaChooser.addItem(schema.getDisplayName(), schema.getName());
                }
                schemaList = list;
                autoLoad(list);
            }
        });
    }

    private void autoLoad(List<Schema> list) {
        int index = (list.size() > 0) ? 0 : -1;
        String param = Window.Location.getParameter("schema");
        for (int i = 0; i < list.size(); ++i) {
            if (list.get(i).getName().equals(param)) {
                index = i;
                break;
            }
        }
        if (index != -1) {
            schemaChooser.setSelectedIndex(index);
            loadSchema(schemaList.get(index));
        }
    }
    
    public void loadSchema(final Schema schema) {
        final Frontend frontend = this;
        backend.loadConfig(schema.getName(),
            new Backend.LoadConfigCallback() {
                @Override
                public void onFailure() {
                    // TODO
                }
                @Override
                public void onReady(Config config) {
                    schema.setConfig(config);
                    engine = new Engine(frontend, backend, schema);
                    editor.setEnabled(true);
                    preeditBox.setEnabled(true);
                    preeditBox.setFocus(true);
                }
            });
    }
    
}
