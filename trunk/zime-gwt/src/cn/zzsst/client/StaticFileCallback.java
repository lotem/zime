package cn.zzsst.client;

import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.Response;

public abstract class StaticFileCallback implements RequestCallback {

    @Override
    public void onError(Request request, Throwable e) {
        onFailure();
    }

    @Override
    public void onResponseReceived(Request request, Response response) {
        if (response.getStatusCode() == Response.SC_OK) {
            onReady(response.getText());
        } else {
            onFailure();
        }
    }

    public abstract void onReady(String text);
    public abstract void onFailure();
    
}
