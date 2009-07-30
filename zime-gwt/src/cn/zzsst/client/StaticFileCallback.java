package cn.zzsst.client;

import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.Response;

public abstract class StaticFileCallback implements RequestCallback {

    @Override
    public void onError(Request request, Throwable e) {
        e.printStackTrace();
    }

    @Override
    public void onResponseReceived(Request request, Response response) {
        onReady(response.getText());
    }

    public abstract void onReady(String text);
    
}
