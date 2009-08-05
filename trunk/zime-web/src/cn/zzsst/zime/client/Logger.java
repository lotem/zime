package cn.zzsst.zime.client;

//import com.google.gwt.dom.client.Document;
//import com.google.gwt.dom.client.Node;
//import com.google.gwt.user.client.Window;

public class Logger {

//    private static Document doc = Document.get();

    public static void log(String msg) {
        System.out.println(msg);
//        Window.alert(msg);
//        Node p = doc.createPElement();
//        p.appendChild(doc.createTextNode(msg));
//        doc.getBody().appendChild(p);
    }

}
