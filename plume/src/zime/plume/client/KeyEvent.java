package zime.plume.client;

import com.google.gwt.event.dom.client.KeyDownEvent;
import com.google.gwt.event.dom.client.KeyUpEvent;

public class KeyEvent {
    
    public static final int NULL_MASK = 0;
    public static final int RELEASE_MASK = 1;
    public static final int SHIFT_MASK = 2;
    public static final int CONTROL_MASK = 4;
    public static final int ALT_MASK = 8;
    public static final int META_MASK = 16;
    
    public static final int KEY_ALT = 18;
    public static final int KEY_BACKSPACE = 8;
    public static final int KEY_CTRL = 17;
    public static final int KEY_DELETE = 46;
    public static final int KEY_DOWN = 40;
    public static final int KEY_END = 35;
    public static final int KEY_ENTER = 13;
    public static final int KEY_ESCAPE = 27;
    public static final int KEY_HOME = 36;
    public static final int KEY_LEFT = 37;
    public static final int KEY_PAGEDOWN = 34;
    public static final int KEY_PAGEUP = 33;
    public static final int KEY_RIGHT = 39;
    public static final int KEY_SHIFT = 16;
    public static final int KEY_TAB = 9;
    public static final int KEY_UP = 38;

    public static final int KEY_SPACE = 32;
    public static final int KEY_0 = 48;
    public static final int KEY_9 = 57;
    public static final int KEY_A = 65;
    public static final int KEY_Z = 90;
    public static final int KEY_BACKQUOTE = 192;
    public static final int KEY_MINUS_1 = 109;
    public static final int KEY_MINUS_2 = 189;
    public static final int KEY_EQUAL_1 = 107;
    public static final int KEY_EQUAL_2 = 187;
    public static final int KEY_L_BRACKET = 219;
    public static final int KEY_R_BRACKET = 221;
    public static final int KEY_BACKSLASH = 220;
    public static final int KEY_SEMICOLON_1 = 59;
    public static final int KEY_SEMICOLON_2 = 186;
    public static final int KEY_APOSTROPHE = 222;
    public static final int KEY_COMMA = 188;
    public static final int KEY_PERIOD = 190;
    public static final int KEY_SLASH = 191;
    
    private static char[][] keyCodeToChar;
    
    static {
        final int[] keyCodes = {
            192, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 109, 189, 107, 187,  
            81, 87, 69, 82, 84, 89, 85, 73, 79, 80, 219, 221, 220, 
            65, 83, 68, 70, 71, 72, 74, 75, 76, 59, 186, 222,
            90, 88, 67, 86, 66, 78, 77, 188, 190, 191,
            32
        };
        final String lowerCase = "`1234567890--==qwertyuiop[]\\asdfghjkl;;\'zxcvbnm,./ ";
        final String upperCase = "~!@#$%^&*()__++QWERTYUIOP{}|ASDFGHJKL::\"ZXCVBNM<>? ";
        
        keyCodeToChar = new char[256][];
        for (int i = 0; i < keyCodes.length; ++i) {
            keyCodeToChar[keyCodes[i]] = new char[] {
                lowerCase.charAt(i),
                upperCase.charAt(i)
            };
        }
    }
    
    public int keyCode;
    public int mask;
    
    public KeyEvent(int keyCode, int mask) {
        this.keyCode = keyCode;
        this.mask = mask;
    }
    
    public KeyEvent(KeyDownEvent event) {
        keyCode = event.getNativeKeyCode();
        mask = NULL_MASK + 
            (event.isShiftKeyDown() ? SHIFT_MASK : 0) + 
            (event.isControlKeyDown() ? CONTROL_MASK : 0) +
            (event.isAltKeyDown() ? ALT_MASK : 0) +
            (event.isMetaKeyDown() ? META_MASK : 0);
    }
    
    public KeyEvent(KeyUpEvent event) {
        keyCode = event.getNativeKeyCode();
        mask = RELEASE_MASK + 
            (event.isShiftKeyDown() ? SHIFT_MASK : 0) + 
            (event.isControlKeyDown() ? CONTROL_MASK : 0) +
            (event.isAltKeyDown() ? ALT_MASK : 0) +
            (event.isMetaKeyDown() ? META_MASK : 0);
    }
    
    public boolean isKeyRelease() {
        return (mask & RELEASE_MASK) != 0;
    }
    
    public boolean isShiftKeyDown() {
        return (mask & SHIFT_MASK) != 0;
    }
    
    public boolean isControlKeyDown() {
        return (mask & CONTROL_MASK) != 0;
    }
    
    public boolean isAltKeyDown() {
        return (mask & ALT_MASK) != 0;
    }
    
    public boolean isMetaKeyDown() {
        return (mask & META_MASK) != 0;
    }

    public char getChar() {
        if (isControlKeyDown() || isAltKeyDown() || isMetaKeyDown())
            return '\0';
        if (keyCode < 0 || keyCode >= 256 || keyCodeToChar[keyCode] == null)
            return '\0';
        return keyCodeToChar[keyCode][isShiftKeyDown() ? 1 : 0];
    }
    
    @Override
    public String toString() {
        return "<KeyEvent: keyCode=" + keyCode + ", mask=" + mask + ">";
    }
}
