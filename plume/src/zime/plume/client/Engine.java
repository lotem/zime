package zime.plume.client;

import zime.plume.client.util.Logger;

import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public class Engine {

    private Frontend frontend;
    private Backend backend;
    private Parser parser;
    private Context context;
    
    protected KeyEventHandler fallback = new KeyEventHandler() {

        @Override
        public boolean process(KeyEvent event, Context context) {
            Logger.debug("Engine.fallback.process: " + event);

            if (event.isKeyRelease())
                return true;

            if (event.keyCode == KeyEvent.KEY_ESCAPE) {
                if (context.isErrorMode()) {
                    context.clearError();
                }
                else {
                    context.clear();
                }
                return true;
            }
            
            if (event.keyCode == KeyEvent.KEY_ENTER) {
                if (context.isConversionMode()) {
                    context.nextConversion();
                } else {
                    frontend.commit(context.getPreedit());
                    context.clear();
                }
                return true;
            }
            
            if (event.keyCode == KeyEvent.KEY_SPACE) {
                if (context.isConversionMode()) {
                    context.nextConversion();
                } else {
                    context.startConversion(backend);
                }
                return true;
            }
            
            if (event.keyCode == KeyEvent.KEY_BACKSPACE) {
                if (context.isConversionMode()) {
                    context.previousConversion();
                }
                return true;
            }
            
            return true;
        }
        
    };
    
    public Engine(Frontend frontend, Backend backend, Schema schema) {
        this.frontend = frontend;
        this.backend = backend;
        parser = Parser.create(schema, fallback);
        context = new Context(schema, this);
        onContextUpdate();
    }

    public void onContextUpdate() {
        frontend.updatePreedit(context.getPreedit(), context.getStartPos(), context.getEndPos());
        frontend.updateCandidates(context.getCandidateList());
    }
    
    public boolean processKeyDown(KeyDownEvent event) {
        if (event.isAltKeyDown() || event.isControlKeyDown() || event.isMetaKeyDown())
            return false;
        return parser.process(new KeyEvent(event), context);
    }

    public boolean processKeyUp(KeyUpEvent event) {
        if (event.isAltKeyDown() || event.isControlKeyDown() || event.isMetaKeyDown())
            return false;
        return parser.process(new KeyEvent(event), context);
    }

}
