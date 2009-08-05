package cn.zzsst.zime.client;

import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.user.client.rpc.AsyncCallback;

public abstract class StaticFileCallback implements RequestCallback, AsyncCallback<String> {

    public static final String PREFIX = "../schema/";
    
    @Override
    public void onError(Request request, Throwable e) {
        onFailure(e);
    }

    @Override
    public void onResponseReceived(Request request, Response response) {
        // 0 is returned for local static files 
        if (response.getStatusCode() <= Response.SC_OK) {
            onSuccess(response.getText());
        } else {
            onFailure(null);
        }
    }

    public void fetch(String filePath) {
        Logger.log("fetch static file: " + filePath);
        RequestBuilder requestBuilder = new RequestBuilder(RequestBuilder.GET, filePath);
        try {
            requestBuilder.sendRequest(null, this);
        } catch (RequestException e) {
            e.printStackTrace();
        }
    }

}
