package zime.plume.client.impl;

import zime.plume.client.Context;
import zime.plume.client.KeyEvent;
import zime.plume.client.KeyEventHandler;
import zime.plume.client.Parser;
import zime.plume.client.Schema;
import zime.plume.client.util.Logger;

public class RomanParser extends Parser {

    private String alphabet;
    private String delimiter;

    public RomanParser(Schema schema, KeyEventHandler fallback) {
        super (schema, fallback);
        alphabet = schema.getConfigValue("Alphabet");
        if (alphabet == null)
            alphabet = "abcdefghijklmnopqrstuvwxyz";
        delimiter = schema.getConfigValue("Delimiter");
        if (delimiter == null)
            delimiter = " \'-";
    }

    @Override
    public boolean process(KeyEvent event, Context context) {
        //Logger.debug("RomanParser.process: " + event);
        
        // ignore shortcuts
        if ((event.mask & (KeyEvent.CONTROL_MASK | KeyEvent.ALT_MASK | KeyEvent.META_MASK)) != 0)
            return false;

        if (event.isKeyRelease())
            return true;

        if (event.keyCode == KeyEvent.KEY_BACKSPACE) {
            if (context.isEmpty() || !context.isEditMode()) {
                return fallback.process(event, context); 
            }
            context.truncateInput(context.getInputLength() - 1);
            return true;
        }
        
        if (event.keyCode == KeyEvent.KEY_SPACE) {
            return fallback.process(event, context);
        }
        
        char ch = event.getChar();
        if (isAlphabet(ch) || !context.isEmpty() && isDelimiter(ch)) {
            context.appendInput(String.valueOf(ch));
            return true;
        }
        
        return fallback.process(event, context);
    }

    private boolean isAlphabet(char ch) {
        return alphabet.indexOf(ch) != -1;
    }

    private boolean isDelimiter(char ch) {
        return delimiter.indexOf(ch) != -1;
    }

}
