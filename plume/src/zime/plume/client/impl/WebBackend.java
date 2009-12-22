package zime.plume.client.impl;

import java.util.ArrayList;
import java.util.List;

import com.google.gwt.core.client.GWT;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.http.client.URL;

import zime.plume.client.Backend;
import zime.plume.client.Config;
import zime.plume.client.Query;
import zime.plume.client.QueryResult;
import zime.plume.client.Schema;
import zime.plume.client.util.Logger;

public class WebBackend implements Backend {

    private static final String SCHEMA_LIST = "schema_list";
    private static final String CONFIG = "config";
    private static final String QUERY = "query";

    @Override
    public void doQuery(final Query query, final QueryCallback callback) {
        String url = GWT.getModuleBaseURL() + QUERY;
        String queryString = "schema=" + URL.encodeComponent(query.getSchemaName()) +
                             "&input=" + URL.encodeComponent(query.getInput());
        request(url, queryString, new RequestCallback() {
            @Override
            public void onError(Request request, Throwable exception) {
                callback.onFailure();
            }
            @Override
            public void onResponseReceived(Request request, Response response) {
                // 0 is returned for local static files 
                if (response.getStatusCode() <= Response.SC_OK) {
                    // TODO
                    //response.getText();
                    callback.onReady(new QueryResult(query, null));
                } else {
                    callback.onFailure();
                }
            }
        });
    }

    @Override
    public void loadConfig(final String schema, final LoadConfigCallback callback) {
        String url = GWT.getModuleBaseURL() + CONFIG; 
        String queryString = "schema=" + URL.encodeComponent(schema);
        request(url, queryString, new RequestCallback() {
            @Override
            public void onError(Request request, Throwable exception) {
                callback.onFailure();
            }
            @Override
            public void onResponseReceived(Request request, Response response) {
                // 0 is returned for local static files 
                if (response.getStatusCode() <= Response.SC_OK) {
                    // TODO
                    //response.getText();
                    callback.onReady(new Config("Config/" + schema + "/", null));
                } else {
                    callback.onFailure();
                }
            }
        });
    }

    @Override
    public void loadSchemaList(final LoadSchemaListCallback callback) {
        String url = GWT.getModuleBaseURL() + SCHEMA_LIST; 
        request(url, null, new RequestCallback() {
            @Override
            public void onError(Request request, Throwable exception) {
                callback.onFailure();
            }
            @Override
            public void onResponseReceived(Request request, Response response) {
                // 0 is returned for local static files 
                if (response.getStatusCode() <= Response.SC_OK) {
                    // TODO: debug code
                    List<Schema> list = new ArrayList<Schema>();
                    list.add(new Schema("Pinyin", "拼音"));
                    callback.onReady(list);
                } else {
                    callback.onFailure();
                }
            }
        });
    }

    private void request(String url, String query, RequestCallback callback) {
        Logger.debug("request: " + url);
        RequestBuilder requestBuilder = new RequestBuilder(RequestBuilder.POST, url);
        try {
            requestBuilder.sendRequest(query, callback);
        } catch (RequestException e) {
            e.printStackTrace();
        }
    }
}
