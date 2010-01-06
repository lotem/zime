package zime.plume.client;


import com.google.gwt.dom.client.NativeEvent;
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
                if (nativeProcessKeyEvent(event.getNativeEvent())) {
                    preeditBox.cancelKey();
                }
            }
        });
        preeditBox.addKeyUpHandler(new KeyUpHandler() {
            @Override
            public void onKeyUp(KeyUpEvent event) {
                if (nativeProcessKeyEvent(event.getNativeEvent())) {
                    preeditBox.cancelKey();
                }
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
        if (cursorPos > 0) {
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
        preeditBox.setText(preedit);
        if (selStart < selEnd) {
            preeditBox.setSelectionRange(selStart, selEnd - selStart);
        }
        else {
            preeditBox.setCursorPos(preedit.length());
        }
    }

    @Override
    public void updateCandidates(CandidateList candidateList) {
        preeditBox.updateCandidates(candidateList);
        if (candidateList.length() == 0) {
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

    public void onSchemaReady() {
        editor.setEnabled(true);
        preeditBox.setEnabled(true);
        preeditBox.setFocus(true);
    }

    public void onSchemaListReady(SchemaList list) {
        if (list == null || list.length() == 0) {
            return;
        }
        // Support activating a specified schema using 'schema' parameter in URL query string
        // --zouive
        int j = 0;
        String schema = Window.Location.getParameter("schema");
        for (int i = 0; i < list.length(); i++) {
            SchemaListItem item = list.get(i);
            schemaChooser.addItem(item.getDisplayName(), item.getSchemaName());
            if(item.getSchemaName().equals(schema)){
            	j = i;
            }
        }
        schema = list.get(j).getSchemaName();
        //Window.alert(schema);
        nativeLoadSchema(schema);
        schemaChooser.setSelectedIndex(j);
        // --zouive
    }

    protected native void initialize() /*-{
        var frontend = $wnd.Frontend.create();
        var gwtFrontend = this;
        frontend.onSchemaListReady = function (schemaList) {
            gwtFrontend.@zime.plume.client.GWTFrontend::onSchemaListReady(Lzime/plume/client/SchemaList;)(schemaList);
        };
        frontend.onSchemaReady = function () {
            gwtFrontend.@zime.plume.client.GWTFrontend::onSchemaReady()();
        };
        frontend.updatePreedit = function (preedit, selStart, selEnd) {
            gwtFrontend.@zime.plume.client.GWTFrontend::updatePreedit(Ljava/lang/String;II)(preedit, selStart, selEnd);
        };
        frontend.updateCandidates = function (candidateList) {
            gwtFrontend.@zime.plume.client.GWTFrontend::updateCandidates(Lzime/plume/client/CandidateList;)(candidateList);
        };
        frontend.commit = function (text) {
            gwtFrontend.@zime.plume.client.GWTFrontend::commit(Ljava/lang/String;)(text);
        };
        frontend.submit = function () {
            gwtFrontend.@zime.plume.client.GWTFrontend::submit()();
        };
        $wnd.frontend = frontend;
    }-*/;

    protected native void nativeLoadSchema(String schemaName) /*-{
        $wnd.frontend.loadSchema(schemaName);
    }-*/;

    protected native boolean nativeProcessKeyEvent(NativeEvent event) /*-{
        return $wnd.frontend.processKeyEvent(event);
    }-*/;

}
