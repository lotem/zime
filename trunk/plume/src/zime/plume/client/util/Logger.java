package zime.plume.client.util;

import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.RootPanel;

public class Logger {

    private static void log(String msg) {
        System.out.println(msg);
        //RootPanel.get().add(new Label(msg));
    }
    
    public static void debug(String message) {
        log("[DEBUG] " + message);
    }

    public static void info(String message) {
        log("[INFO] " + message);
    }

    public static void error(String message) {
        log("[ERROR] " + message);
    }

}
